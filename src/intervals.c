/*
 * Copyright (c) 2022 Pablo Neira Ayuso <pablo@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 (or any
 * later) as published by the Free Software Foundation.
 */

#include <nftables.h>
#include <expression.h>
#include <intervals.h>
#include <rule.h>

static void setelem_expr_to_range(struct expr *expr)
{
	unsigned char data[sizeof(struct in6_addr) * BITS_PER_BYTE];
	struct expr *key, *value;
	mpz_t rop;

	assert(expr->etype == EXPR_SET_ELEM);

	switch (expr->key->etype) {
	case EXPR_SET_ELEM_CATCHALL:
	case EXPR_RANGE:
		break;
	case EXPR_PREFIX:
		mpz_init(rop);
		mpz_bitmask(rop, expr->key->len - expr->key->prefix_len);
		if (expr_basetype(expr)->type == TYPE_STRING)
			mpz_switch_byteorder(expr->key->prefix->value, expr->len / BITS_PER_BYTE);

		mpz_ior(rop, rop, expr->key->prefix->value);
	        mpz_export_data(data, rop, expr->key->prefix->byteorder,
				expr->key->prefix->len / BITS_PER_BYTE);
		mpz_clear(rop);
		value = constant_expr_alloc(&expr->location,
					    expr->key->prefix->dtype,
					    expr->key->prefix->byteorder,
					    expr->key->prefix->len, data);
		key = range_expr_alloc(&expr->location,
				       expr_get(expr->key->prefix),
				       value);
		expr_free(expr->key);
		expr->key = key;
		break;
	case EXPR_VALUE:
		if (expr_basetype(expr)->type == TYPE_STRING)
			mpz_switch_byteorder(expr->key->value, expr->len / BITS_PER_BYTE);

		key = range_expr_alloc(&expr->location,
				       expr_clone(expr->key),
				       expr_get(expr->key));
		expr_free(expr->key);
		expr->key = key;
		break;
	default:
		BUG("unhandled key type %d\n", expr->key->etype);
	}
}

struct set_automerge_ctx {
	struct set	*set;
	struct expr	*init;
	struct expr	*purge;
	unsigned int	debug_mask;
};

static void purge_elem(struct set_automerge_ctx *ctx, struct expr *i)
{
	if (ctx->debug_mask & NFT_DEBUG_SEGTREE) {
		pr_gmp_debug("remove: [%Zx-%Zx]\n",
			     i->key->left->value,
			     i->key->right->value);
	}
	list_move_tail(&i->list, &ctx->purge->expressions);
}

static void remove_overlapping_range(struct set_automerge_ctx *ctx,
				     struct expr *prev, struct expr *i)
{
	if (i->flags & EXPR_F_KERNEL) {
		purge_elem(ctx, i);
		return;
	}
	list_del(&i->list);
	expr_free(i);
	ctx->init->size--;
}

struct range {
	mpz_t	low;
	mpz_t	high;
};

static bool merge_ranges(struct set_automerge_ctx *ctx,
			 struct expr *prev, struct expr *i,
			 struct range *prev_range, struct range *range)
{
	if (prev->flags & EXPR_F_KERNEL) {
		purge_elem(ctx, prev);
		expr_free(i->key->left);
		i->key->left = expr_get(prev->key->left);
		mpz_set(prev_range->high, range->high);
		return true;
	} else if (i->flags & EXPR_F_KERNEL) {
		purge_elem(ctx, i);
		expr_free(prev->key->right);
		prev->key->right = expr_get(i->key->right);
		mpz_set(prev_range->high, range->high);
	} else {
		expr_free(prev->key->right);
		prev->key->right = expr_get(i->key->right);
		mpz_set(prev_range->high, range->high);
		list_del(&i->list);
		expr_free(i);
		ctx->init->size--;
	}
	return false;
}

