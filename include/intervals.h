#ifndef NFTABLES_INTERVALS_H
#define NFTABLES_INTERVALS_H

void set_to_range(struct expr *init);
int set_automerge(struct list_head *msgs, struct cmd *cmd, struct set *set,
		  struct expr *init, unsigned int debug_mask);
int set_delete(struct list_head *msgs, struct cmd *cmd, struct set *set,
	       struct expr *init, unsigned int debug_mask);
int set_overlap(struct list_head *msgs, struct set *set, struct expr *init);
int set_to_intervals(const struct set *set, struct expr *init, bool add);

#endif