static void setelem_automerge(struct set_automerge_ctx *ctx)
{
	struct expr *i, *next, *prev = NULL;
	struct range range, prev_range;
	mpz_t rop;

	mpz_init(prev_range.low);
	mpz_init(prev_range.high);
	mpz_init(range.low);
	mpz_init(range.high);
	mpz_init(rop);

	list_for_each_entry_safe(i, next, &ctx->init->expressions, list) {
		if (i->key->etype == EXPR_SET_ELEM_CATCHALL)
			continue;

		range_expr_value_low(range.low, i);
		range_expr_value_high(range.high, i);

		if (!prev) {
			prev = i;
			mpz_set(prev_range.low, range.low);
			mpz_set(prev_range.high, range.high);
			continue;
		}

		if (mpz_cmp(prev_range.low, range.low) <= 0 &&
		    mpz_cmp(prev_range.high, range.high) >= 0) {
			remove_overlapping_range(ctx, prev, i);
			continue;
		} else if (mpz_cmp(range.low, prev_range.high) <= 0) {
			if (merge_ranges(ctx, prev, i, &prev_range, &range))
				prev = i;
			continue;
		} else if (ctx->set->automerge) {
			mpz_sub(rop, range.low, prev_range.high);
			/* two contiguous ranges */
			if (mpz_cmp_ui(rop, 1) == 0) {
				if (merge_ranges(ctx, prev, i, &prev_range, &range))
					prev = i;
				continue;
			}
		}

		prev = i;
		mpz_set(prev_range.low, range.low);
		mpz_set(prev_range.high, range.high);
	}

	mpz_clear(prev_range.low);
	mpz_clear(prev_range.high);
	mpz_clear(range.low);
	mpz_clear(range.high);
	mpz_clear(rop);
}

static struct expr *interval_expr_key(struct expr *i)
{
	struct expr *elem;

	switch (i->etype) {
	case EXPR_MAPPING:
		elem = i->left;
		break;
	case EXPR_SET_ELEM:
		elem = i;
		break;
	default:
		BUG("unhandled expression type %d\n", i->etype);
		return NULL;
	}

	return elem;
}

void set_to_range(struct expr *init)
{
	struct expr *i, *elem;

	list_for_each_entry(i, &init->expressions, list) {
		elem = interval_expr_key(i);
		setelem_expr_to_range(elem);
	}
}

int set_automerge(struct list_head *msgs, struct cmd *cmd, struct set *set,
		  struct expr *init, unsigned int debug_mask)
{
	struct set *existing_set = set->existing_set;
	struct set_automerge_ctx ctx = {
		.set		= set,
		.init		= init,
		.debug_mask	= debug_mask,
	};
	struct expr *i, *next, *clone;
	struct cmd *purge_cmd;
	struct handle h = {};

	if (existing_set) {
		if (existing_set->init) {
			list_splice_init(&existing_set->init->expressions,
					 &init->expressions);
		} else {
			existing_set->init = set_expr_alloc(&internal_location,
							    set);
		}
	}

	set_to_range(init);
	list_expr_sort(&init->expressions);

	if (set->flags & NFT_SET_MAP)
		return 0;

	ctx.purge = set_expr_alloc(&internal_location, set);

	setelem_automerge(&ctx);

	list_for_each_entry_safe(i, next, &init->expressions, list) {
		if (i->flags & EXPR_F_KERNEL) {
			list_move_tail(&i->list, &existing_set->init->expressions);
		} else if (existing_set) {
			if (debug_mask & NFT_DEBUG_SEGTREE) {
				pr_gmp_debug("add: [%Zx-%Zx]\n",
					     i->key->left->value, i->key->right->value);
			}
			clone = expr_clone(i);
			clone->flags |= EXPR_F_KERNEL;
			list_add_tail(&clone->list, &existing_set->init->expressions);
		}
	}

	if (list_empty(&ctx.purge->expressions)) {
		expr_free(ctx.purge);
		return 0;
	}

	handle_merge(&h, &set->handle);
	purge_cmd = cmd_alloc(CMD_DELETE, CMD_OBJ_ELEMENTS, &h, &init->location, ctx.purge);
	purge_cmd->elem.set = set_get(set);
	list_add_tail(&purge_cmd->list, &cmd->list);

	return 0;
}

static void remove_elem(struct expr *prev, struct set *set, struct expr *purge)
{
	struct expr *clone;

	if (prev->flags & EXPR_F_KERNEL) {
		clone = expr_clone(prev);
		list_move_tail(&clone->list, &purge->expressions);
	}
}

static void __adjust_elem_left(struct set *set, struct expr *prev, struct expr *i)
{
	prev->flags &= ~EXPR_F_KERNEL;
	expr_free(prev->key->left);
	prev->key->left = expr_get(i->key->right);
	mpz_add_ui(prev->key->left->value, prev->key->left->value, 1);
	list_move(&prev->list, &set->existing_set->init->expressions);
}

static void adjust_elem_left(struct set *set, struct expr *prev, struct expr *i,
			     struct expr *purge)
{
	remove_elem(prev, set, purge);
	__adjust_elem_left(set, prev, i);

	list_del(&i->list);
	expr_free(i);
}

static void __adjust_elem_right(struct set *set, struct expr *prev, struct expr *i)
{
	prev->flags &= ~EXPR_F_KERNEL;
	expr_free(prev->key->right);
	prev->key->right = expr_get(i->key->left);
	mpz_sub_ui(prev->key->right->value, prev->key->right->value, 1);
	list_move(&prev->list, &set->existing_set->init->expressions);
}

static void adjust_elem_right(struct set *set, struct expr *prev, struct expr *i,
			      struct expr *purge)
{
	remove_elem(prev, set, purge);
	__adjust_elem_right(set, prev, i);

	list_del(&i->list);
	expr_free(i);
}

static void split_range(struct set *set, struct expr *prev, struct expr *i,
			struct expr *purge)
{
	struct expr *clone;

	if (prev->flags & EXPR_F_KERNEL) {
		clone = expr_clone(prev);
		list_move_tail(&clone->list, &purge->expressions);
	}

	prev->flags &= ~EXPR_F_KERNEL;
	clone = expr_clone(prev);
	expr_free(clone->key->left);
	clone->key->left = expr_get(i->key->right);
	mpz_add_ui(clone->key->left->value, i->key->right->value, 1);
	list_add_tail(&clone->list, &set->existing_set->init->expressions);

	expr_free(prev->key->right);
	prev->key->right = expr_get(i->key->left);
	mpz_sub_ui(prev->key->right->value, i->key->left->value, 1);
	list_move(&prev->list, &set->existing_set->init->expressions);

	list_del(&i->list);
	expr_free(i);
}

static int setelem_adjust(struct set *set, struct expr *purge,
			  struct range *prev_range, struct range *range,
			  struct expr *prev, struct expr *i)
{
	if (mpz_cmp(prev_range->low, range->low) == 0 &&
	    mpz_cmp(prev_range->high, range->high) > 0) {
		if (i->flags & EXPR_F_REMOVE)
			adjust_elem_left(set, prev, i, purge);
	} else if (mpz_cmp(prev_range->low, range->low) < 0 &&
		   mpz_cmp(prev_range->high, range->high) == 0) {
		if (i->flags & EXPR_F_REMOVE)
			adjust_elem_right(set, prev, i, purge);
	} else if (mpz_cmp(prev_range->low, range->low) < 0 &&
		   mpz_cmp(prev_range->high, range->high) > 0) {
		if (i->flags & EXPR_F_REMOVE)
			split_range(set, prev, i, purge);
	} else {
		return -1;
	}

	return 0;
}

static int setelem_delete(struct list_head *msgs, struct set *set,
			  struct expr *purge, struct expr *elems,
			  unsigned int debug_mask)
{
	struct expr *i, *next, *prev = NULL;
	struct range range, prev_range;
	int err = 0;
	mpz_t rop;

	mpz_init(prev_range.low);
	mpz_init(prev_range.high);
	mpz_init(range.low);
	mpz_init(range.high);
	mpz_init(rop);

	list_for_each_entry_safe(i, next, &elems->expressions, list) {
		if (i->key->etype == EXPR_SET_ELEM_CATCHALL)
			continue;

		range_expr_value_low(range.low, i);
		range_expr_value_high(range.high, i);

		if (!prev && i->flags & EXPR_F_REMOVE) {
			expr_error(msgs, i, "element does not exist");
			err = -1;
			goto err;
		}

		if (!(i->flags & EXPR_F_REMOVE)) {
			prev = i;
			mpz_set(prev_range.low, range.low);
			mpz_set(prev_range.high, range.high);
			continue;
		}

		if (mpz_cmp(prev_range.low, range.low) == 0 &&
		    mpz_cmp(prev_range.high, range.high) == 0) {
			if (i->flags & EXPR_F_REMOVE) {
				if (prev->flags & EXPR_F_KERNEL)
					list_move_tail(&prev->list, &purge->expressions);

				list_del(&i->list);
				expr_free(i);
			}
		} else if (set->automerge &&
			   setelem_adjust(set, purge, &prev_range, &range, prev, i) < 0) {
			expr_error(msgs, i, "element does not exist");
			err = -1;
			goto err;
		}
		prev = NULL;
	}
err:
	mpz_clear(prev_range.low);
	mpz_clear(prev_range.high);
	mpz_clear(range.low);
	mpz_clear(range.high);
	mpz_clear(rop);

	return err;
}

static void automerge_delete(struct list_head *msgs, struct set *set,
			     struct expr *init, unsigned int debug_mask)
{
	struct set_automerge_ctx ctx = {
		.set		= set,
		.init		= init,
		.debug_mask	= debug_mask,
	};

	ctx.purge = set_expr_alloc(&internal_location, set);
	list_expr_sort(&init->expressions);
	setelem_automerge(&ctx);
	expr_free(ctx.purge);
}

static int __set_delete(struct list_head *msgs, struct expr *i,	struct set *set,
			struct expr *init, struct set *existing_set,
			unsigned int debug_mask)
{
	i->flags |= EXPR_F_REMOVE;
	list_move(&i->list, &existing_set->init->expressions);
	list_expr_sort(&existing_set->init->expressions);

	return setelem_delete(msgs, set, init, existing_set->init, debug_mask);
}

/* detection for unexisting intervals already exists in Linux kernels >= 5.7. */
int set_delete(struct list_head *msgs, struct cmd *cmd, struct set *set,
	       struct expr *init, unsigned int debug_mask)
{
	struct set *existing_set = set->existing_set;
	struct expr *i, *next, *add, *clone;
	struct handle h = {};
	struct cmd *add_cmd;
	LIST_HEAD(del_list);
	int err;

	set_to_range(init);
	if (set->automerge)
		automerge_delete(msgs, set, init, debug_mask);

	set_to_range(existing_set->init);

	list_splice_init(&init->expressions, &del_list);

	list_for_each_entry_safe(i, next, &del_list, list) {
		err = __set_delete(msgs, i, set, init, existing_set, debug_mask);
		if (err < 0) {
			list_splice(&del_list, &init->expressions);
			return err;
		}
	}

	add = set_expr_alloc(&internal_location, set);
	list_for_each_entry(i, &existing_set->init->expressions, list) {
		if (!(i->flags & EXPR_F_KERNEL)) {
			clone = expr_clone(i);
			list_add_tail(&clone->list, &add->expressions);
			i->flags |= EXPR_F_KERNEL;
		}
	}

	if (debug_mask & NFT_DEBUG_SEGTREE) {
		list_for_each_entry(i, &init->expressions, list)
			pr_gmp_debug("remove: [%Zx-%Zx]\n",
				     i->key->left->value, i->key->right->value);
		list_for_each_entry(i, &add->expressions, list)
			pr_gmp_debug("add: [%Zx-%Zx]\n",
				     i->key->left->value, i->key->right->value);
		list_for_each_entry(i, &existing_set->init->expressions, list)
			pr_gmp_debug("existing: [%Zx-%Zx]\n",
				     i->key->left->value, i->key->right->value);
	}

	if (list_empty(&add->expressions)) {
		expr_free(add);
		return 0;
	}

	handle_merge(&h, &cmd->handle);
	add_cmd = cmd_alloc(CMD_ADD, CMD_OBJ_ELEMENTS, &h, &cmd->location, add);
	add_cmd->elem.set = set_get(set);
	list_add(&add_cmd->list, &cmd->list);

	return 0;
}

static int setelem_overlap(struct list_head *msgs, struct set *set,
			   struct expr *init)
{
	struct expr *i, *next, *elem, *prev = NULL;
	struct range range, prev_range;
	int err = 0;
	mpz_t rop;

	mpz_init(prev_range.low);
	mpz_init(prev_range.high);
	mpz_init(range.low);
	mpz_init(range.high);
	mpz_init(rop);

	list_for_each_entry_safe(elem, next, &init->expressions, list) {
		i = interval_expr_key(elem);

		if (i->key->etype == EXPR_SET_ELEM_CATCHALL)
			continue;

		range_expr_value_low(range.low, i);
		range_expr_value_high(range.high, i);

		if (!prev) {
			prev = elem;
			mpz_set(prev_range.low, range.low);
			mpz_set(prev_range.high, range.high);
			continue;
		}

		if (mpz_cmp(prev_range.low, range.low) == 0 &&
		    mpz_cmp(prev_range.high, range.high) == 0 &&
		    (elem->flags & EXPR_F_KERNEL || prev->flags & EXPR_F_KERNEL))
			goto next;

		if (mpz_cmp(prev_range.low, range.low) <= 0 &&
		    mpz_cmp(prev_range.high, range.high) >= 0) {
			if (prev->flags & EXPR_F_KERNEL)
				expr_error(msgs, i, "interval overlaps with an existing one");
			else if (elem->flags & EXPR_F_KERNEL)
				expr_error(msgs, prev, "interval overlaps with an existing one");
			else
				expr_binary_error(msgs, i, prev,
						  "conflicting intervals specified");
			err = -1;
			goto err_out;
		} else if (mpz_cmp(range.low, prev_range.high) <= 0) {
			if (prev->flags & EXPR_F_KERNEL)
				expr_error(msgs, i, "interval overlaps with an existing one");
			else if (elem->flags & EXPR_F_KERNEL)
				expr_error(msgs, prev, "interval overlaps with an existing one");
			else
				expr_binary_error(msgs, i, prev,
						  "conflicting intervals specified");
			err = -1;
			goto err_out;
		}
next:
		prev = elem;
		mpz_set(prev_range.low, range.low);
		mpz_set(prev_range.high, range.high);
	}

err_out:
	mpz_clear(prev_range.low);
	mpz_clear(prev_range.high);
	mpz_clear(range.low);
	mpz_clear(range.high);
	mpz_clear(rop);

	return err;
}

/* overlap detection for intervals already exists in Linux kernels >= 5.7. */
int set_overlap(struct list_head *msgs, struct set *set, struct expr *init)
{
	struct set *existing_set = set->existing_set;
	struct expr *i, *n, *clone;
	int err;

	if (existing_set) {
		if (existing_set->init) {
			list_splice_init(&existing_set->init->expressions,
					 &init->expressions);
		} else {
			existing_set->init = set_expr_alloc(&internal_location,
							    set);
		}
	}

	set_to_range(init);
	list_expr_sort(&init->expressions);

	err = setelem_overlap(msgs, set, init);

	list_for_each_entry_safe(i, n, &init->expressions, list) {
		if (i->flags & EXPR_F_KERNEL)
			list_move_tail(&i->list, &existing_set->init->expressions);
		else if (existing_set) {
			clone = expr_clone(i);
			clone->flags |= EXPR_F_KERNEL;
			list_add_tail(&clone->list, &existing_set->init->expressions);
		}
	}

	return err;
}

static bool segtree_needs_first_segment(const struct set *set,
					const struct expr *init, bool add)
{
	if (add && !set->root) {
		/* Add the first segment in four situations:
		 *
		 * 1) This is an anonymous set.
		 * 2) This set exists and it is empty.
		 * 3) New empty set and, separately, new elements are added.
		 * 4) This set is created with a number of initial elements.
		 */
		if ((set_is_anonymous(set->flags)) ||
		    (set->init && set->init->size == 0) ||
		    (set->init == NULL && init) ||
		    (set->init == init)) {
			return true;
		}
	}
	/* This is an update for a set that already contains elements, so don't
	 * add the first non-matching elements otherwise we hit EEXIST.
	 */
	return false;
}

int set_to_intervals(const struct set *set, struct expr *init, bool add)
{
	struct expr *i, *n, *prev = NULL, *elem, *newelem = NULL, *root, *expr;
	LIST_HEAD(intervals);
	uint32_t flags;
	mpz_t p, q;

	mpz_init2(p, set->key->len);
	mpz_init2(q, set->key->len);

	list_for_each_entry_safe(i, n, &init->expressions, list) {
		flags = 0;

		elem = interval_expr_key(i);

		if (elem->key->etype == EXPR_SET_ELEM_CATCHALL)
			continue;

		if (!prev && segtree_needs_first_segment(set, init, add) &&
		    mpz_cmp_ui(elem->key->left->value, 0)) {
			mpz_set_ui(p, 0);
			expr = constant_expr_alloc(&internal_location,
						   set->key->dtype,
						   set->key->byteorder,
						   set->key->len, NULL);
			mpz_set(expr->value, p);
			root = set_elem_expr_alloc(&internal_location, expr);
			if (i->etype == EXPR_MAPPING) {
				root = mapping_expr_alloc(&internal_location,
							  root,
							  expr_get(i->right));
			}
			root->flags |= EXPR_F_INTERVAL_END;
			list_add(&root->list, &intervals);
			init->size++;
		}

		if (newelem) {
			mpz_set(p, interval_expr_key(newelem)->key->value);
			if (set->key->byteorder == BYTEORDER_HOST_ENDIAN)
				mpz_switch_byteorder(p, set->key->len / BITS_PER_BYTE);

			if (!(set->flags & NFT_SET_ANONYMOUS) ||
			    mpz_cmp(p, elem->key->left->value) != 0)
				list_add_tail(&newelem->list, &intervals);
			else
				expr_free(newelem);
		}
		newelem = NULL;

		if (mpz_scan0(elem->key->right->value, 0) != set->key->len) {
			mpz_add_ui(p, elem->key->right->value, 1);
			expr = constant_expr_alloc(&elem->key->location, set->key->dtype,
						   set->key->byteorder, set->key->len,
						   NULL);
			mpz_set(expr->value, p);
			if (set->key->byteorder == BYTEORDER_HOST_ENDIAN)
				mpz_switch_byteorder(expr->value, set->key->len / BITS_PER_BYTE);

			newelem = set_elem_expr_alloc(&internal_location, expr);
			if (i->etype == EXPR_MAPPING) {
				newelem = mapping_expr_alloc(&internal_location,
							     newelem,
							     expr_get(i->right));
			}
			newelem->flags |= EXPR_F_INTERVAL_END;
		} else {
			flags = NFTNL_SET_ELEM_F_INTERVAL_OPEN;
		}

		expr = constant_expr_alloc(&elem->key->location, set->key->dtype,
					   set->key->byteorder, set->key->len, NULL);

		mpz_set(expr->value, elem->key->left->value);
		if (set->key->byteorder == BYTEORDER_HOST_ENDIAN)
			mpz_switch_byteorder(expr->value, set->key->len / BITS_PER_BYTE);

		expr_free(elem->key);
		elem->key = expr;
		i->elem_flags |= flags;
		init->size++;
		list_move_tail(&i->list, &intervals);

		prev = i;
	}

	if (newelem)
		list_add_tail(&newelem->list, &intervals);

	list_splice_init(&intervals, &init->expressions);

	mpz_clear(p);
	mpz_clear(q);

	return 0;
}
