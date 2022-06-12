/* A Bison parser, made by GNU Bison 3.7.5.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30705

/* Bison version string.  */
#define YYBISON_VERSION "3.7.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         nft_parse
#define yylex           nft_lex
#define yyerror         nft_error
#define yydebug         nft_debug
#define yynerrs         nft_nerrs

/* First part of user prologue.  */
#line 11 "parser_bison.y"


#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>
#include <syslog.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
#include <linux/netfilter.h>
#include <linux/netfilter/nf_tables.h>
#include <linux/netfilter/nf_conntrack_tuple_common.h>
#include <linux/netfilter/nf_nat.h>
#include <linux/netfilter/nf_log.h>
#include <linux/netfilter/nfnetlink_osf.h>
#include <linux/netfilter/nf_synproxy.h>
#include <linux/xfrm.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp6.h>
#include <libnftnl/common.h>
#include <libnftnl/set.h>
#include <libnftnl/udata.h>

#include <rule.h>
#include <statement.h>
#include <expression.h>
#include <headers.h>
#include <utils.h>
#include <parser.h>
#include <erec.h>
#include <sctp_chunk.h>

#include "parser_bison.h"

void parser_init(struct nft_ctx *nft, struct parser_state *state,
		 struct list_head *msgs, struct list_head *cmds,
		 struct scope *top_scope)
{
	memset(state, 0, sizeof(*state));
	state->msgs = msgs;
	state->cmds = cmds;
	state->scopes[0] = scope_init(top_scope, NULL);
	init_list_head(&state->indesc_list);
}

static void yyerror(struct location *loc, struct nft_ctx *nft, void *scanner,
		    struct parser_state *state, const char *s)
{
	erec_queue(error(loc, "%s", s), state->msgs);
}

static struct scope *current_scope(const struct parser_state *state)
{
	return state->scopes[state->scope];
}

static void open_scope(struct parser_state *state, struct scope *scope)
{
	assert(state->scope < array_size(state->scopes) - 1);
	scope_init(scope, current_scope(state));
	state->scopes[++state->scope] = scope;
}

static void close_scope(struct parser_state *state)
{
	assert(state->scope > 0);
	state->scope--;
}

static void location_init(void *scanner, struct parser_state *state,
			  struct location *loc)
{
	memset(loc, 0, sizeof(*loc));
	loc->indesc = state->indesc;
}

static void location_update(struct location *loc, struct location *rhs, int n)
{
	if (n) {
		loc->indesc       = rhs[n].indesc;
		loc->token_offset = rhs[1].token_offset;
		loc->line_offset  = rhs[1].line_offset;
		loc->first_line   = rhs[1].first_line;
		loc->first_column = rhs[1].first_column;
		loc->last_line    = rhs[n].last_line;
		loc->last_column  = rhs[n].last_column;
	} else {
		loc->indesc       = rhs[0].indesc;
		loc->token_offset = rhs[0].token_offset;
		loc->line_offset  = rhs[0].line_offset;
		loc->first_line   = loc->last_line   = rhs[0].last_line;
		loc->first_column = loc->last_column = rhs[0].last_column;
	}
}

static struct expr *handle_concat_expr(const struct location *loc,
					 struct expr *expr,
					 struct expr *expr_l, struct expr *expr_r,
					 struct location loc_rhs[3])
{
	if (expr->etype != EXPR_CONCAT) {
		expr = concat_expr_alloc(loc);
		compound_expr_add(expr, expr_l);
	} else {
		location_update(&expr_r->location, loc_rhs, 2);

		expr = expr_l;
		expr->location = *loc;
	}

	compound_expr_add(expr, expr_r);
	return expr;
}

static bool already_set(const void *attr, const struct location *loc,
			struct parser_state *state)
{
	if (!attr)
		return false;

	erec_queue(error(loc, "You can only specify this once. This statement is duplicated."),
		   state->msgs);
	return true;
}

#define YYLLOC_DEFAULT(Current, Rhs, N)	location_update(&Current, Rhs, N)

#define symbol_value(loc, str) \
	symbol_expr_alloc(loc, SYMBOL_VALUE, current_scope(state), str)

/* Declare those here to avoid compiler warnings */
void nft_set_debug(int, void *);
int nft_lex(void *, void *, void *);

#line 212 "parser_bison.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_NFT_PARSER_BISON_H_INCLUDED
# define YY_NFT_PARSER_BISON_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int nft_debug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    TOKEN_EOF = 0,                 /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    JUNK = 258,                    /* "junk"  */
    NEWLINE = 259,                 /* "newline"  */
    COLON = 260,                   /* "colon"  */
    SEMICOLON = 261,               /* "semicolon"  */
    COMMA = 262,                   /* "comma"  */
    DOT = 263,                     /* "."  */
    EQ = 264,                      /* "=="  */
    NEQ = 265,                     /* "!="  */
    LT = 266,                      /* "<"  */
    GT = 267,                      /* ">"  */
    GTE = 268,                     /* ">="  */
    LTE = 269,                     /* "<="  */
    LSHIFT = 270,                  /* "<<"  */
    RSHIFT = 271,                  /* ">>"  */
    AMPERSAND = 272,               /* "&"  */
    CARET = 273,                   /* "^"  */
    NOT = 274,                     /* "!"  */
    SLASH = 275,                   /* "/"  */
    ASTERISK = 276,                /* "*"  */
    DASH = 277,                    /* "-"  */
    AT = 278,                      /* "@"  */
    VMAP = 279,                    /* "vmap"  */
    PLUS = 280,                    /* "+"  */
    INCLUDE = 281,                 /* "include"  */
    DEFINE = 282,                  /* "define"  */
    REDEFINE = 283,                /* "redefine"  */
    UNDEFINE = 284,                /* "undefine"  */
    FIB = 285,                     /* "fib"  */
    SOCKET = 286,                  /* "socket"  */
    TRANSPARENT = 287,             /* "transparent"  */
    WILDCARD = 288,                /* "wildcard"  */
    CGROUPV2 = 289,                /* "cgroupv2"  */
    TPROXY = 290,                  /* "tproxy"  */
    OSF = 291,                     /* "osf"  */
    SYNPROXY = 292,                /* "synproxy"  */
    MSS = 293,                     /* "mss"  */
    WSCALE = 294,                  /* "wscale"  */
    TYPEOF = 295,                  /* "typeof"  */
    HOOK = 296,                    /* "hook"  */
    HOOKS = 297,                   /* "hooks"  */
    DEVICE = 298,                  /* "device"  */
    DEVICES = 299,                 /* "devices"  */
    TABLE = 300,                   /* "table"  */
    TABLES = 301,                  /* "tables"  */
    CHAIN = 302,                   /* "chain"  */
    CHAINS = 303,                  /* "chains"  */
    RULE = 304,                    /* "rule"  */
    RULES = 305,                   /* "rules"  */
    SETS = 306,                    /* "sets"  */
    SET = 307,                     /* "set"  */
    ELEMENT = 308,                 /* "element"  */
    MAP = 309,                     /* "map"  */
    MAPS = 310,                    /* "maps"  */
    FLOWTABLE = 311,               /* "flowtable"  */
    HANDLE = 312,                  /* "handle"  */
    RULESET = 313,                 /* "ruleset"  */
    TRACE = 314,                   /* "trace"  */
    INET = 315,                    /* "inet"  */
    NETDEV = 316,                  /* "netdev"  */
    ADD = 317,                     /* "add"  */
    UPDATE = 318,                  /* "update"  */
    REPLACE = 319,                 /* "replace"  */
    CREATE = 320,                  /* "create"  */
    INSERT = 321,                  /* "insert"  */
    DELETE = 322,                  /* "delete"  */
    GET = 323,                     /* "get"  */
    LIST = 324,                    /* "list"  */
    RESET = 325,                   /* "reset"  */
    FLUSH = 326,                   /* "flush"  */
    RENAME = 327,                  /* "rename"  */
    DESCRIBE = 328,                /* "describe"  */
    IMPORT = 329,                  /* "import"  */
    EXPORT = 330,                  /* "export"  */
    MONITOR = 331,                 /* "monitor"  */
    ALL = 332,                     /* "all"  */
    ACCEPT = 333,                  /* "accept"  */
    DROP = 334,                    /* "drop"  */
    CONTINUE = 335,                /* "continue"  */
    JUMP = 336,                    /* "jump"  */
    GOTO = 337,                    /* "goto"  */
    RETURN = 338,                  /* "return"  */
    TO = 339,                      /* "to"  */
    CONSTANT = 340,                /* "constant"  */
    INTERVAL = 341,                /* "interval"  */
    DYNAMIC = 342,                 /* "dynamic"  */
    AUTOMERGE = 343,               /* "auto-merge"  */
    TIMEOUT = 344,                 /* "timeout"  */
    GC_INTERVAL = 345,             /* "gc-interval"  */
    ELEMENTS = 346,                /* "elements"  */
    EXPIRES = 347,                 /* "expires"  */
    POLICY = 348,                  /* "policy"  */
    MEMORY = 349,                  /* "memory"  */
    PERFORMANCE = 350,             /* "performance"  */
    SIZE = 351,                    /* "size"  */
    FLOW = 352,                    /* "flow"  */
    OFFLOAD = 353,                 /* "offload"  */
    METER = 354,                   /* "meter"  */
    METERS = 355,                  /* "meters"  */
    FLOWTABLES = 356,              /* "flowtables"  */
    NUM = 357,                     /* "number"  */
    STRING = 358,                  /* "string"  */
    QUOTED_STRING = 359,           /* "quoted string"  */
    ASTERISK_STRING = 360,         /* "string with a trailing asterisk"  */
    LL_HDR = 361,                  /* "ll"  */
    NETWORK_HDR = 362,             /* "nh"  */
    TRANSPORT_HDR = 363,           /* "th"  */
    BRIDGE = 364,                  /* "bridge"  */
    ETHER = 365,                   /* "ether"  */
    SADDR = 366,                   /* "saddr"  */
    DADDR = 367,                   /* "daddr"  */
    TYPE = 368,                    /* "type"  */
    VLAN = 369,                    /* "vlan"  */
    ID = 370,                      /* "id"  */
    CFI = 371,                     /* "cfi"  */
    DEI = 372,                     /* "dei"  */
    PCP = 373,                     /* "pcp"  */
    ARP = 374,                     /* "arp"  */
    HTYPE = 375,                   /* "htype"  */
    PTYPE = 376,                   /* "ptype"  */
    HLEN = 377,                    /* "hlen"  */
    PLEN = 378,                    /* "plen"  */
    OPERATION = 379,               /* "operation"  */
    IP = 380,                      /* "ip"  */
    HDRVERSION = 381,              /* "version"  */
    HDRLENGTH = 382,               /* "hdrlength"  */
    DSCP = 383,                    /* "dscp"  */
    ECN = 384,                     /* "ecn"  */
    LENGTH = 385,                  /* "length"  */
    FRAG_OFF = 386,                /* "frag-off"  */
    TTL = 387,                     /* "ttl"  */
    PROTOCOL = 388,                /* "protocol"  */
    CHECKSUM = 389,                /* "checksum"  */
    PTR = 390,                     /* "ptr"  */
    VALUE = 391,                   /* "value"  */
    LSRR = 392,                    /* "lsrr"  */
    RR = 393,                      /* "rr"  */
    SSRR = 394,                    /* "ssrr"  */
    RA = 395,                      /* "ra"  */
    ICMP = 396,                    /* "icmp"  */
    CODE = 397,                    /* "code"  */
    SEQUENCE = 398,                /* "seq"  */
    GATEWAY = 399,                 /* "gateway"  */
    MTU = 400,                     /* "mtu"  */
    IGMP = 401,                    /* "igmp"  */
    MRT = 402,                     /* "mrt"  */
    OPTIONS = 403,                 /* "options"  */
    IP6 = 404,                     /* "ip6"  */
    PRIORITY = 405,                /* "priority"  */
    FLOWLABEL = 406,               /* "flowlabel"  */
    NEXTHDR = 407,                 /* "nexthdr"  */
    HOPLIMIT = 408,                /* "hoplimit"  */
    ICMP6 = 409,                   /* "icmpv6"  */
    PPTR = 410,                    /* "param-problem"  */
    MAXDELAY = 411,                /* "max-delay"  */
    AH = 412,                      /* "ah"  */
    RESERVED = 413,                /* "reserved"  */
    SPI = 414,                     /* "spi"  */
    ESP = 415,                     /* "esp"  */
    COMP = 416,                    /* "comp"  */
    FLAGS = 417,                   /* "flags"  */
    CPI = 418,                     /* "cpi"  */
    PORT = 419,                    /* "port"  */
    UDP = 420,                     /* "udp"  */
    SPORT = 421,                   /* "sport"  */
    DPORT = 422,                   /* "dport"  */
    UDPLITE = 423,                 /* "udplite"  */
    CSUMCOV = 424,                 /* "csumcov"  */
    TCP = 425,                     /* "tcp"  */
    ACKSEQ = 426,                  /* "ackseq"  */
    DOFF = 427,                    /* "doff"  */
    WINDOW = 428,                  /* "window"  */
    URGPTR = 429,                  /* "urgptr"  */
    OPTION = 430,                  /* "option"  */
    ECHO = 431,                    /* "echo"  */
    EOL = 432,                     /* "eol"  */
    MPTCP = 433,                   /* "mptcp"  */
    NOP = 434,                     /* "nop"  */
    SACK = 435,                    /* "sack"  */
    SACK0 = 436,                   /* "sack0"  */
    SACK1 = 437,                   /* "sack1"  */
    SACK2 = 438,                   /* "sack2"  */
    SACK3 = 439,                   /* "sack3"  */
    SACK_PERM = 440,               /* "sack-permitted"  */
    FASTOPEN = 441,                /* "fastopen"  */
    MD5SIG = 442,                  /* "md5sig"  */
    TIMESTAMP = 443,               /* "timestamp"  */
    COUNT = 444,                   /* "count"  */
    LEFT = 445,                    /* "left"  */
    RIGHT = 446,                   /* "right"  */
    TSVAL = 447,                   /* "tsval"  */
    TSECR = 448,                   /* "tsecr"  */
    SUBTYPE = 449,                 /* "subtype"  */
    DCCP = 450,                    /* "dccp"  */
    SCTP = 451,                    /* "sctp"  */
    CHUNK = 452,                   /* "chunk"  */
    DATA = 453,                    /* "data"  */
    INIT = 454,                    /* "init"  */
    INIT_ACK = 455,                /* "init-ack"  */
    HEARTBEAT = 456,               /* "heartbeat"  */
    HEARTBEAT_ACK = 457,           /* "heartbeat-ack"  */
    ABORT = 458,                   /* "abort"  */
    SHUTDOWN = 459,                /* "shutdown"  */
    SHUTDOWN_ACK = 460,            /* "shutdown-ack"  */
    ERROR = 461,                   /* "error"  */
    COOKIE_ECHO = 462,             /* "cookie-echo"  */
    COOKIE_ACK = 463,              /* "cookie-ack"  */
    ECNE = 464,                    /* "ecne"  */
    CWR = 465,                     /* "cwr"  */
    SHUTDOWN_COMPLETE = 466,       /* "shutdown-complete"  */
    ASCONF_ACK = 467,              /* "asconf-ack"  */
    FORWARD_TSN = 468,             /* "forward-tsn"  */
    ASCONF = 469,                  /* "asconf"  */
    TSN = 470,                     /* "tsn"  */
    STREAM = 471,                  /* "stream"  */
    SSN = 472,                     /* "ssn"  */
    PPID = 473,                    /* "ppid"  */
    INIT_TAG = 474,                /* "init-tag"  */
    A_RWND = 475,                  /* "a-rwnd"  */
    NUM_OSTREAMS = 476,            /* "num-outbound-streams"  */
    NUM_ISTREAMS = 477,            /* "num-inbound-streams"  */
    INIT_TSN = 478,                /* "initial-tsn"  */
    CUM_TSN_ACK = 479,             /* "cum-tsn-ack"  */
    NUM_GACK_BLOCKS = 480,         /* "num-gap-ack-blocks"  */
    NUM_DUP_TSNS = 481,            /* "num-dup-tsns"  */
    LOWEST_TSN = 482,              /* "lowest-tsn"  */
    SEQNO = 483,                   /* "seqno"  */
    NEW_CUM_TSN = 484,             /* "new-cum-tsn"  */
    VTAG = 485,                    /* "vtag"  */
    RT = 486,                      /* "rt"  */
    RT0 = 487,                     /* "rt0"  */
    RT2 = 488,                     /* "rt2"  */
    RT4 = 489,                     /* "srh"  */
    SEG_LEFT = 490,                /* "seg-left"  */
    ADDR = 491,                    /* "addr"  */
    LAST_ENT = 492,                /* "last-entry"  */
    TAG = 493,                     /* "tag"  */
    SID = 494,                     /* "sid"  */
    HBH = 495,                     /* "hbh"  */
    FRAG = 496,                    /* "frag"  */
    RESERVED2 = 497,               /* "reserved2"  */
    MORE_FRAGMENTS = 498,          /* "more-fragments"  */
    DST = 499,                     /* "dst"  */
    MH = 500,                      /* "mh"  */
    META = 501,                    /* "meta"  */
    MARK = 502,                    /* "mark"  */
    IIF = 503,                     /* "iif"  */
    IIFNAME = 504,                 /* "iifname"  */
    IIFTYPE = 505,                 /* "iiftype"  */
    OIF = 506,                     /* "oif"  */
    OIFNAME = 507,                 /* "oifname"  */
    OIFTYPE = 508,                 /* "oiftype"  */
    SKUID = 509,                   /* "skuid"  */
    SKGID = 510,                   /* "skgid"  */
    NFTRACE = 511,                 /* "nftrace"  */
    RTCLASSID = 512,               /* "rtclassid"  */
    IBRIPORT = 513,                /* "ibriport"  */
    OBRIPORT = 514,                /* "obriport"  */
    IBRIDGENAME = 515,             /* "ibrname"  */
    OBRIDGENAME = 516,             /* "obrname"  */
    PKTTYPE = 517,                 /* "pkttype"  */
    CPU = 518,                     /* "cpu"  */
    IIFGROUP = 519,                /* "iifgroup"  */
    OIFGROUP = 520,                /* "oifgroup"  */
    CGROUP = 521,                  /* "cgroup"  */
    TIME = 522,                    /* "time"  */
    CLASSID = 523,                 /* "classid"  */
    NEXTHOP = 524,                 /* "nexthop"  */
    CT = 525,                      /* "ct"  */
    L3PROTOCOL = 526,              /* "l3proto"  */
    PROTO_SRC = 527,               /* "proto-src"  */
    PROTO_DST = 528,               /* "proto-dst"  */
    ZONE = 529,                    /* "zone"  */
    DIRECTION = 530,               /* "direction"  */
    EVENT = 531,                   /* "event"  */
    EXPECTATION = 532,             /* "expectation"  */
    EXPIRATION = 533,              /* "expiration"  */
    HELPER = 534,                  /* "helper"  */
    LABEL = 535,                   /* "label"  */
    STATE = 536,                   /* "state"  */
    STATUS = 537,                  /* "status"  */
    ORIGINAL = 538,                /* "original"  */
    REPLY = 539,                   /* "reply"  */
    COUNTER = 540,                 /* "counter"  */
    NAME = 541,                    /* "name"  */
    PACKETS = 542,                 /* "packets"  */
    BYTES = 543,                   /* "bytes"  */
    AVGPKT = 544,                  /* "avgpkt"  */
    COUNTERS = 545,                /* "counters"  */
    QUOTAS = 546,                  /* "quotas"  */
    LIMITS = 547,                  /* "limits"  */
    SYNPROXYS = 548,               /* "synproxys"  */
    HELPERS = 549,                 /* "helpers"  */
    LOG = 550,                     /* "log"  */
    PREFIX = 551,                  /* "prefix"  */
    GROUP = 552,                   /* "group"  */
    SNAPLEN = 553,                 /* "snaplen"  */
    QUEUE_THRESHOLD = 554,         /* "queue-threshold"  */
    LEVEL = 555,                   /* "level"  */
    LIMIT = 556,                   /* "limit"  */
    RATE = 557,                    /* "rate"  */
    BURST = 558,                   /* "burst"  */
    OVER = 559,                    /* "over"  */
    UNTIL = 560,                   /* "until"  */
    QUOTA = 561,                   /* "quota"  */
    USED = 562,                    /* "used"  */
    SECMARK = 563,                 /* "secmark"  */
    SECMARKS = 564,                /* "secmarks"  */
    SECOND = 565,                  /* "second"  */
    MINUTE = 566,                  /* "minute"  */
    HOUR = 567,                    /* "hour"  */
    DAY = 568,                     /* "day"  */
    WEEK = 569,                    /* "week"  */
    _REJECT = 570,                 /* "reject"  */
    WITH = 571,                    /* "with"  */
    ICMPX = 572,                   /* "icmpx"  */
    SNAT = 573,                    /* "snat"  */
    DNAT = 574,                    /* "dnat"  */
    MASQUERADE = 575,              /* "masquerade"  */
    REDIRECT = 576,                /* "redirect"  */
    RANDOM = 577,                  /* "random"  */
    FULLY_RANDOM = 578,            /* "fully-random"  */
    PERSISTENT = 579,              /* "persistent"  */
    QUEUE = 580,                   /* "queue"  */
    QUEUENUM = 581,                /* "num"  */
    BYPASS = 582,                  /* "bypass"  */
    FANOUT = 583,                  /* "fanout"  */
    DUP = 584,                     /* "dup"  */
    FWD = 585,                     /* "fwd"  */
    NUMGEN = 586,                  /* "numgen"  */
    INC = 587,                     /* "inc"  */
    MOD = 588,                     /* "mod"  */
    OFFSET = 589,                  /* "offset"  */
    JHASH = 590,                   /* "jhash"  */
    SYMHASH = 591,                 /* "symhash"  */
    SEED = 592,                    /* "seed"  */
    POSITION = 593,                /* "position"  */
    INDEX = 594,                   /* "index"  */
    COMMENT = 595,                 /* "comment"  */
    XML = 596,                     /* "xml"  */
    JSON = 597,                    /* "json"  */
    VM = 598,                      /* "vm"  */
    NOTRACK = 599,                 /* "notrack"  */
    EXISTS = 600,                  /* "exists"  */
    MISSING = 601,                 /* "missing"  */
    EXTHDR = 602,                  /* "exthdr"  */
    IPSEC = 603,                   /* "ipsec"  */
    REQID = 604,                   /* "reqid"  */
    SPNUM = 605,                   /* "spnum"  */
    IN = 606,                      /* "in"  */
    OUT = 607                      /* "out"  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define TOKEN_EOF 0
#define YYerror 256
#define YYUNDEF 257
#define JUNK 258
#define NEWLINE 259
#define COLON 260
#define SEMICOLON 261
#define COMMA 262
#define DOT 263
#define EQ 264
#define NEQ 265
#define LT 266
#define GT 267
#define GTE 268
#define LTE 269
#define LSHIFT 270
#define RSHIFT 271
#define AMPERSAND 272
#define CARET 273
#define NOT 274
#define SLASH 275
#define ASTERISK 276
#define DASH 277
#define AT 278
#define VMAP 279
#define PLUS 280
#define INCLUDE 281
#define DEFINE 282
#define REDEFINE 283
#define UNDEFINE 284
#define FIB 285
#define SOCKET 286
#define TRANSPARENT 287
#define WILDCARD 288
#define CGROUPV2 289
#define TPROXY 290
#define OSF 291
#define SYNPROXY 292
#define MSS 293
#define WSCALE 294
#define TYPEOF 295
#define HOOK 296
#define HOOKS 297
#define DEVICE 298
#define DEVICES 299
#define TABLE 300
#define TABLES 301
#define CHAIN 302
#define CHAINS 303
#define RULE 304
#define RULES 305
#define SETS 306
#define SET 307
#define ELEMENT 308
#define MAP 309
#define MAPS 310
#define FLOWTABLE 311
#define HANDLE 312
#define RULESET 313
#define TRACE 314
#define INET 315
#define NETDEV 316
#define ADD 317
#define UPDATE 318
#define REPLACE 319
#define CREATE 320
#define INSERT 321
#define DELETE 322
#define GET 323
#define LIST 324
#define RESET 325
#define FLUSH 326
#define RENAME 327
#define DESCRIBE 328
#define IMPORT 329
#define EXPORT 330
#define MONITOR 331
#define ALL 332
#define ACCEPT 333
#define DROP 334
#define CONTINUE 335
#define JUMP 336
#define GOTO 337
#define RETURN 338
#define TO 339
#define CONSTANT 340
#define INTERVAL 341
#define DYNAMIC 342
#define AUTOMERGE 343
#define TIMEOUT 344
#define GC_INTERVAL 345
#define ELEMENTS 346
#define EXPIRES 347
#define POLICY 348
#define MEMORY 349
#define PERFORMANCE 350
#define SIZE 351
#define FLOW 352
#define OFFLOAD 353
#define METER 354
#define METERS 355
#define FLOWTABLES 356
#define NUM 357
#define STRING 358
#define QUOTED_STRING 359
#define ASTERISK_STRING 360
#define LL_HDR 361
#define NETWORK_HDR 362
#define TRANSPORT_HDR 363
#define BRIDGE 364
#define ETHER 365
#define SADDR 366
#define DADDR 367
#define TYPE 368
#define VLAN 369
#define ID 370
#define CFI 371
#define DEI 372
#define PCP 373
#define ARP 374
#define HTYPE 375
#define PTYPE 376
#define HLEN 377
#define PLEN 378
#define OPERATION 379
#define IP 380
#define HDRVERSION 381
#define HDRLENGTH 382
#define DSCP 383
#define ECN 384
#define LENGTH 385
#define FRAG_OFF 386
#define TTL 387
#define PROTOCOL 388
#define CHECKSUM 389
#define PTR 390
#define VALUE 391
#define LSRR 392
#define RR 393
#define SSRR 394
#define RA 395
#define ICMP 396
#define CODE 397
#define SEQUENCE 398
#define GATEWAY 399
#define MTU 400
#define IGMP 401
#define MRT 402
#define OPTIONS 403
#define IP6 404
#define PRIORITY 405
#define FLOWLABEL 406
#define NEXTHDR 407
#define HOPLIMIT 408
#define ICMP6 409
#define PPTR 410
#define MAXDELAY 411
#define AH 412
#define RESERVED 413
#define SPI 414
#define ESP 415
#define COMP 416
#define FLAGS 417
#define CPI 418
#define PORT 419
#define UDP 420
#define SPORT 421
#define DPORT 422
#define UDPLITE 423
#define CSUMCOV 424
#define TCP 425
#define ACKSEQ 426
#define DOFF 427
#define WINDOW 428
#define URGPTR 429
#define OPTION 430
#define ECHO 431
#define EOL 432
#define MPTCP 433
#define NOP 434
#define SACK 435
#define SACK0 436
#define SACK1 437
#define SACK2 438
#define SACK3 439
#define SACK_PERM 440
#define FASTOPEN 441
#define MD5SIG 442
#define TIMESTAMP 443
#define COUNT 444
#define LEFT 445
#define RIGHT 446
#define TSVAL 447
#define TSECR 448
#define SUBTYPE 449
#define DCCP 450
#define SCTP 451
#define CHUNK 452
#define DATA 453
#define INIT 454
#define INIT_ACK 455
#define HEARTBEAT 456
#define HEARTBEAT_ACK 457
#define ABORT 458
#define SHUTDOWN 459
#define SHUTDOWN_ACK 460
#define ERROR 461
#define COOKIE_ECHO 462
#define COOKIE_ACK 463
#define ECNE 464
#define CWR 465
#define SHUTDOWN_COMPLETE 466
#define ASCONF_ACK 467
#define FORWARD_TSN 468
#define ASCONF 469
#define TSN 470
#define STREAM 471
#define SSN 472
#define PPID 473
#define INIT_TAG 474
#define A_RWND 475
#define NUM_OSTREAMS 476
#define NUM_ISTREAMS 477
#define INIT_TSN 478
#define CUM_TSN_ACK 479
#define NUM_GACK_BLOCKS 480
#define NUM_DUP_TSNS 481
#define LOWEST_TSN 482
#define SEQNO 483
#define NEW_CUM_TSN 484
#define VTAG 485
#define RT 486
#define RT0 487
#define RT2 488
#define RT4 489
#define SEG_LEFT 490
#define ADDR 491
#define LAST_ENT 492
#define TAG 493
#define SID 494
#define HBH 495
#define FRAG 496
#define RESERVED2 497
#define MORE_FRAGMENTS 498
#define DST 499
#define MH 500
#define META 501
#define MARK 502
#define IIF 503
#define IIFNAME 504
#define IIFTYPE 505
#define OIF 506
#define OIFNAME 507
#define OIFTYPE 508
#define SKUID 509
#define SKGID 510
#define NFTRACE 511
#define RTCLASSID 512
#define IBRIPORT 513
#define OBRIPORT 514
#define IBRIDGENAME 515
#define OBRIDGENAME 516
#define PKTTYPE 517
#define CPU 518
#define IIFGROUP 519
#define OIFGROUP 520
#define CGROUP 521
#define TIME 522
#define CLASSID 523
#define NEXTHOP 524
#define CT 525
#define L3PROTOCOL 526
#define PROTO_SRC 527
#define PROTO_DST 528
#define ZONE 529
#define DIRECTION 530
#define EVENT 531
#define EXPECTATION 532
#define EXPIRATION 533
#define HELPER 534
#define LABEL 535
#define STATE 536
#define STATUS 537
#define ORIGINAL 538
#define REPLY 539
#define COUNTER 540
#define NAME 541
#define PACKETS 542
#define BYTES 543
#define AVGPKT 544
#define COUNTERS 545
#define QUOTAS 546
#define LIMITS 547
#define SYNPROXYS 548
#define HELPERS 549
#define LOG 550
#define PREFIX 551
#define GROUP 552
#define SNAPLEN 553
#define QUEUE_THRESHOLD 554
#define LEVEL 555
#define LIMIT 556
#define RATE 557
#define BURST 558
#define OVER 559
#define UNTIL 560
#define QUOTA 561
#define USED 562
#define SECMARK 563
#define SECMARKS 564
#define SECOND 565
#define MINUTE 566
#define HOUR 567
#define DAY 568
#define WEEK 569
#define _REJECT 570
#define WITH 571
#define ICMPX 572
#define SNAT 573
#define DNAT 574
#define MASQUERADE 575
#define REDIRECT 576
#define RANDOM 577
#define FULLY_RANDOM 578
#define PERSISTENT 579
#define QUEUE 580
#define QUEUENUM 581
#define BYPASS 582
#define FANOUT 583
#define DUP 584
#define FWD 585
#define NUMGEN 586
#define INC 587
#define MOD 588
#define OFFSET 589
#define JHASH 590
#define SYMHASH 591
#define SEED 592
#define POSITION 593
#define INDEX 594
#define COMMENT 595
#define XML 596
#define JSON 597
#define VM 598
#define NOTRACK 599
#define EXISTS 600
#define MISSING 601
#define EXTHDR 602
#define IPSEC 603
#define REQID 604
#define SPNUM 605
#define IN 606
#define OUT 607

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 167 "parser_bison.y"

	uint64_t		val;
	uint32_t		val32;
	uint8_t			val8;
	const char *		string;

	struct list_head	*list;
	struct cmd		*cmd;
	struct handle		handle;
	struct table		*table;
	struct chain		*chain;
	struct rule		*rule;
	struct stmt		*stmt;
	struct expr		*expr;
	struct set		*set;
	struct obj		*obj;
	struct flowtable	*flowtable;
	struct ct		*ct;
	const struct datatype	*datatype;
	struct handle_spec	handle_spec;
	struct position_spec	position_spec;
	struct prio_spec	prio_spec;
	struct limit_rate	limit_rate;
	struct tcp_kind_field {
		uint16_t kind; /* must allow > 255 for SACK1, 2.. hack */
		uint8_t field;
	} tcp_kind_field;

#line 998 "parser_bison.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int nft_parse (struct nft_ctx *nft, void *scanner, struct parser_state *state);

#endif /* !YY_NFT_PARSER_BISON_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_JUNK = 3,                       /* "junk"  */
  YYSYMBOL_NEWLINE = 4,                    /* "newline"  */
  YYSYMBOL_COLON = 5,                      /* "colon"  */
  YYSYMBOL_SEMICOLON = 6,                  /* "semicolon"  */
  YYSYMBOL_COMMA = 7,                      /* "comma"  */
  YYSYMBOL_DOT = 8,                        /* "."  */
  YYSYMBOL_EQ = 9,                         /* "=="  */
  YYSYMBOL_NEQ = 10,                       /* "!="  */
  YYSYMBOL_LT = 11,                        /* "<"  */
  YYSYMBOL_GT = 12,                        /* ">"  */
  YYSYMBOL_GTE = 13,                       /* ">="  */
  YYSYMBOL_LTE = 14,                       /* "<="  */
  YYSYMBOL_LSHIFT = 15,                    /* "<<"  */
  YYSYMBOL_RSHIFT = 16,                    /* ">>"  */
  YYSYMBOL_AMPERSAND = 17,                 /* "&"  */
  YYSYMBOL_CARET = 18,                     /* "^"  */
  YYSYMBOL_NOT = 19,                       /* "!"  */
  YYSYMBOL_SLASH = 20,                     /* "/"  */
  YYSYMBOL_ASTERISK = 21,                  /* "*"  */
  YYSYMBOL_DASH = 22,                      /* "-"  */
  YYSYMBOL_AT = 23,                        /* "@"  */
  YYSYMBOL_VMAP = 24,                      /* "vmap"  */
  YYSYMBOL_PLUS = 25,                      /* "+"  */
  YYSYMBOL_INCLUDE = 26,                   /* "include"  */
  YYSYMBOL_DEFINE = 27,                    /* "define"  */
  YYSYMBOL_REDEFINE = 28,                  /* "redefine"  */
  YYSYMBOL_UNDEFINE = 29,                  /* "undefine"  */
  YYSYMBOL_FIB = 30,                       /* "fib"  */
  YYSYMBOL_SOCKET = 31,                    /* "socket"  */
  YYSYMBOL_TRANSPARENT = 32,               /* "transparent"  */
  YYSYMBOL_WILDCARD = 33,                  /* "wildcard"  */
  YYSYMBOL_CGROUPV2 = 34,                  /* "cgroupv2"  */
  YYSYMBOL_TPROXY = 35,                    /* "tproxy"  */
  YYSYMBOL_OSF = 36,                       /* "osf"  */
  YYSYMBOL_SYNPROXY = 37,                  /* "synproxy"  */
  YYSYMBOL_MSS = 38,                       /* "mss"  */
  YYSYMBOL_WSCALE = 39,                    /* "wscale"  */
  YYSYMBOL_TYPEOF = 40,                    /* "typeof"  */
  YYSYMBOL_HOOK = 41,                      /* "hook"  */
  YYSYMBOL_HOOKS = 42,                     /* "hooks"  */
  YYSYMBOL_DEVICE = 43,                    /* "device"  */
  YYSYMBOL_DEVICES = 44,                   /* "devices"  */
  YYSYMBOL_TABLE = 45,                     /* "table"  */
  YYSYMBOL_TABLES = 46,                    /* "tables"  */
  YYSYMBOL_CHAIN = 47,                     /* "chain"  */
  YYSYMBOL_CHAINS = 48,                    /* "chains"  */
  YYSYMBOL_RULE = 49,                      /* "rule"  */
  YYSYMBOL_RULES = 50,                     /* "rules"  */
  YYSYMBOL_SETS = 51,                      /* "sets"  */
  YYSYMBOL_SET = 52,                       /* "set"  */
  YYSYMBOL_ELEMENT = 53,                   /* "element"  */
  YYSYMBOL_MAP = 54,                       /* "map"  */
  YYSYMBOL_MAPS = 55,                      /* "maps"  */
  YYSYMBOL_FLOWTABLE = 56,                 /* "flowtable"  */
  YYSYMBOL_HANDLE = 57,                    /* "handle"  */
  YYSYMBOL_RULESET = 58,                   /* "ruleset"  */
  YYSYMBOL_TRACE = 59,                     /* "trace"  */
  YYSYMBOL_INET = 60,                      /* "inet"  */
  YYSYMBOL_NETDEV = 61,                    /* "netdev"  */
  YYSYMBOL_ADD = 62,                       /* "add"  */
  YYSYMBOL_UPDATE = 63,                    /* "update"  */
  YYSYMBOL_REPLACE = 64,                   /* "replace"  */
  YYSYMBOL_CREATE = 65,                    /* "create"  */
  YYSYMBOL_INSERT = 66,                    /* "insert"  */
  YYSYMBOL_DELETE = 67,                    /* "delete"  */
  YYSYMBOL_GET = 68,                       /* "get"  */
  YYSYMBOL_LIST = 69,                      /* "list"  */
  YYSYMBOL_RESET = 70,                     /* "reset"  */
  YYSYMBOL_FLUSH = 71,                     /* "flush"  */
  YYSYMBOL_RENAME = 72,                    /* "rename"  */
  YYSYMBOL_DESCRIBE = 73,                  /* "describe"  */
  YYSYMBOL_IMPORT = 74,                    /* "import"  */
  YYSYMBOL_EXPORT = 75,                    /* "export"  */
  YYSYMBOL_MONITOR = 76,                   /* "monitor"  */
  YYSYMBOL_ALL = 77,                       /* "all"  */
  YYSYMBOL_ACCEPT = 78,                    /* "accept"  */
  YYSYMBOL_DROP = 79,                      /* "drop"  */
  YYSYMBOL_CONTINUE = 80,                  /* "continue"  */
  YYSYMBOL_JUMP = 81,                      /* "jump"  */
  YYSYMBOL_GOTO = 82,                      /* "goto"  */
  YYSYMBOL_RETURN = 83,                    /* "return"  */
  YYSYMBOL_TO = 84,                        /* "to"  */
  YYSYMBOL_CONSTANT = 85,                  /* "constant"  */
  YYSYMBOL_INTERVAL = 86,                  /* "interval"  */
  YYSYMBOL_DYNAMIC = 87,                   /* "dynamic"  */
  YYSYMBOL_AUTOMERGE = 88,                 /* "auto-merge"  */
  YYSYMBOL_TIMEOUT = 89,                   /* "timeout"  */
  YYSYMBOL_GC_INTERVAL = 90,               /* "gc-interval"  */
  YYSYMBOL_ELEMENTS = 91,                  /* "elements"  */
  YYSYMBOL_EXPIRES = 92,                   /* "expires"  */
  YYSYMBOL_POLICY = 93,                    /* "policy"  */
  YYSYMBOL_MEMORY = 94,                    /* "memory"  */
  YYSYMBOL_PERFORMANCE = 95,               /* "performance"  */
  YYSYMBOL_SIZE = 96,                      /* "size"  */
  YYSYMBOL_FLOW = 97,                      /* "flow"  */
  YYSYMBOL_OFFLOAD = 98,                   /* "offload"  */
  YYSYMBOL_METER = 99,                     /* "meter"  */
  YYSYMBOL_METERS = 100,                   /* "meters"  */
  YYSYMBOL_FLOWTABLES = 101,               /* "flowtables"  */
  YYSYMBOL_NUM = 102,                      /* "number"  */
  YYSYMBOL_STRING = 103,                   /* "string"  */
  YYSYMBOL_QUOTED_STRING = 104,            /* "quoted string"  */
  YYSYMBOL_ASTERISK_STRING = 105,          /* "string with a trailing asterisk"  */
  YYSYMBOL_LL_HDR = 106,                   /* "ll"  */
  YYSYMBOL_NETWORK_HDR = 107,              /* "nh"  */
  YYSYMBOL_TRANSPORT_HDR = 108,            /* "th"  */
  YYSYMBOL_BRIDGE = 109,                   /* "bridge"  */
  YYSYMBOL_ETHER = 110,                    /* "ether"  */
  YYSYMBOL_SADDR = 111,                    /* "saddr"  */
  YYSYMBOL_DADDR = 112,                    /* "daddr"  */
  YYSYMBOL_TYPE = 113,                     /* "type"  */
  YYSYMBOL_VLAN = 114,                     /* "vlan"  */
  YYSYMBOL_ID = 115,                       /* "id"  */
  YYSYMBOL_CFI = 116,                      /* "cfi"  */
  YYSYMBOL_DEI = 117,                      /* "dei"  */
  YYSYMBOL_PCP = 118,                      /* "pcp"  */
  YYSYMBOL_ARP = 119,                      /* "arp"  */
  YYSYMBOL_HTYPE = 120,                    /* "htype"  */
  YYSYMBOL_PTYPE = 121,                    /* "ptype"  */
  YYSYMBOL_HLEN = 122,                     /* "hlen"  */
  YYSYMBOL_PLEN = 123,                     /* "plen"  */
  YYSYMBOL_OPERATION = 124,                /* "operation"  */
  YYSYMBOL_IP = 125,                       /* "ip"  */
  YYSYMBOL_HDRVERSION = 126,               /* "version"  */
  YYSYMBOL_HDRLENGTH = 127,                /* "hdrlength"  */
  YYSYMBOL_DSCP = 128,                     /* "dscp"  */
  YYSYMBOL_ECN = 129,                      /* "ecn"  */
  YYSYMBOL_LENGTH = 130,                   /* "length"  */
  YYSYMBOL_FRAG_OFF = 131,                 /* "frag-off"  */
  YYSYMBOL_TTL = 132,                      /* "ttl"  */
  YYSYMBOL_PROTOCOL = 133,                 /* "protocol"  */
  YYSYMBOL_CHECKSUM = 134,                 /* "checksum"  */
  YYSYMBOL_PTR = 135,                      /* "ptr"  */
  YYSYMBOL_VALUE = 136,                    /* "value"  */
  YYSYMBOL_LSRR = 137,                     /* "lsrr"  */
  YYSYMBOL_RR = 138,                       /* "rr"  */
  YYSYMBOL_SSRR = 139,                     /* "ssrr"  */
  YYSYMBOL_RA = 140,                       /* "ra"  */
  YYSYMBOL_ICMP = 141,                     /* "icmp"  */
  YYSYMBOL_CODE = 142,                     /* "code"  */
  YYSYMBOL_SEQUENCE = 143,                 /* "seq"  */
  YYSYMBOL_GATEWAY = 144,                  /* "gateway"  */
  YYSYMBOL_MTU = 145,                      /* "mtu"  */
  YYSYMBOL_IGMP = 146,                     /* "igmp"  */
  YYSYMBOL_MRT = 147,                      /* "mrt"  */
  YYSYMBOL_OPTIONS = 148,                  /* "options"  */
  YYSYMBOL_IP6 = 149,                      /* "ip6"  */
  YYSYMBOL_PRIORITY = 150,                 /* "priority"  */
  YYSYMBOL_FLOWLABEL = 151,                /* "flowlabel"  */
  YYSYMBOL_NEXTHDR = 152,                  /* "nexthdr"  */
  YYSYMBOL_HOPLIMIT = 153,                 /* "hoplimit"  */
  YYSYMBOL_ICMP6 = 154,                    /* "icmpv6"  */
  YYSYMBOL_PPTR = 155,                     /* "param-problem"  */
  YYSYMBOL_MAXDELAY = 156,                 /* "max-delay"  */
  YYSYMBOL_AH = 157,                       /* "ah"  */
  YYSYMBOL_RESERVED = 158,                 /* "reserved"  */
  YYSYMBOL_SPI = 159,                      /* "spi"  */
  YYSYMBOL_ESP = 160,                      /* "esp"  */
  YYSYMBOL_COMP = 161,                     /* "comp"  */
  YYSYMBOL_FLAGS = 162,                    /* "flags"  */
  YYSYMBOL_CPI = 163,                      /* "cpi"  */
  YYSYMBOL_PORT = 164,                     /* "port"  */
  YYSYMBOL_UDP = 165,                      /* "udp"  */
  YYSYMBOL_SPORT = 166,                    /* "sport"  */
  YYSYMBOL_DPORT = 167,                    /* "dport"  */
  YYSYMBOL_UDPLITE = 168,                  /* "udplite"  */
  YYSYMBOL_CSUMCOV = 169,                  /* "csumcov"  */
  YYSYMBOL_TCP = 170,                      /* "tcp"  */
  YYSYMBOL_ACKSEQ = 171,                   /* "ackseq"  */
  YYSYMBOL_DOFF = 172,                     /* "doff"  */
  YYSYMBOL_WINDOW = 173,                   /* "window"  */
  YYSYMBOL_URGPTR = 174,                   /* "urgptr"  */
  YYSYMBOL_OPTION = 175,                   /* "option"  */
  YYSYMBOL_ECHO = 176,                     /* "echo"  */
  YYSYMBOL_EOL = 177,                      /* "eol"  */
  YYSYMBOL_MPTCP = 178,                    /* "mptcp"  */
  YYSYMBOL_NOP = 179,                      /* "nop"  */
  YYSYMBOL_SACK = 180,                     /* "sack"  */
  YYSYMBOL_SACK0 = 181,                    /* "sack0"  */
  YYSYMBOL_SACK1 = 182,                    /* "sack1"  */
  YYSYMBOL_SACK2 = 183,                    /* "sack2"  */
  YYSYMBOL_SACK3 = 184,                    /* "sack3"  */
  YYSYMBOL_SACK_PERM = 185,                /* "sack-permitted"  */
  YYSYMBOL_FASTOPEN = 186,                 /* "fastopen"  */
  YYSYMBOL_MD5SIG = 187,                   /* "md5sig"  */
  YYSYMBOL_TIMESTAMP = 188,                /* "timestamp"  */
  YYSYMBOL_COUNT = 189,                    /* "count"  */
  YYSYMBOL_LEFT = 190,                     /* "left"  */
  YYSYMBOL_RIGHT = 191,                    /* "right"  */
  YYSYMBOL_TSVAL = 192,                    /* "tsval"  */
  YYSYMBOL_TSECR = 193,                    /* "tsecr"  */
  YYSYMBOL_SUBTYPE = 194,                  /* "subtype"  */
  YYSYMBOL_DCCP = 195,                     /* "dccp"  */
  YYSYMBOL_SCTP = 196,                     /* "sctp"  */
  YYSYMBOL_CHUNK = 197,                    /* "chunk"  */
  YYSYMBOL_DATA = 198,                     /* "data"  */
  YYSYMBOL_INIT = 199,                     /* "init"  */
  YYSYMBOL_INIT_ACK = 200,                 /* "init-ack"  */
  YYSYMBOL_HEARTBEAT = 201,                /* "heartbeat"  */
  YYSYMBOL_HEARTBEAT_ACK = 202,            /* "heartbeat-ack"  */
  YYSYMBOL_ABORT = 203,                    /* "abort"  */
  YYSYMBOL_SHUTDOWN = 204,                 /* "shutdown"  */
  YYSYMBOL_SHUTDOWN_ACK = 205,             /* "shutdown-ack"  */
  YYSYMBOL_ERROR = 206,                    /* "error"  */
  YYSYMBOL_COOKIE_ECHO = 207,              /* "cookie-echo"  */
  YYSYMBOL_COOKIE_ACK = 208,               /* "cookie-ack"  */
  YYSYMBOL_ECNE = 209,                     /* "ecne"  */
  YYSYMBOL_CWR = 210,                      /* "cwr"  */
  YYSYMBOL_SHUTDOWN_COMPLETE = 211,        /* "shutdown-complete"  */
  YYSYMBOL_ASCONF_ACK = 212,               /* "asconf-ack"  */
  YYSYMBOL_FORWARD_TSN = 213,              /* "forward-tsn"  */
  YYSYMBOL_ASCONF = 214,                   /* "asconf"  */
  YYSYMBOL_TSN = 215,                      /* "tsn"  */
  YYSYMBOL_STREAM = 216,                   /* "stream"  */
  YYSYMBOL_SSN = 217,                      /* "ssn"  */
  YYSYMBOL_PPID = 218,                     /* "ppid"  */
  YYSYMBOL_INIT_TAG = 219,                 /* "init-tag"  */
  YYSYMBOL_A_RWND = 220,                   /* "a-rwnd"  */
  YYSYMBOL_NUM_OSTREAMS = 221,             /* "num-outbound-streams"  */
  YYSYMBOL_NUM_ISTREAMS = 222,             /* "num-inbound-streams"  */
  YYSYMBOL_INIT_TSN = 223,                 /* "initial-tsn"  */
  YYSYMBOL_CUM_TSN_ACK = 224,              /* "cum-tsn-ack"  */
  YYSYMBOL_NUM_GACK_BLOCKS = 225,          /* "num-gap-ack-blocks"  */
  YYSYMBOL_NUM_DUP_TSNS = 226,             /* "num-dup-tsns"  */
  YYSYMBOL_LOWEST_TSN = 227,               /* "lowest-tsn"  */
  YYSYMBOL_SEQNO = 228,                    /* "seqno"  */
  YYSYMBOL_NEW_CUM_TSN = 229,              /* "new-cum-tsn"  */
  YYSYMBOL_VTAG = 230,                     /* "vtag"  */
  YYSYMBOL_RT = 231,                       /* "rt"  */
  YYSYMBOL_RT0 = 232,                      /* "rt0"  */
  YYSYMBOL_RT2 = 233,                      /* "rt2"  */
  YYSYMBOL_RT4 = 234,                      /* "srh"  */
  YYSYMBOL_SEG_LEFT = 235,                 /* "seg-left"  */
  YYSYMBOL_ADDR = 236,                     /* "addr"  */
  YYSYMBOL_LAST_ENT = 237,                 /* "last-entry"  */
  YYSYMBOL_TAG = 238,                      /* "tag"  */
  YYSYMBOL_SID = 239,                      /* "sid"  */
  YYSYMBOL_HBH = 240,                      /* "hbh"  */
  YYSYMBOL_FRAG = 241,                     /* "frag"  */
  YYSYMBOL_RESERVED2 = 242,                /* "reserved2"  */
  YYSYMBOL_MORE_FRAGMENTS = 243,           /* "more-fragments"  */
  YYSYMBOL_DST = 244,                      /* "dst"  */
  YYSYMBOL_MH = 245,                       /* "mh"  */
  YYSYMBOL_META = 246,                     /* "meta"  */
  YYSYMBOL_MARK = 247,                     /* "mark"  */
  YYSYMBOL_IIF = 248,                      /* "iif"  */
  YYSYMBOL_IIFNAME = 249,                  /* "iifname"  */
  YYSYMBOL_IIFTYPE = 250,                  /* "iiftype"  */
  YYSYMBOL_OIF = 251,                      /* "oif"  */
  YYSYMBOL_OIFNAME = 252,                  /* "oifname"  */
  YYSYMBOL_OIFTYPE = 253,                  /* "oiftype"  */
  YYSYMBOL_SKUID = 254,                    /* "skuid"  */
  YYSYMBOL_SKGID = 255,                    /* "skgid"  */
  YYSYMBOL_NFTRACE = 256,                  /* "nftrace"  */
  YYSYMBOL_RTCLASSID = 257,                /* "rtclassid"  */
  YYSYMBOL_IBRIPORT = 258,                 /* "ibriport"  */
  YYSYMBOL_OBRIPORT = 259,                 /* "obriport"  */
  YYSYMBOL_IBRIDGENAME = 260,              /* "ibrname"  */
  YYSYMBOL_OBRIDGENAME = 261,              /* "obrname"  */
  YYSYMBOL_PKTTYPE = 262,                  /* "pkttype"  */
  YYSYMBOL_CPU = 263,                      /* "cpu"  */
  YYSYMBOL_IIFGROUP = 264,                 /* "iifgroup"  */
  YYSYMBOL_OIFGROUP = 265,                 /* "oifgroup"  */
  YYSYMBOL_CGROUP = 266,                   /* "cgroup"  */
  YYSYMBOL_TIME = 267,                     /* "time"  */
  YYSYMBOL_CLASSID = 268,                  /* "classid"  */
  YYSYMBOL_NEXTHOP = 269,                  /* "nexthop"  */
  YYSYMBOL_CT = 270,                       /* "ct"  */
  YYSYMBOL_L3PROTOCOL = 271,               /* "l3proto"  */
  YYSYMBOL_PROTO_SRC = 272,                /* "proto-src"  */
  YYSYMBOL_PROTO_DST = 273,                /* "proto-dst"  */
  YYSYMBOL_ZONE = 274,                     /* "zone"  */
  YYSYMBOL_DIRECTION = 275,                /* "direction"  */
  YYSYMBOL_EVENT = 276,                    /* "event"  */
  YYSYMBOL_EXPECTATION = 277,              /* "expectation"  */
  YYSYMBOL_EXPIRATION = 278,               /* "expiration"  */
  YYSYMBOL_HELPER = 279,                   /* "helper"  */
  YYSYMBOL_LABEL = 280,                    /* "label"  */
  YYSYMBOL_STATE = 281,                    /* "state"  */
  YYSYMBOL_STATUS = 282,                   /* "status"  */
  YYSYMBOL_ORIGINAL = 283,                 /* "original"  */
  YYSYMBOL_REPLY = 284,                    /* "reply"  */
  YYSYMBOL_COUNTER = 285,                  /* "counter"  */
  YYSYMBOL_NAME = 286,                     /* "name"  */
  YYSYMBOL_PACKETS = 287,                  /* "packets"  */
  YYSYMBOL_BYTES = 288,                    /* "bytes"  */
  YYSYMBOL_AVGPKT = 289,                   /* "avgpkt"  */
  YYSYMBOL_COUNTERS = 290,                 /* "counters"  */
  YYSYMBOL_QUOTAS = 291,                   /* "quotas"  */
  YYSYMBOL_LIMITS = 292,                   /* "limits"  */
  YYSYMBOL_SYNPROXYS = 293,                /* "synproxys"  */
  YYSYMBOL_HELPERS = 294,                  /* "helpers"  */
  YYSYMBOL_LOG = 295,                      /* "log"  */
  YYSYMBOL_PREFIX = 296,                   /* "prefix"  */
  YYSYMBOL_GROUP = 297,                    /* "group"  */
  YYSYMBOL_SNAPLEN = 298,                  /* "snaplen"  */
  YYSYMBOL_QUEUE_THRESHOLD = 299,          /* "queue-threshold"  */
  YYSYMBOL_LEVEL = 300,                    /* "level"  */
  YYSYMBOL_LIMIT = 301,                    /* "limit"  */
  YYSYMBOL_RATE = 302,                     /* "rate"  */
  YYSYMBOL_BURST = 303,                    /* "burst"  */
  YYSYMBOL_OVER = 304,                     /* "over"  */
  YYSYMBOL_UNTIL = 305,                    /* "until"  */
  YYSYMBOL_QUOTA = 306,                    /* "quota"  */
  YYSYMBOL_USED = 307,                     /* "used"  */
  YYSYMBOL_SECMARK = 308,                  /* "secmark"  */
  YYSYMBOL_SECMARKS = 309,                 /* "secmarks"  */
  YYSYMBOL_SECOND = 310,                   /* "second"  */
  YYSYMBOL_MINUTE = 311,                   /* "minute"  */
  YYSYMBOL_HOUR = 312,                     /* "hour"  */
  YYSYMBOL_DAY = 313,                      /* "day"  */
  YYSYMBOL_WEEK = 314,                     /* "week"  */
  YYSYMBOL__REJECT = 315,                  /* "reject"  */
  YYSYMBOL_WITH = 316,                     /* "with"  */
  YYSYMBOL_ICMPX = 317,                    /* "icmpx"  */
  YYSYMBOL_SNAT = 318,                     /* "snat"  */
  YYSYMBOL_DNAT = 319,                     /* "dnat"  */
  YYSYMBOL_MASQUERADE = 320,               /* "masquerade"  */
  YYSYMBOL_REDIRECT = 321,                 /* "redirect"  */
  YYSYMBOL_RANDOM = 322,                   /* "random"  */
  YYSYMBOL_FULLY_RANDOM = 323,             /* "fully-random"  */
  YYSYMBOL_PERSISTENT = 324,               /* "persistent"  */
  YYSYMBOL_QUEUE = 325,                    /* "queue"  */
  YYSYMBOL_QUEUENUM = 326,                 /* "num"  */
  YYSYMBOL_BYPASS = 327,                   /* "bypass"  */
  YYSYMBOL_FANOUT = 328,                   /* "fanout"  */
  YYSYMBOL_DUP = 329,                      /* "dup"  */
  YYSYMBOL_FWD = 330,                      /* "fwd"  */
  YYSYMBOL_NUMGEN = 331,                   /* "numgen"  */
  YYSYMBOL_INC = 332,                      /* "inc"  */
  YYSYMBOL_MOD = 333,                      /* "mod"  */
  YYSYMBOL_OFFSET = 334,                   /* "offset"  */
  YYSYMBOL_JHASH = 335,                    /* "jhash"  */
  YYSYMBOL_SYMHASH = 336,                  /* "symhash"  */
  YYSYMBOL_SEED = 337,                     /* "seed"  */
  YYSYMBOL_POSITION = 338,                 /* "position"  */
  YYSYMBOL_INDEX = 339,                    /* "index"  */
  YYSYMBOL_COMMENT = 340,                  /* "comment"  */
  YYSYMBOL_XML = 341,                      /* "xml"  */
  YYSYMBOL_JSON = 342,                     /* "json"  */
  YYSYMBOL_VM = 343,                       /* "vm"  */
  YYSYMBOL_NOTRACK = 344,                  /* "notrack"  */
  YYSYMBOL_EXISTS = 345,                   /* "exists"  */
  YYSYMBOL_MISSING = 346,                  /* "missing"  */
  YYSYMBOL_EXTHDR = 347,                   /* "exthdr"  */
  YYSYMBOL_IPSEC = 348,                    /* "ipsec"  */
  YYSYMBOL_REQID = 349,                    /* "reqid"  */
  YYSYMBOL_SPNUM = 350,                    /* "spnum"  */
  YYSYMBOL_IN = 351,                       /* "in"  */
  YYSYMBOL_OUT = 352,                      /* "out"  */
  YYSYMBOL_353_ = 353,                     /* '='  */
  YYSYMBOL_354_ = 354,                     /* '{'  */
  YYSYMBOL_355_ = 355,                     /* '}'  */
  YYSYMBOL_356_ = 356,                     /* '('  */
  YYSYMBOL_357_ = 357,                     /* ')'  */
  YYSYMBOL_358_ = 358,                     /* '|'  */
  YYSYMBOL_359_ = 359,                     /* '$'  */
  YYSYMBOL_360_ = 360,                     /* '['  */
  YYSYMBOL_361_ = 361,                     /* ']'  */
  YYSYMBOL_YYACCEPT = 362,                 /* $accept  */
  YYSYMBOL_input = 363,                    /* input  */
  YYSYMBOL_stmt_separator = 364,           /* stmt_separator  */
  YYSYMBOL_opt_newline = 365,              /* opt_newline  */
  YYSYMBOL_close_scope_ah = 366,           /* close_scope_ah  */
  YYSYMBOL_close_scope_arp = 367,          /* close_scope_arp  */
  YYSYMBOL_close_scope_at = 368,           /* close_scope_at  */
  YYSYMBOL_close_scope_comp = 369,         /* close_scope_comp  */
  YYSYMBOL_close_scope_ct = 370,           /* close_scope_ct  */
  YYSYMBOL_close_scope_counter = 371,      /* close_scope_counter  */
  YYSYMBOL_close_scope_dccp = 372,         /* close_scope_dccp  */
  YYSYMBOL_close_scope_dst = 373,          /* close_scope_dst  */
  YYSYMBOL_close_scope_dup = 374,          /* close_scope_dup  */
  YYSYMBOL_close_scope_esp = 375,          /* close_scope_esp  */
  YYSYMBOL_close_scope_eth = 376,          /* close_scope_eth  */
  YYSYMBOL_close_scope_export = 377,       /* close_scope_export  */
  YYSYMBOL_close_scope_fib = 378,          /* close_scope_fib  */
  YYSYMBOL_close_scope_flags = 379,        /* close_scope_flags  */
  YYSYMBOL_close_scope_frag = 380,         /* close_scope_frag  */
  YYSYMBOL_close_scope_fwd = 381,          /* close_scope_fwd  */
  YYSYMBOL_close_scope_hash = 382,         /* close_scope_hash  */
  YYSYMBOL_close_scope_hbh = 383,          /* close_scope_hbh  */
  YYSYMBOL_close_scope_ip = 384,           /* close_scope_ip  */
  YYSYMBOL_close_scope_ip6 = 385,          /* close_scope_ip6  */
  YYSYMBOL_close_scope_vlan = 386,         /* close_scope_vlan  */
  YYSYMBOL_close_scope_icmp = 387,         /* close_scope_icmp  */
  YYSYMBOL_close_scope_igmp = 388,         /* close_scope_igmp  */
  YYSYMBOL_close_scope_import = 389,       /* close_scope_import  */
  YYSYMBOL_close_scope_ipsec = 390,        /* close_scope_ipsec  */
  YYSYMBOL_close_scope_list = 391,         /* close_scope_list  */
  YYSYMBOL_close_scope_limit = 392,        /* close_scope_limit  */
  YYSYMBOL_close_scope_meta = 393,         /* close_scope_meta  */
  YYSYMBOL_close_scope_mh = 394,           /* close_scope_mh  */
  YYSYMBOL_close_scope_monitor = 395,      /* close_scope_monitor  */
  YYSYMBOL_close_scope_nat = 396,          /* close_scope_nat  */
  YYSYMBOL_close_scope_numgen = 397,       /* close_scope_numgen  */
  YYSYMBOL_close_scope_osf = 398,          /* close_scope_osf  */
  YYSYMBOL_close_scope_policy = 399,       /* close_scope_policy  */
  YYSYMBOL_close_scope_quota = 400,        /* close_scope_quota  */
  YYSYMBOL_close_scope_queue = 401,        /* close_scope_queue  */
  YYSYMBOL_close_scope_reject = 402,       /* close_scope_reject  */
  YYSYMBOL_close_scope_reset = 403,        /* close_scope_reset  */
  YYSYMBOL_close_scope_rt = 404,           /* close_scope_rt  */
  YYSYMBOL_close_scope_sctp = 405,         /* close_scope_sctp  */
  YYSYMBOL_close_scope_sctp_chunk = 406,   /* close_scope_sctp_chunk  */
  YYSYMBOL_close_scope_secmark = 407,      /* close_scope_secmark  */
  YYSYMBOL_close_scope_socket = 408,       /* close_scope_socket  */
  YYSYMBOL_close_scope_tcp = 409,          /* close_scope_tcp  */
  YYSYMBOL_close_scope_tproxy = 410,       /* close_scope_tproxy  */
  YYSYMBOL_close_scope_type = 411,         /* close_scope_type  */
  YYSYMBOL_close_scope_th = 412,           /* close_scope_th  */
  YYSYMBOL_close_scope_udp = 413,          /* close_scope_udp  */
  YYSYMBOL_close_scope_udplite = 414,      /* close_scope_udplite  */
  YYSYMBOL_close_scope_log = 415,          /* close_scope_log  */
  YYSYMBOL_close_scope_synproxy = 416,     /* close_scope_synproxy  */
  YYSYMBOL_common_block = 417,             /* common_block  */
  YYSYMBOL_line = 418,                     /* line  */
  YYSYMBOL_base_cmd = 419,                 /* base_cmd  */
  YYSYMBOL_add_cmd = 420,                  /* add_cmd  */
  YYSYMBOL_replace_cmd = 421,              /* replace_cmd  */
  YYSYMBOL_create_cmd = 422,               /* create_cmd  */
  YYSYMBOL_insert_cmd = 423,               /* insert_cmd  */
  YYSYMBOL_table_or_id_spec = 424,         /* table_or_id_spec  */
  YYSYMBOL_chain_or_id_spec = 425,         /* chain_or_id_spec  */
  YYSYMBOL_set_or_id_spec = 426,           /* set_or_id_spec  */
  YYSYMBOL_obj_or_id_spec = 427,           /* obj_or_id_spec  */
  YYSYMBOL_delete_cmd = 428,               /* delete_cmd  */
  YYSYMBOL_get_cmd = 429,                  /* get_cmd  */
  YYSYMBOL_list_cmd = 430,                 /* list_cmd  */
  YYSYMBOL_basehook_device_name = 431,     /* basehook_device_name  */
  YYSYMBOL_basehook_spec = 432,            /* basehook_spec  */
  YYSYMBOL_reset_cmd = 433,                /* reset_cmd  */
  YYSYMBOL_flush_cmd = 434,                /* flush_cmd  */
  YYSYMBOL_rename_cmd = 435,               /* rename_cmd  */
  YYSYMBOL_import_cmd = 436,               /* import_cmd  */
  YYSYMBOL_export_cmd = 437,               /* export_cmd  */
  YYSYMBOL_monitor_cmd = 438,              /* monitor_cmd  */
  YYSYMBOL_monitor_event = 439,            /* monitor_event  */
  YYSYMBOL_monitor_object = 440,           /* monitor_object  */
  YYSYMBOL_monitor_format = 441,           /* monitor_format  */
  YYSYMBOL_markup_format = 442,            /* markup_format  */
  YYSYMBOL_describe_cmd = 443,             /* describe_cmd  */
  YYSYMBOL_table_block_alloc = 444,        /* table_block_alloc  */
  YYSYMBOL_table_options = 445,            /* table_options  */
  YYSYMBOL_table_block = 446,              /* table_block  */
  YYSYMBOL_chain_block_alloc = 447,        /* chain_block_alloc  */
  YYSYMBOL_chain_block = 448,              /* chain_block  */
  YYSYMBOL_subchain_block = 449,           /* subchain_block  */
  YYSYMBOL_typeof_data_expr = 450,         /* typeof_data_expr  */
  YYSYMBOL_typeof_expr = 451,              /* typeof_expr  */
  YYSYMBOL_set_block_alloc = 452,          /* set_block_alloc  */
  YYSYMBOL_set_block = 453,                /* set_block  */
  YYSYMBOL_set_block_expr = 454,           /* set_block_expr  */
  YYSYMBOL_set_flag_list = 455,            /* set_flag_list  */
  YYSYMBOL_set_flag = 456,                 /* set_flag  */
  YYSYMBOL_map_block_alloc = 457,          /* map_block_alloc  */
  YYSYMBOL_map_block_obj_type = 458,       /* map_block_obj_type  */
  YYSYMBOL_map_block = 459,                /* map_block  */
  YYSYMBOL_set_mechanism = 460,            /* set_mechanism  */
  YYSYMBOL_set_policy_spec = 461,          /* set_policy_spec  */
  YYSYMBOL_flowtable_block_alloc = 462,    /* flowtable_block_alloc  */
  YYSYMBOL_flowtable_block = 463,          /* flowtable_block  */
  YYSYMBOL_flowtable_expr = 464,           /* flowtable_expr  */
  YYSYMBOL_flowtable_list_expr = 465,      /* flowtable_list_expr  */
  YYSYMBOL_flowtable_expr_member = 466,    /* flowtable_expr_member  */
  YYSYMBOL_data_type_atom_expr = 467,      /* data_type_atom_expr  */
  YYSYMBOL_data_type_expr = 468,           /* data_type_expr  */
  YYSYMBOL_obj_block_alloc = 469,          /* obj_block_alloc  */
  YYSYMBOL_counter_block = 470,            /* counter_block  */
  YYSYMBOL_quota_block = 471,              /* quota_block  */
  YYSYMBOL_ct_helper_block = 472,          /* ct_helper_block  */
  YYSYMBOL_ct_timeout_block = 473,         /* ct_timeout_block  */
  YYSYMBOL_ct_expect_block = 474,          /* ct_expect_block  */
  YYSYMBOL_limit_block = 475,              /* limit_block  */
  YYSYMBOL_secmark_block = 476,            /* secmark_block  */
  YYSYMBOL_synproxy_block = 477,           /* synproxy_block  */
  YYSYMBOL_type_identifier = 478,          /* type_identifier  */
  YYSYMBOL_hook_spec = 479,                /* hook_spec  */
  YYSYMBOL_prio_spec = 480,                /* prio_spec  */
  YYSYMBOL_extended_prio_name = 481,       /* extended_prio_name  */
  YYSYMBOL_extended_prio_spec = 482,       /* extended_prio_spec  */
  YYSYMBOL_int_num = 483,                  /* int_num  */
  YYSYMBOL_dev_spec = 484,                 /* dev_spec  */
  YYSYMBOL_flags_spec = 485,               /* flags_spec  */
  YYSYMBOL_policy_spec = 486,              /* policy_spec  */
  YYSYMBOL_policy_expr = 487,              /* policy_expr  */
  YYSYMBOL_chain_policy = 488,             /* chain_policy  */
  YYSYMBOL_identifier = 489,               /* identifier  */
  YYSYMBOL_string = 490,                   /* string  */
  YYSYMBOL_time_spec = 491,                /* time_spec  */
  YYSYMBOL_family_spec = 492,              /* family_spec  */
  YYSYMBOL_family_spec_explicit = 493,     /* family_spec_explicit  */
  YYSYMBOL_table_spec = 494,               /* table_spec  */
  YYSYMBOL_tableid_spec = 495,             /* tableid_spec  */
  YYSYMBOL_chain_spec = 496,               /* chain_spec  */
  YYSYMBOL_chainid_spec = 497,             /* chainid_spec  */
  YYSYMBOL_chain_identifier = 498,         /* chain_identifier  */
  YYSYMBOL_set_spec = 499,                 /* set_spec  */
  YYSYMBOL_setid_spec = 500,               /* setid_spec  */
  YYSYMBOL_set_identifier = 501,           /* set_identifier  */
  YYSYMBOL_flowtable_spec = 502,           /* flowtable_spec  */
  YYSYMBOL_flowtableid_spec = 503,         /* flowtableid_spec  */
  YYSYMBOL_flowtable_identifier = 504,     /* flowtable_identifier  */
  YYSYMBOL_obj_spec = 505,                 /* obj_spec  */
  YYSYMBOL_objid_spec = 506,               /* objid_spec  */
  YYSYMBOL_obj_identifier = 507,           /* obj_identifier  */
  YYSYMBOL_handle_spec = 508,              /* handle_spec  */
  YYSYMBOL_position_spec = 509,            /* position_spec  */
  YYSYMBOL_index_spec = 510,               /* index_spec  */
  YYSYMBOL_rule_position = 511,            /* rule_position  */
  YYSYMBOL_ruleid_spec = 512,              /* ruleid_spec  */
  YYSYMBOL_comment_spec = 513,             /* comment_spec  */
  YYSYMBOL_ruleset_spec = 514,             /* ruleset_spec  */
  YYSYMBOL_rule = 515,                     /* rule  */
  YYSYMBOL_rule_alloc = 516,               /* rule_alloc  */
  YYSYMBOL_stmt_list = 517,                /* stmt_list  */
  YYSYMBOL_stateful_stmt_list = 518,       /* stateful_stmt_list  */
  YYSYMBOL_stateful_stmt = 519,            /* stateful_stmt  */
  YYSYMBOL_stmt = 520,                     /* stmt  */
  YYSYMBOL_chain_stmt_type = 521,          /* chain_stmt_type  */
  YYSYMBOL_chain_stmt = 522,               /* chain_stmt  */
  YYSYMBOL_verdict_stmt = 523,             /* verdict_stmt  */
  YYSYMBOL_verdict_map_stmt = 524,         /* verdict_map_stmt  */
  YYSYMBOL_verdict_map_expr = 525,         /* verdict_map_expr  */
  YYSYMBOL_verdict_map_list_expr = 526,    /* verdict_map_list_expr  */
  YYSYMBOL_verdict_map_list_member_expr = 527, /* verdict_map_list_member_expr  */
  YYSYMBOL_connlimit_stmt = 528,           /* connlimit_stmt  */
  YYSYMBOL_counter_stmt = 529,             /* counter_stmt  */
  YYSYMBOL_counter_stmt_alloc = 530,       /* counter_stmt_alloc  */
  YYSYMBOL_counter_args = 531,             /* counter_args  */
  YYSYMBOL_counter_arg = 532,              /* counter_arg  */
  YYSYMBOL_log_stmt = 533,                 /* log_stmt  */
  YYSYMBOL_log_stmt_alloc = 534,           /* log_stmt_alloc  */
  YYSYMBOL_log_args = 535,                 /* log_args  */
  YYSYMBOL_log_arg = 536,                  /* log_arg  */
  YYSYMBOL_level_type = 537,               /* level_type  */
  YYSYMBOL_log_flags = 538,                /* log_flags  */
  YYSYMBOL_log_flags_tcp = 539,            /* log_flags_tcp  */
  YYSYMBOL_log_flag_tcp = 540,             /* log_flag_tcp  */
  YYSYMBOL_limit_stmt = 541,               /* limit_stmt  */
  YYSYMBOL_quota_mode = 542,               /* quota_mode  */
  YYSYMBOL_quota_unit = 543,               /* quota_unit  */
  YYSYMBOL_quota_used = 544,               /* quota_used  */
  YYSYMBOL_quota_stmt = 545,               /* quota_stmt  */
  YYSYMBOL_limit_mode = 546,               /* limit_mode  */
  YYSYMBOL_limit_burst_pkts = 547,         /* limit_burst_pkts  */
  YYSYMBOL_limit_rate_pkts = 548,          /* limit_rate_pkts  */
  YYSYMBOL_limit_burst_bytes = 549,        /* limit_burst_bytes  */
  YYSYMBOL_limit_rate_bytes = 550,         /* limit_rate_bytes  */
  YYSYMBOL_limit_bytes = 551,              /* limit_bytes  */
  YYSYMBOL_time_unit = 552,                /* time_unit  */
  YYSYMBOL_reject_stmt = 553,              /* reject_stmt  */
  YYSYMBOL_reject_stmt_alloc = 554,        /* reject_stmt_alloc  */
  YYSYMBOL_reject_with_expr = 555,         /* reject_with_expr  */
  YYSYMBOL_reject_opts = 556,              /* reject_opts  */
  YYSYMBOL_nat_stmt = 557,                 /* nat_stmt  */
  YYSYMBOL_nat_stmt_alloc = 558,           /* nat_stmt_alloc  */
  YYSYMBOL_tproxy_stmt = 559,              /* tproxy_stmt  */
  YYSYMBOL_synproxy_stmt = 560,            /* synproxy_stmt  */
  YYSYMBOL_synproxy_stmt_alloc = 561,      /* synproxy_stmt_alloc  */
  YYSYMBOL_synproxy_args = 562,            /* synproxy_args  */
  YYSYMBOL_synproxy_arg = 563,             /* synproxy_arg  */
  YYSYMBOL_synproxy_config = 564,          /* synproxy_config  */
  YYSYMBOL_synproxy_obj = 565,             /* synproxy_obj  */
  YYSYMBOL_synproxy_ts = 566,              /* synproxy_ts  */
  YYSYMBOL_synproxy_sack = 567,            /* synproxy_sack  */
  YYSYMBOL_primary_stmt_expr = 568,        /* primary_stmt_expr  */
  YYSYMBOL_shift_stmt_expr = 569,          /* shift_stmt_expr  */
  YYSYMBOL_and_stmt_expr = 570,            /* and_stmt_expr  */
  YYSYMBOL_exclusive_or_stmt_expr = 571,   /* exclusive_or_stmt_expr  */
  YYSYMBOL_inclusive_or_stmt_expr = 572,   /* inclusive_or_stmt_expr  */
  YYSYMBOL_basic_stmt_expr = 573,          /* basic_stmt_expr  */
  YYSYMBOL_concat_stmt_expr = 574,         /* concat_stmt_expr  */
  YYSYMBOL_map_stmt_expr_set = 575,        /* map_stmt_expr_set  */
  YYSYMBOL_map_stmt_expr = 576,            /* map_stmt_expr  */
  YYSYMBOL_prefix_stmt_expr = 577,         /* prefix_stmt_expr  */
  YYSYMBOL_range_stmt_expr = 578,          /* range_stmt_expr  */
  YYSYMBOL_multiton_stmt_expr = 579,       /* multiton_stmt_expr  */
  YYSYMBOL_stmt_expr = 580,                /* stmt_expr  */
  YYSYMBOL_nat_stmt_args = 581,            /* nat_stmt_args  */
  YYSYMBOL_masq_stmt = 582,                /* masq_stmt  */
  YYSYMBOL_masq_stmt_alloc = 583,          /* masq_stmt_alloc  */
  YYSYMBOL_masq_stmt_args = 584,           /* masq_stmt_args  */
  YYSYMBOL_redir_stmt = 585,               /* redir_stmt  */
  YYSYMBOL_redir_stmt_alloc = 586,         /* redir_stmt_alloc  */
  YYSYMBOL_redir_stmt_arg = 587,           /* redir_stmt_arg  */
  YYSYMBOL_dup_stmt = 588,                 /* dup_stmt  */
  YYSYMBOL_fwd_stmt = 589,                 /* fwd_stmt  */
  YYSYMBOL_nf_nat_flags = 590,             /* nf_nat_flags  */
  YYSYMBOL_nf_nat_flag = 591,              /* nf_nat_flag  */
  YYSYMBOL_queue_stmt = 592,               /* queue_stmt  */
  YYSYMBOL_queue_stmt_compat = 593,        /* queue_stmt_compat  */
  YYSYMBOL_queue_stmt_alloc = 594,         /* queue_stmt_alloc  */
  YYSYMBOL_queue_stmt_args = 595,          /* queue_stmt_args  */
  YYSYMBOL_queue_stmt_arg = 596,           /* queue_stmt_arg  */
  YYSYMBOL_queue_expr = 597,               /* queue_expr  */
  YYSYMBOL_queue_stmt_expr_simple = 598,   /* queue_stmt_expr_simple  */
  YYSYMBOL_queue_stmt_expr = 599,          /* queue_stmt_expr  */
  YYSYMBOL_queue_stmt_flags = 600,         /* queue_stmt_flags  */
  YYSYMBOL_queue_stmt_flag = 601,          /* queue_stmt_flag  */
  YYSYMBOL_set_elem_expr_stmt = 602,       /* set_elem_expr_stmt  */
  YYSYMBOL_set_elem_expr_stmt_alloc = 603, /* set_elem_expr_stmt_alloc  */
  YYSYMBOL_set_stmt = 604,                 /* set_stmt  */
  YYSYMBOL_set_stmt_op = 605,              /* set_stmt_op  */
  YYSYMBOL_map_stmt = 606,                 /* map_stmt  */
  YYSYMBOL_meter_stmt = 607,               /* meter_stmt  */
  YYSYMBOL_flow_stmt_legacy_alloc = 608,   /* flow_stmt_legacy_alloc  */
  YYSYMBOL_flow_stmt_opts = 609,           /* flow_stmt_opts  */
  YYSYMBOL_flow_stmt_opt = 610,            /* flow_stmt_opt  */
  YYSYMBOL_meter_stmt_alloc = 611,         /* meter_stmt_alloc  */
  YYSYMBOL_match_stmt = 612,               /* match_stmt  */
  YYSYMBOL_variable_expr = 613,            /* variable_expr  */
  YYSYMBOL_symbol_expr = 614,              /* symbol_expr  */
  YYSYMBOL_set_ref_expr = 615,             /* set_ref_expr  */
  YYSYMBOL_set_ref_symbol_expr = 616,      /* set_ref_symbol_expr  */
  YYSYMBOL_integer_expr = 617,             /* integer_expr  */
  YYSYMBOL_primary_expr = 618,             /* primary_expr  */
  YYSYMBOL_fib_expr = 619,                 /* fib_expr  */
  YYSYMBOL_fib_result = 620,               /* fib_result  */
  YYSYMBOL_fib_flag = 621,                 /* fib_flag  */
  YYSYMBOL_fib_tuple = 622,                /* fib_tuple  */
  YYSYMBOL_osf_expr = 623,                 /* osf_expr  */
  YYSYMBOL_osf_ttl = 624,                  /* osf_ttl  */
  YYSYMBOL_shift_expr = 625,               /* shift_expr  */
  YYSYMBOL_and_expr = 626,                 /* and_expr  */
  YYSYMBOL_exclusive_or_expr = 627,        /* exclusive_or_expr  */
  YYSYMBOL_inclusive_or_expr = 628,        /* inclusive_or_expr  */
  YYSYMBOL_basic_expr = 629,               /* basic_expr  */
  YYSYMBOL_concat_expr = 630,              /* concat_expr  */
  YYSYMBOL_prefix_rhs_expr = 631,          /* prefix_rhs_expr  */
  YYSYMBOL_range_rhs_expr = 632,           /* range_rhs_expr  */
  YYSYMBOL_multiton_rhs_expr = 633,        /* multiton_rhs_expr  */
  YYSYMBOL_map_expr = 634,                 /* map_expr  */
  YYSYMBOL_expr = 635,                     /* expr  */
  YYSYMBOL_set_expr = 636,                 /* set_expr  */
  YYSYMBOL_set_list_expr = 637,            /* set_list_expr  */
  YYSYMBOL_set_list_member_expr = 638,     /* set_list_member_expr  */
  YYSYMBOL_meter_key_expr = 639,           /* meter_key_expr  */
  YYSYMBOL_meter_key_expr_alloc = 640,     /* meter_key_expr_alloc  */
  YYSYMBOL_set_elem_expr = 641,            /* set_elem_expr  */
  YYSYMBOL_set_elem_key_expr = 642,        /* set_elem_key_expr  */
  YYSYMBOL_set_elem_expr_alloc = 643,      /* set_elem_expr_alloc  */
  YYSYMBOL_set_elem_options = 644,         /* set_elem_options  */
  YYSYMBOL_set_elem_option = 645,          /* set_elem_option  */
  YYSYMBOL_set_elem_expr_options = 646,    /* set_elem_expr_options  */
  YYSYMBOL_set_elem_stmt_list = 647,       /* set_elem_stmt_list  */
  YYSYMBOL_set_elem_stmt = 648,            /* set_elem_stmt  */
  YYSYMBOL_set_elem_expr_option = 649,     /* set_elem_expr_option  */
  YYSYMBOL_set_lhs_expr = 650,             /* set_lhs_expr  */
  YYSYMBOL_set_rhs_expr = 651,             /* set_rhs_expr  */
  YYSYMBOL_initializer_expr = 652,         /* initializer_expr  */
  YYSYMBOL_counter_config = 653,           /* counter_config  */
  YYSYMBOL_counter_obj = 654,              /* counter_obj  */
  YYSYMBOL_quota_config = 655,             /* quota_config  */
  YYSYMBOL_quota_obj = 656,                /* quota_obj  */
  YYSYMBOL_secmark_config = 657,           /* secmark_config  */
  YYSYMBOL_secmark_obj = 658,              /* secmark_obj  */
  YYSYMBOL_ct_obj_type = 659,              /* ct_obj_type  */
  YYSYMBOL_ct_cmd_type = 660,              /* ct_cmd_type  */
  YYSYMBOL_ct_l4protoname = 661,           /* ct_l4protoname  */
  YYSYMBOL_ct_helper_config = 662,         /* ct_helper_config  */
  YYSYMBOL_timeout_states = 663,           /* timeout_states  */
  YYSYMBOL_timeout_state = 664,            /* timeout_state  */
  YYSYMBOL_ct_timeout_config = 665,        /* ct_timeout_config  */
  YYSYMBOL_ct_expect_config = 666,         /* ct_expect_config  */
  YYSYMBOL_ct_obj_alloc = 667,             /* ct_obj_alloc  */
  YYSYMBOL_limit_config = 668,             /* limit_config  */
  YYSYMBOL_limit_obj = 669,                /* limit_obj  */
  YYSYMBOL_relational_expr = 670,          /* relational_expr  */
  YYSYMBOL_list_rhs_expr = 671,            /* list_rhs_expr  */
  YYSYMBOL_rhs_expr = 672,                 /* rhs_expr  */
  YYSYMBOL_shift_rhs_expr = 673,           /* shift_rhs_expr  */
  YYSYMBOL_and_rhs_expr = 674,             /* and_rhs_expr  */
  YYSYMBOL_exclusive_or_rhs_expr = 675,    /* exclusive_or_rhs_expr  */
  YYSYMBOL_inclusive_or_rhs_expr = 676,    /* inclusive_or_rhs_expr  */
  YYSYMBOL_basic_rhs_expr = 677,           /* basic_rhs_expr  */
  YYSYMBOL_concat_rhs_expr = 678,          /* concat_rhs_expr  */
  YYSYMBOL_boolean_keys = 679,             /* boolean_keys  */
  YYSYMBOL_boolean_expr = 680,             /* boolean_expr  */
  YYSYMBOL_keyword_expr = 681,             /* keyword_expr  */
  YYSYMBOL_primary_rhs_expr = 682,         /* primary_rhs_expr  */
  YYSYMBOL_relational_op = 683,            /* relational_op  */
  YYSYMBOL_verdict_expr = 684,             /* verdict_expr  */
  YYSYMBOL_chain_expr = 685,               /* chain_expr  */
  YYSYMBOL_meta_expr = 686,                /* meta_expr  */
  YYSYMBOL_meta_key = 687,                 /* meta_key  */
  YYSYMBOL_meta_key_qualified = 688,       /* meta_key_qualified  */
  YYSYMBOL_meta_key_unqualified = 689,     /* meta_key_unqualified  */
  YYSYMBOL_meta_stmt = 690,                /* meta_stmt  */
  YYSYMBOL_socket_expr = 691,              /* socket_expr  */
  YYSYMBOL_socket_key = 692,               /* socket_key  */
  YYSYMBOL_offset_opt = 693,               /* offset_opt  */
  YYSYMBOL_numgen_type = 694,              /* numgen_type  */
  YYSYMBOL_numgen_expr = 695,              /* numgen_expr  */
  YYSYMBOL_xfrm_spnum = 696,               /* xfrm_spnum  */
  YYSYMBOL_xfrm_dir = 697,                 /* xfrm_dir  */
  YYSYMBOL_xfrm_state_key = 698,           /* xfrm_state_key  */
  YYSYMBOL_xfrm_state_proto_key = 699,     /* xfrm_state_proto_key  */
  YYSYMBOL_xfrm_expr = 700,                /* xfrm_expr  */
  YYSYMBOL_hash_expr = 701,                /* hash_expr  */
  YYSYMBOL_nf_key_proto = 702,             /* nf_key_proto  */
  YYSYMBOL_rt_expr = 703,                  /* rt_expr  */
  YYSYMBOL_rt_key = 704,                   /* rt_key  */
  YYSYMBOL_ct_expr = 705,                  /* ct_expr  */
  YYSYMBOL_ct_dir = 706,                   /* ct_dir  */
  YYSYMBOL_ct_key = 707,                   /* ct_key  */
  YYSYMBOL_ct_key_dir = 708,               /* ct_key_dir  */
  YYSYMBOL_ct_key_proto_field = 709,       /* ct_key_proto_field  */
  YYSYMBOL_ct_key_dir_optional = 710,      /* ct_key_dir_optional  */
  YYSYMBOL_symbol_stmt_expr = 711,         /* symbol_stmt_expr  */
  YYSYMBOL_list_stmt_expr = 712,           /* list_stmt_expr  */
  YYSYMBOL_ct_stmt = 713,                  /* ct_stmt  */
  YYSYMBOL_payload_stmt = 714,             /* payload_stmt  */
  YYSYMBOL_payload_expr = 715,             /* payload_expr  */
  YYSYMBOL_payload_raw_expr = 716,         /* payload_raw_expr  */
  YYSYMBOL_payload_base_spec = 717,        /* payload_base_spec  */
  YYSYMBOL_eth_hdr_expr = 718,             /* eth_hdr_expr  */
  YYSYMBOL_eth_hdr_field = 719,            /* eth_hdr_field  */
  YYSYMBOL_vlan_hdr_expr = 720,            /* vlan_hdr_expr  */
  YYSYMBOL_vlan_hdr_field = 721,           /* vlan_hdr_field  */
  YYSYMBOL_arp_hdr_expr = 722,             /* arp_hdr_expr  */
  YYSYMBOL_arp_hdr_field = 723,            /* arp_hdr_field  */
  YYSYMBOL_ip_hdr_expr = 724,              /* ip_hdr_expr  */
  YYSYMBOL_ip_hdr_field = 725,             /* ip_hdr_field  */
  YYSYMBOL_ip_option_type = 726,           /* ip_option_type  */
  YYSYMBOL_ip_option_field = 727,          /* ip_option_field  */
  YYSYMBOL_icmp_hdr_expr = 728,            /* icmp_hdr_expr  */
  YYSYMBOL_icmp_hdr_field = 729,           /* icmp_hdr_field  */
  YYSYMBOL_igmp_hdr_expr = 730,            /* igmp_hdr_expr  */
  YYSYMBOL_igmp_hdr_field = 731,           /* igmp_hdr_field  */
  YYSYMBOL_ip6_hdr_expr = 732,             /* ip6_hdr_expr  */
  YYSYMBOL_ip6_hdr_field = 733,            /* ip6_hdr_field  */
  YYSYMBOL_icmp6_hdr_expr = 734,           /* icmp6_hdr_expr  */
  YYSYMBOL_icmp6_hdr_field = 735,          /* icmp6_hdr_field  */
  YYSYMBOL_auth_hdr_expr = 736,            /* auth_hdr_expr  */
  YYSYMBOL_auth_hdr_field = 737,           /* auth_hdr_field  */
  YYSYMBOL_esp_hdr_expr = 738,             /* esp_hdr_expr  */
  YYSYMBOL_esp_hdr_field = 739,            /* esp_hdr_field  */
  YYSYMBOL_comp_hdr_expr = 740,            /* comp_hdr_expr  */
  YYSYMBOL_comp_hdr_field = 741,           /* comp_hdr_field  */
  YYSYMBOL_udp_hdr_expr = 742,             /* udp_hdr_expr  */
  YYSYMBOL_udp_hdr_field = 743,            /* udp_hdr_field  */
  YYSYMBOL_udplite_hdr_expr = 744,         /* udplite_hdr_expr  */
  YYSYMBOL_udplite_hdr_field = 745,        /* udplite_hdr_field  */
  YYSYMBOL_tcp_hdr_expr = 746,             /* tcp_hdr_expr  */
  YYSYMBOL_optstrip_stmt = 747,            /* optstrip_stmt  */
  YYSYMBOL_tcp_hdr_field = 748,            /* tcp_hdr_field  */
  YYSYMBOL_tcp_hdr_option_kind_and_field = 749, /* tcp_hdr_option_kind_and_field  */
  YYSYMBOL_tcp_hdr_option_sack = 750,      /* tcp_hdr_option_sack  */
  YYSYMBOL_tcp_hdr_option_type = 751,      /* tcp_hdr_option_type  */
  YYSYMBOL_tcpopt_field_sack = 752,        /* tcpopt_field_sack  */
  YYSYMBOL_tcpopt_field_window = 753,      /* tcpopt_field_window  */
  YYSYMBOL_tcpopt_field_tsopt = 754,       /* tcpopt_field_tsopt  */
  YYSYMBOL_tcpopt_field_maxseg = 755,      /* tcpopt_field_maxseg  */
  YYSYMBOL_tcpopt_field_mptcp = 756,       /* tcpopt_field_mptcp  */
  YYSYMBOL_dccp_hdr_expr = 757,            /* dccp_hdr_expr  */
  YYSYMBOL_dccp_hdr_field = 758,           /* dccp_hdr_field  */
  YYSYMBOL_sctp_chunk_type = 759,          /* sctp_chunk_type  */
  YYSYMBOL_sctp_chunk_common_field = 760,  /* sctp_chunk_common_field  */
  YYSYMBOL_sctp_chunk_data_field = 761,    /* sctp_chunk_data_field  */
  YYSYMBOL_sctp_chunk_init_field = 762,    /* sctp_chunk_init_field  */
  YYSYMBOL_sctp_chunk_sack_field = 763,    /* sctp_chunk_sack_field  */
  YYSYMBOL_sctp_chunk_alloc = 764,         /* sctp_chunk_alloc  */
  YYSYMBOL_sctp_hdr_expr = 765,            /* sctp_hdr_expr  */
  YYSYMBOL_sctp_hdr_field = 766,           /* sctp_hdr_field  */
  YYSYMBOL_th_hdr_expr = 767,              /* th_hdr_expr  */
  YYSYMBOL_th_hdr_field = 768,             /* th_hdr_field  */
  YYSYMBOL_exthdr_expr = 769,              /* exthdr_expr  */
  YYSYMBOL_hbh_hdr_expr = 770,             /* hbh_hdr_expr  */
  YYSYMBOL_hbh_hdr_field = 771,            /* hbh_hdr_field  */
  YYSYMBOL_rt_hdr_expr = 772,              /* rt_hdr_expr  */
  YYSYMBOL_rt_hdr_field = 773,             /* rt_hdr_field  */
  YYSYMBOL_rt0_hdr_expr = 774,             /* rt0_hdr_expr  */
  YYSYMBOL_rt0_hdr_field = 775,            /* rt0_hdr_field  */
  YYSYMBOL_rt2_hdr_expr = 776,             /* rt2_hdr_expr  */
  YYSYMBOL_rt2_hdr_field = 777,            /* rt2_hdr_field  */
  YYSYMBOL_rt4_hdr_expr = 778,             /* rt4_hdr_expr  */
  YYSYMBOL_rt4_hdr_field = 779,            /* rt4_hdr_field  */
  YYSYMBOL_frag_hdr_expr = 780,            /* frag_hdr_expr  */
  YYSYMBOL_frag_hdr_field = 781,           /* frag_hdr_field  */
  YYSYMBOL_dst_hdr_expr = 782,             /* dst_hdr_expr  */
  YYSYMBOL_dst_hdr_field = 783,            /* dst_hdr_field  */
  YYSYMBOL_mh_hdr_expr = 784,              /* mh_hdr_expr  */
  YYSYMBOL_mh_hdr_field = 785,             /* mh_hdr_field  */
  YYSYMBOL_exthdr_exists_expr = 786,       /* exthdr_exists_expr  */
  YYSYMBOL_exthdr_key = 787                /* exthdr_key  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   7977

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  362
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  426
/* YYNRULES -- Number of rules.  */
#define YYNRULES  1279
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  2208

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   607


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int16 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,   359,     2,     2,     2,
     356,   357,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   353,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   360,     2,   361,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   354,   358,   355,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   914,   914,   915,   924,   925,   928,   929,   932,   933,
     934,   935,   936,   937,   938,   939,   940,   941,   942,   943,
     944,   945,   946,   947,   948,   949,   950,   951,   952,   953,
     954,   955,   956,   957,   958,   959,   960,   961,   962,   963,
     964,   965,   966,   967,   968,   969,   970,   971,   972,   973,
     974,   975,   976,   977,   978,   979,   980,   982,   983,   985,
     993,  1008,  1015,  1027,  1035,  1036,  1037,  1038,  1058,  1059,
    1060,  1061,  1062,  1063,  1064,  1065,  1066,  1067,  1068,  1069,
    1070,  1071,  1072,  1075,  1079,  1086,  1090,  1098,  1102,  1106,
    1113,  1120,  1124,  1131,  1140,  1144,  1148,  1152,  1156,  1160,
    1164,  1168,  1172,  1176,  1180,  1184,  1188,  1194,  1200,  1204,
    1211,  1215,  1223,  1230,  1237,  1241,  1248,  1257,  1261,  1265,
    1269,  1273,  1277,  1281,  1285,  1291,  1297,  1298,  1301,  1302,
    1305,  1306,  1309,  1310,  1313,  1317,  1321,  1325,  1329,  1333,
    1337,  1341,  1345,  1352,  1356,  1360,  1366,  1370,  1374,  1380,
    1386,  1390,  1394,  1398,  1402,  1406,  1410,  1414,  1418,  1422,
    1426,  1430,  1434,  1438,  1442,  1446,  1450,  1454,  1458,  1462,
    1466,  1470,  1474,  1478,  1482,  1486,  1490,  1494,  1498,  1502,
    1506,  1510,  1514,  1518,  1524,  1530,  1534,  1544,  1548,  1552,
    1556,  1560,  1564,  1570,  1574,  1578,  1582,  1586,  1590,  1594,
    1600,  1607,  1613,  1621,  1627,  1635,  1644,  1645,  1648,  1649,
    1650,  1651,  1652,  1653,  1654,  1655,  1658,  1659,  1662,  1663,
    1664,  1667,  1676,  1682,  1697,  1707,  1708,  1709,  1710,  1711,
    1722,  1732,  1743,  1753,  1764,  1775,  1784,  1793,  1802,  1813,
    1824,  1838,  1844,  1845,  1846,  1847,  1848,  1849,  1850,  1855,
    1865,  1866,  1867,  1874,  1895,  1906,  1917,  1930,  1935,  1936,
    1937,  1938,  1943,  1949,  1954,  1959,  1964,  1970,  1975,  1980,
    1981,  1992,  1993,  1996,  2000,  2003,  2004,  2005,  2006,  2010,
    2015,  2016,  2017,  2018,  2019,  2022,  2023,  2024,  2025,  2030,
    2040,  2051,  2062,  2074,  2083,  2088,  2094,  2099,  2108,  2111,
    2115,  2121,  2122,  2126,  2131,  2132,  2133,  2134,  2148,  2152,
    2156,  2162,  2167,  2174,  2179,  2184,  2187,  2194,  2201,  2208,
    2221,  2228,  2229,  2241,  2246,  2247,  2248,  2249,  2253,  2263,
    2264,  2265,  2266,  2270,  2280,  2281,  2282,  2283,  2287,  2298,
    2302,  2303,  2304,  2308,  2318,  2319,  2320,  2321,  2325,  2335,
    2336,  2337,  2338,  2342,  2352,  2353,  2354,  2355,  2359,  2369,
    2370,  2371,  2372,  2376,  2386,  2387,  2388,  2389,  2390,  2393,
    2424,  2431,  2435,  2438,  2448,  2455,  2466,  2479,  2494,  2495,
    2498,  2510,  2516,  2520,  2523,  2529,  2542,  2547,  2556,  2557,
    2560,  2563,  2564,  2565,  2568,  2583,  2584,  2587,  2588,  2589,
    2590,  2591,  2592,  2595,  2604,  2613,  2621,  2629,  2637,  2645,
    2653,  2661,  2669,  2677,  2685,  2693,  2701,  2709,  2717,  2725,
    2733,  2737,  2742,  2750,  2757,  2764,  2778,  2782,  2789,  2793,
    2799,  2811,  2817,  2824,  2830,  2837,  2838,  2839,  2840,  2843,
    2844,  2845,  2846,  2847,  2848,  2849,  2850,  2851,  2852,  2853,
    2854,  2855,  2856,  2857,  2858,  2859,  2860,  2861,  2862,  2863,
    2866,  2867,  2870,  2879,  2883,  2889,  2895,  2900,  2903,  2908,
    2913,  2916,  2922,  2927,  2935,  2936,  2938,  2942,  2950,  2954,
    2957,  2961,  2967,  2968,  2971,  2977,  2981,  2984,  3109,  3114,
    3119,  3124,  3129,  3135,  3165,  3169,  3173,  3177,  3181,  3187,
    3191,  3194,  3198,  3204,  3218,  3232,  3240,  3241,  3242,  3245,
    3246,  3249,  3250,  3265,  3281,  3289,  3290,  3291,  3294,  3295,
    3298,  3305,  3306,  3309,  3323,  3330,  3331,  3346,  3347,  3348,
    3349,  3350,  3353,  3356,  3362,  3368,  3372,  3376,  3383,  3390,
    3397,  3404,  3410,  3416,  3422,  3425,  3426,  3429,  3435,  3441,
    3447,  3454,  3461,  3469,  3470,  3473,  3477,  3485,  3489,  3492,
    3497,  3502,  3506,  3512,  3528,  3547,  3553,  3554,  3560,  3561,
    3567,  3568,  3569,  3570,  3571,  3572,  3573,  3574,  3575,  3576,
    3577,  3578,  3579,  3582,  3583,  3587,  3593,  3594,  3600,  3601,
    3607,  3608,  3614,  3617,  3618,  3629,  3630,  3633,  3637,  3640,
    3646,  3652,  3653,  3656,  3657,  3658,  3661,  3665,  3669,  3674,
    3679,  3684,  3690,  3694,  3698,  3702,  3708,  3713,  3717,  3725,
    3734,  3735,  3738,  3741,  3745,  3750,  3756,  3757,  3760,  3763,
    3767,  3771,  3775,  3780,  3787,  3792,  3800,  3805,  3814,  3815,
    3821,  3822,  3823,  3826,  3827,  3831,  3835,  3841,  3842,  3845,
    3851,  3855,  3858,  3863,  3869,  3870,  3873,  3874,  3875,  3881,
    3882,  3883,  3884,  3887,  3888,  3894,  3895,  3898,  3899,  3902,
    3908,  3915,  3922,  3933,  3934,  3935,  3938,  3946,  3958,  3965,
    3968,  3974,  3978,  3981,  3987,  3996,  4007,  4013,  4039,  4040,
    4049,  4050,  4053,  4062,  4073,  4074,  4075,  4076,  4077,  4078,
    4079,  4080,  4081,  4082,  4083,  4084,  4085,  4086,  4087,  4090,
    4113,  4114,  4115,  4118,  4119,  4120,  4121,  4122,  4125,  4129,
    4132,  4136,  4143,  4146,  4162,  4163,  4167,  4173,  4174,  4180,
    4181,  4187,  4188,  4194,  4197,  4198,  4209,  4215,  4221,  4222,
    4225,  4231,  4232,  4233,  4236,  4243,  4248,  4253,  4256,  4260,
    4264,  4270,  4271,  4278,  4284,  4285,  4288,  4289,  4292,  4298,
    4304,  4308,  4311,  4315,  4319,  4329,  4333,  4336,  4342,  4349,
    4353,  4359,  4373,  4387,  4392,  4400,  4404,  4408,  4418,  4421,
    4422,  4425,  4426,  4427,  4428,  4439,  4450,  4456,  4477,  4483,
    4500,  4506,  4507,  4508,  4511,  4512,  4513,  4516,  4517,  4520,
    4536,  4542,  4548,  4555,  4569,  4577,  4585,  4591,  4595,  4599,
    4603,  4607,  4614,  4619,  4630,  4644,  4650,  4654,  4658,  4662,
    4666,  4670,  4674,  4678,  4684,  4690,  4698,  4699,  4700,  4703,
    4704,  4708,  4714,  4715,  4721,  4722,  4728,  4729,  4735,  4738,
    4739,  4740,  4749,  4760,  4761,  4764,  4772,  4773,  4774,  4775,
    4776,  4777,  4778,  4779,  4780,  4781,  4782,  4783,  4786,  4787,
    4788,  4789,  4790,  4797,  4804,  4811,  4818,  4825,  4832,  4839,
    4846,  4853,  4860,  4867,  4874,  4877,  4878,  4879,  4880,  4881,
    4882,  4883,  4886,  4890,  4894,  4898,  4902,  4906,  4912,  4913,
    4923,  4927,  4931,  4947,  4948,  4951,  4952,  4953,  4954,  4955,
    4958,  4959,  4960,  4961,  4962,  4963,  4964,  4965,  4966,  4967,
    4968,  4969,  4970,  4971,  4972,  4973,  4974,  4975,  4976,  4977,
    4978,  4979,  4980,  4981,  4984,  5004,  5008,  5022,  5026,  5030,
    5036,  5040,  5046,  5047,  5048,  5051,  5052,  5055,  5056,  5059,
    5065,  5066,  5069,  5070,  5073,  5074,  5077,  5078,  5081,  5089,
    5116,  5121,  5126,  5132,  5133,  5136,  5140,  5160,  5161,  5162,
    5163,  5166,  5170,  5174,  5180,  5181,  5184,  5185,  5186,  5187,
    5188,  5189,  5190,  5191,  5192,  5193,  5194,  5195,  5196,  5197,
    5198,  5199,  5200,  5203,  5204,  5205,  5206,  5207,  5208,  5209,
    5212,  5213,  5214,  5215,  5218,  5219,  5220,  5221,  5224,  5225,
    5228,  5234,  5242,  5255,  5262,  5268,  5274,  5283,  5284,  5285,
    5286,  5287,  5288,  5289,  5290,  5291,  5292,  5293,  5294,  5295,
    5296,  5297,  5298,  5299,  5300,  5303,  5312,  5313,  5314,  5315,
    5328,  5334,  5335,  5336,  5339,  5345,  5346,  5347,  5348,  5349,
    5352,  5358,  5359,  5360,  5361,  5362,  5363,  5364,  5365,  5366,
    5369,  5373,  5381,  5388,  5389,  5390,  5391,  5392,  5393,  5394,
    5395,  5396,  5397,  5398,  5399,  5402,  5403,  5404,  5405,  5408,
    5409,  5410,  5411,  5412,  5415,  5421,  5422,  5423,  5424,  5425,
    5426,  5427,  5430,  5436,  5437,  5438,  5439,  5442,  5448,  5449,
    5450,  5451,  5452,  5453,  5454,  5455,  5456,  5458,  5464,  5465,
    5466,  5467,  5468,  5469,  5470,  5471,  5474,  5480,  5481,  5482,
    5483,  5484,  5487,  5493,  5494,  5497,  5503,  5504,  5505,  5508,
    5514,  5515,  5516,  5517,  5520,  5526,  5527,  5528,  5529,  5532,
    5536,  5541,  5545,  5552,  5559,  5560,  5561,  5562,  5563,  5564,
    5565,  5566,  5567,  5568,  5571,  5576,  5581,  5586,  5591,  5596,
    5603,  5604,  5605,  5606,  5607,  5610,  5611,  5612,  5613,  5614,
    5615,  5616,  5617,  5618,  5619,  5620,  5621,  5630,  5631,  5634,
    5637,  5638,  5641,  5644,  5647,  5653,  5654,  5655,  5658,  5659,
    5660,  5661,  5662,  5663,  5664,  5665,  5666,  5667,  5668,  5669,
    5670,  5671,  5672,  5673,  5674,  5675,  5678,  5679,  5680,  5683,
    5684,  5685,  5686,  5689,  5690,  5691,  5692,  5693,  5696,  5697,
    5698,  5699,  5702,  5707,  5711,  5715,  5719,  5723,  5727,  5732,
    5737,  5742,  5747,  5752,  5759,  5763,  5769,  5770,  5771,  5772,
    5775,  5783,  5784,  5787,  5788,  5789,  5790,  5791,  5792,  5793,
    5794,  5797,  5803,  5804,  5807,  5813,  5814,  5815,  5816,  5819,
    5825,  5831,  5837,  5840,  5846,  5847,  5848,  5849,  5855,  5861,
    5862,  5863,  5864,  5865,  5866,  5869,  5875,  5876,  5879,  5885,
    5886,  5887,  5888,  5889,  5892,  5906,  5907,  5908,  5909,  5910
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "\"junk\"",
  "\"newline\"", "\"colon\"", "\"semicolon\"", "\"comma\"", "\".\"",
  "\"==\"", "\"!=\"", "\"<\"", "\">\"", "\">=\"", "\"<=\"", "\"<<\"",
  "\">>\"", "\"&\"", "\"^\"", "\"!\"", "\"/\"", "\"*\"", "\"-\"", "\"@\"",
  "\"vmap\"", "\"+\"", "\"include\"", "\"define\"", "\"redefine\"",
  "\"undefine\"", "\"fib\"", "\"socket\"", "\"transparent\"",
  "\"wildcard\"", "\"cgroupv2\"", "\"tproxy\"", "\"osf\"", "\"synproxy\"",
  "\"mss\"", "\"wscale\"", "\"typeof\"", "\"hook\"", "\"hooks\"",
  "\"device\"", "\"devices\"", "\"table\"", "\"tables\"", "\"chain\"",
  "\"chains\"", "\"rule\"", "\"rules\"", "\"sets\"", "\"set\"",
  "\"element\"", "\"map\"", "\"maps\"", "\"flowtable\"", "\"handle\"",
  "\"ruleset\"", "\"trace\"", "\"inet\"", "\"netdev\"", "\"add\"",
  "\"update\"", "\"replace\"", "\"create\"", "\"insert\"", "\"delete\"",
  "\"get\"", "\"list\"", "\"reset\"", "\"flush\"", "\"rename\"",
  "\"describe\"", "\"import\"", "\"export\"", "\"monitor\"", "\"all\"",
  "\"accept\"", "\"drop\"", "\"continue\"", "\"jump\"", "\"goto\"",
  "\"return\"", "\"to\"", "\"constant\"", "\"interval\"", "\"dynamic\"",
  "\"auto-merge\"", "\"timeout\"", "\"gc-interval\"", "\"elements\"",
  "\"expires\"", "\"policy\"", "\"memory\"", "\"performance\"", "\"size\"",
  "\"flow\"", "\"offload\"", "\"meter\"", "\"meters\"", "\"flowtables\"",
  "\"number\"", "\"string\"", "\"quoted string\"",
  "\"string with a trailing asterisk\"", "\"ll\"", "\"nh\"", "\"th\"",
  "\"bridge\"", "\"ether\"", "\"saddr\"", "\"daddr\"", "\"type\"",
  "\"vlan\"", "\"id\"", "\"cfi\"", "\"dei\"", "\"pcp\"", "\"arp\"",
  "\"htype\"", "\"ptype\"", "\"hlen\"", "\"plen\"", "\"operation\"",
  "\"ip\"", "\"version\"", "\"hdrlength\"", "\"dscp\"", "\"ecn\"",
  "\"length\"", "\"frag-off\"", "\"ttl\"", "\"protocol\"", "\"checksum\"",
  "\"ptr\"", "\"value\"", "\"lsrr\"", "\"rr\"", "\"ssrr\"", "\"ra\"",
  "\"icmp\"", "\"code\"", "\"seq\"", "\"gateway\"", "\"mtu\"", "\"igmp\"",
  "\"mrt\"", "\"options\"", "\"ip6\"", "\"priority\"", "\"flowlabel\"",
  "\"nexthdr\"", "\"hoplimit\"", "\"icmpv6\"", "\"param-problem\"",
  "\"max-delay\"", "\"ah\"", "\"reserved\"", "\"spi\"", "\"esp\"",
  "\"comp\"", "\"flags\"", "\"cpi\"", "\"port\"", "\"udp\"", "\"sport\"",
  "\"dport\"", "\"udplite\"", "\"csumcov\"", "\"tcp\"", "\"ackseq\"",
  "\"doff\"", "\"window\"", "\"urgptr\"", "\"option\"", "\"echo\"",
  "\"eol\"", "\"mptcp\"", "\"nop\"", "\"sack\"", "\"sack0\"", "\"sack1\"",
  "\"sack2\"", "\"sack3\"", "\"sack-permitted\"", "\"fastopen\"",
  "\"md5sig\"", "\"timestamp\"", "\"count\"", "\"left\"", "\"right\"",
  "\"tsval\"", "\"tsecr\"", "\"subtype\"", "\"dccp\"", "\"sctp\"",
  "\"chunk\"", "\"data\"", "\"init\"", "\"init-ack\"", "\"heartbeat\"",
  "\"heartbeat-ack\"", "\"abort\"", "\"shutdown\"", "\"shutdown-ack\"",
  "\"error\"", "\"cookie-echo\"", "\"cookie-ack\"", "\"ecne\"", "\"cwr\"",
  "\"shutdown-complete\"", "\"asconf-ack\"", "\"forward-tsn\"",
  "\"asconf\"", "\"tsn\"", "\"stream\"", "\"ssn\"", "\"ppid\"",
  "\"init-tag\"", "\"a-rwnd\"", "\"num-outbound-streams\"",
  "\"num-inbound-streams\"", "\"initial-tsn\"", "\"cum-tsn-ack\"",
  "\"num-gap-ack-blocks\"", "\"num-dup-tsns\"", "\"lowest-tsn\"",
  "\"seqno\"", "\"new-cum-tsn\"", "\"vtag\"", "\"rt\"", "\"rt0\"",
  "\"rt2\"", "\"srh\"", "\"seg-left\"", "\"addr\"", "\"last-entry\"",
  "\"tag\"", "\"sid\"", "\"hbh\"", "\"frag\"", "\"reserved2\"",
  "\"more-fragments\"", "\"dst\"", "\"mh\"", "\"meta\"", "\"mark\"",
  "\"iif\"", "\"iifname\"", "\"iiftype\"", "\"oif\"", "\"oifname\"",
  "\"oiftype\"", "\"skuid\"", "\"skgid\"", "\"nftrace\"", "\"rtclassid\"",
  "\"ibriport\"", "\"obriport\"", "\"ibrname\"", "\"obrname\"",
  "\"pkttype\"", "\"cpu\"", "\"iifgroup\"", "\"oifgroup\"", "\"cgroup\"",
  "\"time\"", "\"classid\"", "\"nexthop\"", "\"ct\"", "\"l3proto\"",
  "\"proto-src\"", "\"proto-dst\"", "\"zone\"", "\"direction\"",
  "\"event\"", "\"expectation\"", "\"expiration\"", "\"helper\"",
  "\"label\"", "\"state\"", "\"status\"", "\"original\"", "\"reply\"",
  "\"counter\"", "\"name\"", "\"packets\"", "\"bytes\"", "\"avgpkt\"",
  "\"counters\"", "\"quotas\"", "\"limits\"", "\"synproxys\"",
  "\"helpers\"", "\"log\"", "\"prefix\"", "\"group\"", "\"snaplen\"",
  "\"queue-threshold\"", "\"level\"", "\"limit\"", "\"rate\"", "\"burst\"",
  "\"over\"", "\"until\"", "\"quota\"", "\"used\"", "\"secmark\"",
  "\"secmarks\"", "\"second\"", "\"minute\"", "\"hour\"", "\"day\"",
  "\"week\"", "\"reject\"", "\"with\"", "\"icmpx\"", "\"snat\"",
  "\"dnat\"", "\"masquerade\"", "\"redirect\"", "\"random\"",
  "\"fully-random\"", "\"persistent\"", "\"queue\"", "\"num\"",
  "\"bypass\"", "\"fanout\"", "\"dup\"", "\"fwd\"", "\"numgen\"",
  "\"inc\"", "\"mod\"", "\"offset\"", "\"jhash\"", "\"symhash\"",
  "\"seed\"", "\"position\"", "\"index\"", "\"comment\"", "\"xml\"",
  "\"json\"", "\"vm\"", "\"notrack\"", "\"exists\"", "\"missing\"",
  "\"exthdr\"", "\"ipsec\"", "\"reqid\"", "\"spnum\"", "\"in\"", "\"out\"",
  "'='", "'{'", "'}'", "'('", "')'", "'|'", "'$'", "'['", "']'", "$accept",
  "input", "stmt_separator", "opt_newline", "close_scope_ah",
  "close_scope_arp", "close_scope_at", "close_scope_comp",
  "close_scope_ct", "close_scope_counter", "close_scope_dccp",
  "close_scope_dst", "close_scope_dup", "close_scope_esp",
  "close_scope_eth", "close_scope_export", "close_scope_fib",
  "close_scope_flags", "close_scope_frag", "close_scope_fwd",
  "close_scope_hash", "close_scope_hbh", "close_scope_ip",
  "close_scope_ip6", "close_scope_vlan", "close_scope_icmp",
  "close_scope_igmp", "close_scope_import", "close_scope_ipsec",
  "close_scope_list", "close_scope_limit", "close_scope_meta",
  "close_scope_mh", "close_scope_monitor", "close_scope_nat",
  "close_scope_numgen", "close_scope_osf", "close_scope_policy",
  "close_scope_quota", "close_scope_queue", "close_scope_reject",
  "close_scope_reset", "close_scope_rt", "close_scope_sctp",
  "close_scope_sctp_chunk", "close_scope_secmark", "close_scope_socket",
  "close_scope_tcp", "close_scope_tproxy", "close_scope_type",
  "close_scope_th", "close_scope_udp", "close_scope_udplite",
  "close_scope_log", "close_scope_synproxy", "common_block", "line",
  "base_cmd", "add_cmd", "replace_cmd", "create_cmd", "insert_cmd",
  "table_or_id_spec", "chain_or_id_spec", "set_or_id_spec",
  "obj_or_id_spec", "delete_cmd", "get_cmd", "list_cmd",
  "basehook_device_name", "basehook_spec", "reset_cmd", "flush_cmd",
  "rename_cmd", "import_cmd", "export_cmd", "monitor_cmd", "monitor_event",
  "monitor_object", "monitor_format", "markup_format", "describe_cmd",
  "table_block_alloc", "table_options", "table_block", "chain_block_alloc",
  "chain_block", "subchain_block", "typeof_data_expr", "typeof_expr",
  "set_block_alloc", "set_block", "set_block_expr", "set_flag_list",
  "set_flag", "map_block_alloc", "map_block_obj_type", "map_block",
  "set_mechanism", "set_policy_spec", "flowtable_block_alloc",
  "flowtable_block", "flowtable_expr", "flowtable_list_expr",
  "flowtable_expr_member", "data_type_atom_expr", "data_type_expr",
  "obj_block_alloc", "counter_block", "quota_block", "ct_helper_block",
  "ct_timeout_block", "ct_expect_block", "limit_block", "secmark_block",
  "synproxy_block", "type_identifier", "hook_spec", "prio_spec",
  "extended_prio_name", "extended_prio_spec", "int_num", "dev_spec",
  "flags_spec", "policy_spec", "policy_expr", "chain_policy", "identifier",
  "string", "time_spec", "family_spec", "family_spec_explicit",
  "table_spec", "tableid_spec", "chain_spec", "chainid_spec",
  "chain_identifier", "set_spec", "setid_spec", "set_identifier",
  "flowtable_spec", "flowtableid_spec", "flowtable_identifier", "obj_spec",
  "objid_spec", "obj_identifier", "handle_spec", "position_spec",
  "index_spec", "rule_position", "ruleid_spec", "comment_spec",
  "ruleset_spec", "rule", "rule_alloc", "stmt_list", "stateful_stmt_list",
  "stateful_stmt", "stmt", "chain_stmt_type", "chain_stmt", "verdict_stmt",
  "verdict_map_stmt", "verdict_map_expr", "verdict_map_list_expr",
  "verdict_map_list_member_expr", "connlimit_stmt", "counter_stmt",
  "counter_stmt_alloc", "counter_args", "counter_arg", "log_stmt",
  "log_stmt_alloc", "log_args", "log_arg", "level_type", "log_flags",
  "log_flags_tcp", "log_flag_tcp", "limit_stmt", "quota_mode",
  "quota_unit", "quota_used", "quota_stmt", "limit_mode",
  "limit_burst_pkts", "limit_rate_pkts", "limit_burst_bytes",
  "limit_rate_bytes", "limit_bytes", "time_unit", "reject_stmt",
  "reject_stmt_alloc", "reject_with_expr", "reject_opts", "nat_stmt",
  "nat_stmt_alloc", "tproxy_stmt", "synproxy_stmt", "synproxy_stmt_alloc",
  "synproxy_args", "synproxy_arg", "synproxy_config", "synproxy_obj",
  "synproxy_ts", "synproxy_sack", "primary_stmt_expr", "shift_stmt_expr",
  "and_stmt_expr", "exclusive_or_stmt_expr", "inclusive_or_stmt_expr",
  "basic_stmt_expr", "concat_stmt_expr", "map_stmt_expr_set",
  "map_stmt_expr", "prefix_stmt_expr", "range_stmt_expr",
  "multiton_stmt_expr", "stmt_expr", "nat_stmt_args", "masq_stmt",
  "masq_stmt_alloc", "masq_stmt_args", "redir_stmt", "redir_stmt_alloc",
  "redir_stmt_arg", "dup_stmt", "fwd_stmt", "nf_nat_flags", "nf_nat_flag",
  "queue_stmt", "queue_stmt_compat", "queue_stmt_alloc", "queue_stmt_args",
  "queue_stmt_arg", "queue_expr", "queue_stmt_expr_simple",
  "queue_stmt_expr", "queue_stmt_flags", "queue_stmt_flag",
  "set_elem_expr_stmt", "set_elem_expr_stmt_alloc", "set_stmt",
  "set_stmt_op", "map_stmt", "meter_stmt", "flow_stmt_legacy_alloc",
  "flow_stmt_opts", "flow_stmt_opt", "meter_stmt_alloc", "match_stmt",
  "variable_expr", "symbol_expr", "set_ref_expr", "set_ref_symbol_expr",
  "integer_expr", "primary_expr", "fib_expr", "fib_result", "fib_flag",
  "fib_tuple", "osf_expr", "osf_ttl", "shift_expr", "and_expr",
  "exclusive_or_expr", "inclusive_or_expr", "basic_expr", "concat_expr",
  "prefix_rhs_expr", "range_rhs_expr", "multiton_rhs_expr", "map_expr",
  "expr", "set_expr", "set_list_expr", "set_list_member_expr",
  "meter_key_expr", "meter_key_expr_alloc", "set_elem_expr",
  "set_elem_key_expr", "set_elem_expr_alloc", "set_elem_options",
  "set_elem_option", "set_elem_expr_options", "set_elem_stmt_list",
  "set_elem_stmt", "set_elem_expr_option", "set_lhs_expr", "set_rhs_expr",
  "initializer_expr", "counter_config", "counter_obj", "quota_config",
  "quota_obj", "secmark_config", "secmark_obj", "ct_obj_type",
  "ct_cmd_type", "ct_l4protoname", "ct_helper_config", "timeout_states",
  "timeout_state", "ct_timeout_config", "ct_expect_config", "ct_obj_alloc",
  "limit_config", "limit_obj", "relational_expr", "list_rhs_expr",
  "rhs_expr", "shift_rhs_expr", "and_rhs_expr", "exclusive_or_rhs_expr",
  "inclusive_or_rhs_expr", "basic_rhs_expr", "concat_rhs_expr",
  "boolean_keys", "boolean_expr", "keyword_expr", "primary_rhs_expr",
  "relational_op", "verdict_expr", "chain_expr", "meta_expr", "meta_key",
  "meta_key_qualified", "meta_key_unqualified", "meta_stmt", "socket_expr",
  "socket_key", "offset_opt", "numgen_type", "numgen_expr", "xfrm_spnum",
  "xfrm_dir", "xfrm_state_key", "xfrm_state_proto_key", "xfrm_expr",
  "hash_expr", "nf_key_proto", "rt_expr", "rt_key", "ct_expr", "ct_dir",
  "ct_key", "ct_key_dir", "ct_key_proto_field", "ct_key_dir_optional",
  "symbol_stmt_expr", "list_stmt_expr", "ct_stmt", "payload_stmt",
  "payload_expr", "payload_raw_expr", "payload_base_spec", "eth_hdr_expr",
  "eth_hdr_field", "vlan_hdr_expr", "vlan_hdr_field", "arp_hdr_expr",
  "arp_hdr_field", "ip_hdr_expr", "ip_hdr_field", "ip_option_type",
  "ip_option_field", "icmp_hdr_expr", "icmp_hdr_field", "igmp_hdr_expr",
  "igmp_hdr_field", "ip6_hdr_expr", "ip6_hdr_field", "icmp6_hdr_expr",
  "icmp6_hdr_field", "auth_hdr_expr", "auth_hdr_field", "esp_hdr_expr",
  "esp_hdr_field", "comp_hdr_expr", "comp_hdr_field", "udp_hdr_expr",
  "udp_hdr_field", "udplite_hdr_expr", "udplite_hdr_field", "tcp_hdr_expr",
  "optstrip_stmt", "tcp_hdr_field", "tcp_hdr_option_kind_and_field",
  "tcp_hdr_option_sack", "tcp_hdr_option_type", "tcpopt_field_sack",
  "tcpopt_field_window", "tcpopt_field_tsopt", "tcpopt_field_maxseg",
  "tcpopt_field_mptcp", "dccp_hdr_expr", "dccp_hdr_field",
  "sctp_chunk_type", "sctp_chunk_common_field", "sctp_chunk_data_field",
  "sctp_chunk_init_field", "sctp_chunk_sack_field", "sctp_chunk_alloc",
  "sctp_hdr_expr", "sctp_hdr_field", "th_hdr_expr", "th_hdr_field",
  "exthdr_expr", "hbh_hdr_expr", "hbh_hdr_field", "rt_hdr_expr",
  "rt_hdr_field", "rt0_hdr_expr", "rt0_hdr_field", "rt2_hdr_expr",
  "rt2_hdr_field", "rt4_hdr_expr", "rt4_hdr_field", "frag_hdr_expr",
  "frag_hdr_field", "dst_hdr_expr", "dst_hdr_field", "mh_hdr_expr",
  "mh_hdr_field", "exthdr_exists_expr", "exthdr_key", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   473,   474,
     475,   476,   477,   478,   479,   480,   481,   482,   483,   484,
     485,   486,   487,   488,   489,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   499,   500,   501,   502,   503,   504,
     505,   506,   507,   508,   509,   510,   511,   512,   513,   514,
     515,   516,   517,   518,   519,   520,   521,   522,   523,   524,
     525,   526,   527,   528,   529,   530,   531,   532,   533,   534,
     535,   536,   537,   538,   539,   540,   541,   542,   543,   544,
     545,   546,   547,   548,   549,   550,   551,   552,   553,   554,
     555,   556,   557,   558,   559,   560,   561,   562,   563,   564,
     565,   566,   567,   568,   569,   570,   571,   572,   573,   574,
     575,   576,   577,   578,   579,   580,   581,   582,   583,   584,
     585,   586,   587,   588,   589,   590,   591,   592,   593,   594,
     595,   596,   597,   598,   599,   600,   601,   602,   603,   604,
     605,   606,   607,    61,   123,   125,    40,    41,   124,    36,
      91,    93
};
#endif

#define YYPACT_NINF (-1710)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1000)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
   -1710,  7377, -1710,   447, -1710, -1710,   154,   162,   162,   162,
    1394,  1394,  1394,  1394,  1394,  1394,  1394,  1394, -1710, -1710,
    2579,   285,  1929,   291,  2007,   310,  6920,   620,  1942,   320,
    6844,    95,   129,   314, -1710, -1710, -1710, -1710,   403,  1394,
    1394,  1394,  1394, -1710, -1710, -1710,   705, -1710,   162, -1710,
     162,   157,  6225, -1710,   447, -1710,   177,   248,   447,   162,
   -1710,   254,   290,  6225,   162, -1710,  -120, -1710,   162, -1710,
   -1710,  1394, -1710,  1394,  1394,  1394,  1394,  1394,  1394,  1394,
     481,  1394,  1394,  1394,  1394, -1710,  1394, -1710,  1394,  1394,
    1394,  1394,  1394,  1394,  1394,  1394,   492,  1394,  1394,  1394,
    1394, -1710,  1394, -1710,  1394,  1394,  1394,  1394,  1394,  1394,
    1041,  1394,  1394,  1394,  1394,  1394,   267,  1394,  1394,  1394,
     376,  1394,  1428,  1461,  1791,  1846,  1394,  1394,  1394,  1909,
   -1710,  1394,  2129,  2205,  1394, -1710,  1394,  1394,  1394,  1394,
    1394,   440,  1394, -1710,  1394, -1710,  1091,   596,   370,   516,
   -1710, -1710, -1710, -1710,   466,  1190,  1480,  1967,  2680,  1076,
     578,  2444,   997,  1029,   271,   183,   918,   560,  2258,   116,
     814,   902,   425,   432,   663,    91,   891,   226,   641,  7600,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710,  3910, -1710, -1710,   242,  6483,   355,   874,   465,  6844,
     162, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710,   977, -1710, -1710,   353,
   -1710, -1710,   977, -1710, -1710, -1710, -1710,  1513, -1710, -1710,
   -1710,  1394,  1394,  1394,   -79, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710,   597,   639,   707, -1710, -1710, -1710,   151,   431,
     733, -1710, -1710, -1710,   652, -1710, -1710, -1710,    72,    72,
   -1710,   159,   162,  7629,  3573,   548, -1710,   332,   219, -1710,
   -1710, -1710, -1710, -1710,   127,   752,   213, -1710,   846, -1710,
     520,  6225, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
     610, -1710,   625, -1710, -1710, -1710,   563, -1710,  4745, -1710,
   -1710,   589, -1710,   193, -1710,   316, -1710, -1710, -1710, -1710,
    1000, -1710,   117, -1710, -1710,   850, -1710, -1710, -1710,   989,
     868,   885,   555, -1710,   682, -1710,  5750, -1710, -1710, -1710,
     889, -1710, -1710, -1710,   901, -1710, -1710,  6089,  6089, -1710,
   -1710,   114,   566,   582, -1710, -1710,   602, -1710, -1710, -1710,
     607, -1710,   613,   913,  6225, -1710,   254,   290, -1710,  -120,
   -1710, -1710,  1394,  1394,  1394,   701, -1710, -1710, -1710,  6225,
   -1710,   269, -1710, -1710, -1710,   441, -1710, -1710, -1710,   533,
   -1710, -1710, -1710, -1710,   538, -1710, -1710,  -120, -1710,   542,
     637, -1710, -1710, -1710, -1710,  1394, -1710, -1710, -1710, -1710,
    -120, -1710, -1710, -1710,   950, -1710, -1710, -1710, -1710,  1394,
   -1710, -1710, -1710, -1710, -1710, -1710,  1394,  1394, -1710, -1710,
   -1710,   955,   971, -1710,  1394,   976, -1710,  1394, -1710,  1394,
   -1710,  1394, -1710,  1394, -1710, -1710, -1710, -1710,  1394, -1710,
   -1710, -1710,  1394, -1710,  1394, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710,  1394, -1710,   162, -1710, -1710, -1710, -1710,
     988, -1710, -1710, -1710, -1710, -1710,  1015,    92, -1710, -1710,
     726, -1710, -1710,   925,    71, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,   229,   504,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,   956, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710,  2193, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710,  3100, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710,    50, -1710, -1710,
     678, -1710, -1710, -1710, -1710, -1710, -1710,   681, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
    1891, -1710, -1710, -1710, -1710,   740,   615,   743,   951, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,   706,   721,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710,   977, -1710, -1710, -1710, -1710,
     -56,   -64,    12,   119, -1710, -1710, -1710,  4946,  1004,  7013,
    6844,   929, -1710, -1710, -1710, -1710,  1083,  1093,    70,  1086,
    1089,  1099,    77,  1106,  1891,  1116,  7013,  7013,   741,  7013,
   -1710, -1710,  1026,  6844,   755,  7013,  7013,  1051, -1710,  5819,
     122, -1710,  1325, -1710, -1710,   789, -1710,  1081,  1125,   610,
   -1710, -1710,   720,  1325,  1142,  1147,  1151,  1325,   625, -1710,
   -1710,   415, -1710, -1710,  7013, -1710,  5147,  1179,  1190,  1480,
    1967,  2680, -1710,  2444,   263, -1710, -1710, -1710,  1181, -1710,
   -1710, -1710, -1710,  7013, -1710,  1139,  1178,  1250,   912,   729,
     623, -1710, -1710, -1710, -1710,  1269,  1113,  1270, -1710, -1710,
   -1710, -1710,  1290, -1710, -1710, -1710, -1710,   120, -1710, -1710,
    1292,  1299, -1710, -1710, -1710,  1229,  1235, -1710, -1710,   589,
   -1710, -1710,  1334, -1710, -1710, -1710, -1710,  1336, -1710, -1710,
    5348, -1710,  1336, -1710, -1710, -1710,    59, -1710, -1710,  1000,
   -1710,  1338, -1710,   162, -1710,   994, -1710,   162,   106, -1710,
    7484,  7484,  7484,  7484,  7484,  6844,   107,  7214, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710,  7484, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
     527, -1710,  1160,  1342,  1337,   999,   225,  1354, -1710, -1710,
   -1710,  7214,  7013,  7013,  1275,   124,   447,  1373, -1710,   799,
     447,  1281, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710,  1347,  1034,  1038,  1039, -1710,  1044,  1054, -1710, -1710,
   -1710, -1710,  1108,  1097,   900,  1325, -1710, -1710,  1308,  1315,
    1318,  1319, -1710,  1321,  1079, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710,  1322, -1710, -1710, -1710, -1710, -1710,  1394, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710,  1330,   596, -1710, -1710, -1710,
   -1710,  1344, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
     884, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710,  1352, -1710,  1260,
   -1710, -1710,  1258, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710,  1024, -1710,  1045,  1327, -1710, -1710,   960,  1077,
    1524,  1524, -1710, -1710, -1710,  1237, -1710, -1710, -1710, -1710,
    1236,  1238, -1710,  1242,  1243,  1256,   197, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710,  1364, -1710, -1710, -1710,
    1369, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710,  1128, -1710,  1145, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710,  1385,  1388,  1157, -1710, -1710, -1710, -1710,
   -1710,  1391,    66, -1710, -1710, -1710,  1140,  1144,  1150,  1399,
   -1710, -1710,   741, -1710, -1710, -1710,  1406, -1710, -1710, -1710,
   -1710,  7013,  2680,  2444,  1491,  5549, -1710,   117,   174,  1502,
    2285,  1325,  1325,  1407,  6844,  7013,  7013,  7013, -1710,  1410,
    7013,  1462,  7013, -1710, -1710, -1710, -1710,  1418, -1710,    83,
    1503, -1710, -1710,   155,   234,   615, -1710,   307,   427,  1338,
    1481, -1710,  7013, -1710,   846,  1255,   454,   189, -1710,   770,
    1354,   846, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
    1386,   461, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710,   774,   825, -1710,   831, -1710, -1710,  7013,  1530,  7013,
   -1710, -1710, -1710,   300,   507,  7013, -1710, -1710,  1182, -1710,
   -1710,  7013,  7013,  7013,  7013,  7013,  1434,  7013,  7013,   108,
    7013,  1336,  7013,  1454,  1532,  1458,  2717,  2717, -1710, -1710,
   -1710,  7013,  1113,  7013,  1113, -1710,  1533,  1540, -1710,   755,
   -1710,  6844, -1710,  6844, -1710, -1710, -1710,  1160,  1342,  1337,
   -1710,   846, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,  1189,  7484,
    7484,  7484,  7484,  7484,  7484,  7484,  7484,  7588,  7484,  7484,
     651, -1710,   932, -1710, -1710, -1710, -1710, -1710,  1452, -1710,
     766,   658, -1710,  3078,  3469,  2321,  3652,   258, -1710, -1710,
   -1710, -1710, -1710, -1710,  1212,  1215,  1224, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710,  1572, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,  2285,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710,  1220,  1223, -1710, -1710, -1710, -1710, -1710, -1710,
    1157,  -101,  1483, -1710, -1710, -1710, -1710, -1710,  1198, -1710,
   -1710, -1710,  1294,  1071, -1710,  1418,  1120, -1710,   646,    83,
   -1710,   302, -1710, -1710,  7013,  7013,  1582, -1710,  1486,  1486,
   -1710,   174, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710,  1245,  1502,  6225,   174, -1710, -1710, -1710, -1710, -1710,
   -1710,  7013, -1710, -1710,   164,  1287,  1289,  1581, -1710, -1710,
   -1710,  1300,    59, -1710,   101,  7013,  1563, -1710,  7436, -1710,
    1420,  1332,  1320,   454, -1710,  1486,  1486, -1710,   189, -1710,
    5819, -1710,  4478, -1710, -1710, -1710, -1710,  1613, -1710, -1710,
   -1710,  1210, -1710, -1710,  1210, -1710,  1551,  1210, -1710, -1710,
    7013, -1710, -1710, -1710, -1710, -1710,  1139,  1178,  1250, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710,  1627,  7013,  1463,  7013,
   -1710, -1710, -1710, -1710,  1113, -1710,  1113,  1336, -1710, -1710,
     163,  6225,  5867,   131, -1710, -1710, -1710,  1373,  1626, -1710,
   -1710,  1160,  1342,  1337, -1710,   130,  1373, -1710, -1710,   770,
    7484,  7588, -1710,  1534,  1596, -1710, -1710, -1710, -1710, -1710,
     162,   162,   162,   162,   162,  1535,   509,   162,   162,   162,
     162, -1710, -1710, -1710,   447, -1710,   115, -1710,  1541, -1710,
   -1710, -1710,   447,   447,   447,   447,   447,  6844, -1710,  1486,
    1486,  1291,  1221,  1538,   840,  1078,  1457, -1710, -1710, -1710,
     447,   447,   506, -1710,  6844,  1486,  1303,   840,  1078, -1710,
   -1710, -1710,   447,   447,   506,  1544,  1304,  1560, -1710, -1710,
   -1710, -1710, -1710,  4314,  3806,  3122,  3989,   675, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710,   845, -1710, -1710,  1559, -1710,
   -1710, -1710,  1659, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710,  1565, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710,  1036,   493,   474,  1566, -1710, -1710, -1710, -1710, -1710,
    1287,  1289, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710,  1300, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
    7013, -1710, -1710, -1710, -1710, -1710, -1710, -1710,  6844,  1310,
     174, -1710, -1710, -1710, -1710, -1710, -1710, -1710,  1451,  1653,
   -1710,  1573, -1710,  1574, -1710,  1451,  1576, -1710, -1710, -1710,
   -1710,  6844,    59, -1710,  7013,    72,    72,   846,  1354, -1710,
      97,  1579, -1710,   741, -1710, -1710, -1710, -1710, -1710, -1710,
     447, -1710,   461, -1710, -1710, -1710, -1710, -1710, -1710,  7013,
   -1710,  1590, -1710,  1336,  1336,  6844, -1710,   345,  1333,  1691,
     846, -1710,  1373,  1373,  1516,  1603, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710,   162,   162,   162,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
    1611, -1710, -1710, -1710, -1710, -1710, -1710,  1119, -1710,   447,
     447,  -120, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710,  1267, -1710, -1710, -1710, -1710, -1710,   863,
   -1710, -1710, -1710, -1710, -1710,   723,   447,  -120,   730,   863,
   -1710, -1710, -1710,  1556,   338,   447, -1710, -1710, -1710, -1710,
   -1710, -1710,  1205,   903,   890, -1710, -1710,  1617, -1710,  1157,
   -1710, -1710,  1362,   553,  1394, -1710, -1710, -1710, -1710, -1710,
    1486,  1618,   553,  1619,  1394, -1710, -1710, -1710, -1710, -1710,
    1620,  1394, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710,  6225, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710,  1438, -1710,   125, -1710, -1710,
   -1710,    83, -1710, -1710, -1710, -1710, -1710, -1710,  1624,  1439,
    1418, -1710, -1710, -1710, -1710, -1710, -1710, -1710,  7013,  1374,
    6844, -1710, -1710,  1992,  5867, -1710, -1710,  1545,   447,  1377,
    1382,  1396,  1397,  1400, -1710, -1710, -1710, -1710,  1401,  1404,
    1416,  1421, -1710,  1737, -1710,  6844, -1710, -1710, -1710, -1710,
   -1710,   840, -1710,  1078, -1710,  6675, -1710, -1710,  2621, -1710,
     152,   447,    98,   447, -1710, -1710, -1710, -1710, -1710, -1710,
    1725, -1710,  1425, -1710, -1710,   447,   447, -1710,   447,   447,
     447,   447,   447, -1710,  1649,   447, -1710,  1430, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710,  1681,  1287,  1289, -1710, -1710,
   -1710, -1710,  1436,   846, -1710, -1710,  1516, -1710, -1710, -1710,
   -1710, -1710,  1440,  1443,  1444, -1710, -1710, -1710, -1710,  1689,
   -1710, -1710, -1710, -1710, -1710,  6844,   447,  1793,  1794, -1710,
     840, -1710, -1710, -1710, -1710,   447,  1267, -1710,  1701, -1710,
   -1710, -1710,   756, -1710, -1710, -1710, -1710, -1710, -1710,   132,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710,  1702, -1710,  1704,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,   553,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,  1545,  1340,
    4143,  3398,  4414,  1783, -1710, -1710, -1710,  1789,  1658,  1378,
    1412,  1279,  1119, -1710,  6844,  1267, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710,  1703,  1706,   123, -1710, -1710,  1808,   135,
   -1710,   447, -1710, -1710, -1710, -1710,   447,   447,   447,   447,
     447,  1501,   936,  1622,   447,   447,   447,   447,   166,  1467,
    1556, -1710,  1806, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
    1717,  1704,   447, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,   338, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710,   447,   447,   447, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,     1,     0,     4,     5,     0,     0,     0,     0,
     395,   395,   395,   395,   395,   395,   395,   395,   399,   402,
     395,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   206,   401,     9,    26,    27,     0,   395,
     395,   395,   395,    65,    64,     3,     0,    68,     0,   396,
       0,   420,     0,    63,     0,   390,     0,     0,     0,     0,
     565,    83,    85,     0,     0,   257,     0,   279,     0,   303,
      69,   395,    70,   395,   395,   395,   395,   395,   395,   395,
       0,   395,   395,   395,   395,    71,   395,    72,   395,   395,
     395,   395,   395,   395,   395,   395,     0,   395,   395,   395,
     395,    73,   395,    74,   395,   426,   395,   426,   395,   426,
     426,   395,   395,   426,   395,   426,     0,   395,   426,   426,
       0,   395,   426,   426,   426,   426,   395,   395,   395,   426,
      33,   395,   426,   426,   395,    45,   395,   395,   395,   395,
     426,     0,   395,    77,   395,    78,     0,     0,     0,   722,
     693,   391,   392,   393,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     900,   901,   902,   903,   904,   905,   906,   907,   908,   909,
     910,   911,   912,   913,   914,   915,   916,   917,   918,   919,
     921,     0,   923,   922,     0,     0,     0,     0,    32,     0,
       0,    82,   689,   688,   694,   695,   221,   705,   706,   699,
     891,   700,   703,   707,   704,   701,   702,   696,  1007,  1008,
    1009,  1010,  1011,  1012,  1013,  1014,  1015,  1016,  1017,  1018,
    1019,  1020,    51,  1022,  1023,  1024,   697,  1233,  1234,  1235,
    1236,  1237,  1238,  1239,  1240,   698,     0,   218,   219,     0,
      31,   202,     0,    19,   204,   207,    37,   208,   400,   397,
     398,   395,   395,   395,    13,   815,   788,   790,    67,    66,
     403,   405,     0,     0,     0,   422,   421,   423,     0,   555,
       0,   673,   674,   675,     0,   882,   883,   884,   460,   461,
     887,   680,     0,     0,     0,   476,   484,     0,   508,   533,
     545,   546,   622,   628,   649,     0,     0,   927,     7,    88,
     428,   430,   443,   431,   241,   458,   439,   464,   438,    13,
     474,    57,   482,   436,   437,    44,   536,    38,     0,    52,
      58,   553,    38,   621,    38,   627,    16,    23,   449,    43,
     647,   455,     0,   456,   441,     0,   679,   440,   724,   727,
     729,   731,   733,   734,   741,   743,     0,   742,   686,   463,
     891,   444,   450,   442,   696,   459,    59,     0,     0,    62,
     414,     0,     0,     0,    87,   408,     0,    91,   272,   271,
       0,   411,     0,     0,     0,   565,   108,   110,   257,     0,
     279,   303,   395,   395,   395,    13,   815,   788,   790,     0,
      58,     0,   132,   133,   134,     0,   126,   127,   135,     0,
     128,   129,   136,   137,     0,   130,   131,     0,   138,     0,
     140,   141,   792,   793,   791,   395,    13,    34,    42,    49,
       0,    58,   183,   427,   185,   150,   151,   152,   153,   395,
     154,   156,   180,   179,   178,   172,   395,   426,   176,   175,
     177,   792,   793,   794,   395,     0,    13,   395,   157,   395,
     160,   395,   163,   395,   169,    34,    42,    49,   395,   166,
      75,    13,   395,   187,   395,   190,    42,    76,   193,   194,
     195,   196,   199,   395,   198,     0,  1029,  1026,  1027,    54,
       0,   713,   714,   715,   716,   717,   719,     0,   932,   934,
       0,   933,    50,     0,     0,  1231,  1232,    54,  1031,  1032,
      53,    18,    53,  1035,  1036,  1037,  1038,    28,     0,     0,
    1041,  1042,  1043,  1044,  1045,     9,  1063,  1064,  1058,  1053,
    1054,  1055,  1056,  1057,  1059,  1060,  1061,  1062,     0,    26,
      53,  1078,  1077,  1076,  1079,  1080,  1081,    29,    53,  1084,
    1085,  1086,    30,  1095,  1096,  1088,  1089,  1090,  1092,  1091,
    1093,  1094,    27,    53,  1103,  1100,  1099,  1104,  1102,  1101,
    1105,    29,  1108,  1111,  1107,  1109,  1110,     8,  1114,  1113,
      17,  1116,    21,  1118,    11,  1122,  1123,  1120,  1121,    55,
    1128,  1125,  1126,  1127,    56,  1142,  1136,  1139,    21,  1134,
    1135,  1137,  1138,  1141,  1143,     0,  1129,    53,  1175,  1176,
      14,  1229,  1226,  1227,     0,  1228,    47,    53,    26,  1246,
     959,    27,  1245,  1248,   957,   958,    32,     0,    46,    46,
       0,    46,  1252,    46,    21,  1254,  1256,     0,    46,  1243,
    1242,    25,  1264,  1261,  1259,  1260,  1262,  1263,    22,  1267,
    1266,    15,    53,  1270,  1273,  1269,  1272,    36,    35,   895,
     896,   897,    49,   898,    32,    35,   893,   894,   974,   975,
     981,   967,   968,   966,   976,   977,   997,   970,   979,   972,
     973,   978,   969,   971,   964,   965,   995,   994,   996,    49,
       0,    12,   982,   938,   937,     0,   741,     0,     0,    46,
      25,    22,    15,    36,  1274,   942,   943,   920,   941,     0,
     687,  1021,   201,   220,    79,   203,    80,    81,   209,   210,
     212,   211,   214,   215,   213,   216,   812,   812,   812,    93,
       0,     0,   508,     0,   417,   418,   419,     0,     0,     0,
       0,     0,   889,   888,   885,   886,     0,     0,     0,    35,
      35,     0,     0,     0,     0,    12,     0,     0,   517,     0,
     506,   507,     0,     0,     0,     0,     0,     0,     6,     0,
       0,   745,     0,   429,   432,     0,   435,     0,     0,   475,
     478,   445,     0,     0,     0,     0,     0,     0,   483,   485,
     446,     0,   532,   447,     0,    45,     0,     0,    18,    28,
       9,    26,   853,    27,     0,   857,   855,   856,     0,    38,
      38,   843,   844,     0,   583,   586,   588,   590,   592,   593,
     598,   603,   601,   602,   604,   606,   544,   570,   571,   581,
     845,   572,   579,   573,   580,   576,   577,     0,   574,   575,
       0,   605,   578,   448,   457,     0,     0,   562,   561,   554,
     557,   451,     0,   640,   641,   642,   620,   625,   638,   452,
       0,   626,   631,   453,   454,   643,     0,   665,   666,   648,
     650,   653,   663,     0,   691,     0,   690,     0,     0,   681,
       0,     0,     0,     0,     0,     0,     0,     0,   875,   876,
     877,   878,   879,   880,   881,    18,    28,     9,    26,    29,
     868,    27,    29,     8,    17,    11,    55,    56,    51,    14,
      47,    38,     0,   858,   828,   859,   738,   739,   840,   827,
     817,   816,   832,   834,   836,   838,   839,   826,   860,   861,
     829,     0,     0,     0,     0,     7,     0,   782,   781,   839,
       0,     0,   359,    58,   225,   242,   258,   285,   304,   424,
     107,     0,     0,     0,     0,   114,     0,     0,   812,   812,
     812,   116,     0,     0,   508,     0,   125,   148,     0,     0,
       0,     0,   139,     0,     0,   812,   143,   146,   144,   147,
     149,   171,     0,   186,   155,   174,   173,    12,   395,   159,
     158,   161,   164,   170,   165,   162,   168,   167,   189,   188,
     191,   192,   197,   200,  1028,     0,     0,    53,   710,   711,
      20,     0,   930,   723,    40,    40,  1230,  1033,  1030,  1039,
    1034,    18,    26,    18,    26,  1040,  1065,  1066,  1067,  1068,
      26,  1050,  1075,  1074,  1083,  1082,  1087,  1098,  1097,  1106,
    1112,  1117,  1115,  1119,  1124,  1140,    10,  1160,  1166,  1164,
    1155,  1156,  1159,  1161,  1150,  1151,  1152,  1153,  1154,  1162,
    1157,  1158,  1163,  1131,  1165,  1130,  1177,  1174,  1181,  1178,
    1179,  1180,  1182,  1183,  1184,  1185,  1186,  1187,  1188,  1189,
    1190,  1191,  1192,  1193,  1194,  1195,  1212,    48,  1224,  1247,
     953,   954,   960,    46,   955,  1244,     0,  1249,  1251,  1255,
       0,  1253,  1241,  1258,  1265,  1271,  1268,   892,   899,   890,
     980,   983,   984,     0,   986,     0,   985,   987,   988,    12,
      12,   989,   961,     0,     0,   935,  1276,  1275,  1277,  1278,
    1279,     0,     0,   708,   205,   217,     0,     0,     0,     0,
     324,    13,   517,   349,    34,   329,     0,    42,   354,   789,
      49,     0,    26,    27,   547,     0,   556,     0,   667,   669,
       0,     0,     0,     0,     0,     0,     0,     0,    12,     0,
       0,   989,     0,   477,    34,   515,   516,     0,    42,     0,
       0,   662,    43,   657,   656,     0,   661,   659,   660,    21,
     634,   636,     0,   757,     7,     7,   759,   754,   756,   839,
     778,     7,   744,   425,   250,   480,   481,   479,   498,    18,
       0,     0,   496,    21,   487,   488,   489,   490,   493,   491,
     486,     0,     0,    51,     0,   612,   854,     0,   607,     0,
     846,   849,   850,   847,   848,     0,   852,   851,     0,   570,
     579,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   614,     0,     0,     0,     0,     0,     0,   559,   560,
     558,     0,     0,     0,   629,   652,   657,   656,   651,     0,
      10,     0,   683,     0,   682,   725,   726,   728,   730,   732,
     735,     7,   465,   467,   740,   847,   867,   848,   869,   866,
     865,   870,   863,   864,   862,   871,   872,   873,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     823,   822,   839,   925,  1006,   784,   783,    60,     0,    61,
       0,     0,   105,     0,     0,     0,     0,     0,    58,   225,
     242,   258,   285,   304,     0,     0,     0,    13,    34,    42,
      49,   415,   404,   406,   409,   412,   304,    12,   184,   181,
      12,     0,   718,   712,   709,    50,   720,   721,  1046,  1048,
    1047,  1049,    53,  1070,  1072,  1071,  1073,  1052,    26,     0,
    1172,  1144,  1169,  1146,  1173,  1149,  1170,  1171,  1147,  1167,
    1168,  1145,  1148,  1209,  1208,  1210,  1211,  1217,  1199,  1200,
    1201,  1202,  1214,  1203,  1204,  1205,  1206,  1207,  1215,  1216,
    1218,  1219,  1220,  1221,  1222,  1223,    53,  1198,    21,  1213,
      47,   956,     0,     0,    26,    26,    27,    27,   962,   963,
     935,   935,     0,    24,   940,   944,   945,    32,     0,   339,
     344,   334,     0,     0,    94,     0,     0,   101,     0,     0,
      96,     0,   103,   549,     0,     0,   548,   670,     0,     0,
     764,   668,   760,  1160,  1164,  1159,  1163,  1165,    51,    10,
      10,     0,   753,     0,   751,    35,    35,    12,   472,    12,
      12,     0,    12,   505,     0,   518,   521,     0,   514,   510,
     509,   511,     0,   644,     0,     0,     0,   748,     0,   749,
       0,    13,     0,   758,   767,     0,     0,   777,   755,   765,
     747,   746,     0,   497,    26,   501,   502,    51,   500,   492,
     534,     0,   538,   535,     0,   540,     0,     0,   542,   613,
       0,   617,   619,   582,   584,   585,   587,   589,   591,   599,
     600,   594,   597,   596,   595,   609,   608,     0,     0,     0,
     998,   999,  1000,  1001,   623,   639,   630,   632,   664,   692,
       0,     0,     0,     0,   468,   874,   825,   819,     0,   830,
     831,   833,   835,   837,   824,   736,   818,   737,   841,   842,
       0,     0,   736,     0,     0,    58,   361,   360,   363,   362,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    84,   227,   226,     0,   224,     0,    53,     0,    86,
     244,   243,     0,     0,     0,     0,     0,     0,   268,     0,
       0,     0,     0,     0,     0,     0,     0,    89,   260,   259,
       0,     0,     0,   433,     0,     0,     0,     0,     0,    90,
     287,   286,     0,     0,     0,     0,     0,     0,    13,    92,
     306,   305,   124,     0,     0,     0,     0,     0,   339,   344,
     334,   117,   122,   118,   123,     0,   145,   182,     0,   931,
    1069,  1051,     0,  1196,  1197,  1225,  1250,  1257,   990,   991,
     992,   993,    39,     0,    24,   936,   952,   948,   947,   946,
      32,     0,     0,     0,     0,    13,   326,   325,   328,   327,
     518,   521,    34,   351,   350,   353,   352,    42,   331,   330,
     333,   332,   511,    49,   356,   355,   358,   357,   550,   552,
       0,   394,   762,   763,   761,  1133,   929,   928,     0,     0,
     752,   926,   924,  1003,   473,  1004,    12,  1002,     0,   523,
     525,     0,    34,     0,    34,     0,     0,    42,   658,   654,
     655,     0,     0,   635,     0,     0,     0,     7,   779,   780,
       0,     0,   769,   517,   768,   775,   776,   766,   462,   251,
       0,   495,     0,   494,    53,    53,    45,    53,   610,     0,
     616,     0,   618,   624,   633,     0,   671,     0,     0,     0,
       7,   466,   821,   820,   566,     0,   106,   416,   323,   407,
     241,   410,   257,   279,   413,   303,    21,     0,     0,     0,
     323,   323,   323,   323,   228,   388,   389,    41,   387,   386,
       0,    21,   245,   247,   246,   249,   248,     0,   255,     0,
       0,     0,   302,   301,    41,   300,   364,   366,   367,   365,
     320,   368,   321,     0,   319,   275,   276,   278,   277,     0,
     274,   269,   270,   266,   434,     0,     0,     0,     0,     0,
     298,   297,   295,     0,     0,     0,   309,   109,   111,   112,
     113,   115,     0,     0,     0,   142,    10,     0,   939,   935,
     951,   949,     0,     0,     0,    12,   341,   340,   343,   342,
       0,     0,     0,     0,     0,    12,   346,   345,   348,   347,
       0,     0,    12,   336,   335,   338,   337,   785,    95,   813,
     814,   102,    97,   787,   104,   551,     0,   684,  1005,   527,
     528,   529,   530,   531,   520,     0,   503,     0,   522,   504,
     524,     0,   513,    43,    43,   637,   750,    12,     0,     0,
       0,   252,   499,    29,    29,   543,   541,   611,     0,     0,
       0,   672,   678,     0,   470,   469,   567,   568,     0,     0,
       0,     0,     0,     0,   223,   323,   323,   323,     0,     0,
       0,     0,   385,     0,   384,     0,   262,   264,   265,   267,
     299,     0,    53,     0,    21,     0,   288,   296,     0,    21,
       0,     0,     0,     0,   312,    21,    12,    12,    12,  1025,
       0,    24,     0,    55,    51,     0,     0,    99,     0,     0,
       0,     0,     0,   100,     0,     0,    98,     0,   519,   526,
     512,   645,   646,   773,    12,     0,   518,   521,   537,   539,
     615,   676,     0,     7,   569,   563,   566,   359,   242,   258,
     285,   304,     0,     0,     0,   324,   349,   329,   354,     0,
     256,   322,   261,   273,   263,     0,     0,     0,   253,   284,
       0,    13,    34,    42,    49,     0,     0,   294,     0,   378,
     372,   371,   375,   370,   373,   374,   307,   317,   316,     0,
     313,   318,   308,   310,   120,   121,   119,     0,   950,     0,
     798,   797,   804,   806,   809,   810,   807,   808,   811,     0,
     800,   685,   774,    13,    34,    34,   677,   471,   568,     0,
       0,     0,     0,     0,   339,   344,   334,     0,     0,     0,
       0,   383,     0,   291,     0,     0,   280,   282,   281,   283,
      53,    53,   379,     0,     0,     7,   311,  1132,     0,     0,
     801,     0,   770,   771,   772,   564,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   292,   254,    53,   293,   289,   377,   376,   315,   314,
       0,     0,     0,    53,    58,   229,   230,   231,   232,    12,
      12,    12,    13,    34,    42,    49,   380,   381,     0,   369,
     290,   803,   802,    41,   799,   240,     0,     0,     0,   233,
     238,   234,   239,   382,   805,   236,   237,   235
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1710, -1710,    -1, -1141,   908,    40, -1209,   910,  -623,  -323,
     904,  1118, -1710,   920,  -453, -1710, -1710,  -510,  1124, -1710,
   -1543,  1127,   -22,   -12,  1311,  -557, -1710, -1710,  -610, -1710,
    -402,  -634,  1133, -1710,  -258, -1710,   815, -1709,  -353, -1150,
   -1710,  -768,  -479,  -841, -1710,  -462,   482,  -882, -1710,  -414,
    1343,  -886,   937, -1710,  -403,    16, -1710, -1710,  1829, -1710,
   -1710, -1710, -1710, -1710, -1710,  1476, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
      18, -1710,  1465, -1710,   514,  -314, -1275, -1710, -1710, -1522,
    -393, -1274,  -380,   224,  -126,  -384, -1710, -1273, -1280, -1710,
    -381, -1284,  -321, -1710,  -270,  -113, -1534,  -752,  -176,  -173,
   -1575, -1568, -1563,  -171,  -163,  -145, -1710, -1710,  -266, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710,   148,  -731, -1324,
    1807,   -18,  3179, -1710,   671, -1710, -1710,   317, -1710,   304,
     744, -1710, -1710,  2929, -1710,  -717,  1509, -1710, -1710,   207,
    1804,  1111,  2418,   -34, -1710, -1710, -1288, -1259,  -300, -1710,
   -1710, -1710, -1710, -1710, -1710,   118, -1710, -1710, -1710, -1710,
    1122, -1710, -1710, -1710,  1107, -1710, -1710, -1710,   140, -1710,
    1605, -1355,   202, -1710, -1097, -1535, -1367, -1518, -1357,   172,
     175, -1710, -1710,  -927, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710,  1058,  -307,  1527,  -117,  -185,  -184,   673,   676,   672,
   -1710,  -706, -1710, -1710, -1710, -1710, -1710, -1710,  1916, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,  -287,   657,
   -1710, -1710, -1710, -1710,  1052,   443,  -837,   185,  1163,   660,
   -1220, -1710, -1710,  1651, -1710, -1710, -1710, -1710,  1055, -1710,
   -1710,   -58,   -29,  -776,  -312,   987,   -26, -1710, -1710, -1710,
     931,   -17, -1710, -1710, -1710, -1710, -1710,  -155,  -202, -1710,
   -1710,   629,  -742,  1745,   -57, -1710,   742, -1190, -1710, -1446,
   -1710, -1710,   477, -1307, -1710, -1710,   449,   453, -1710, -1710,
    1585,  -573,  1567,  -552,  1557,  -543,  1571,  1853, -1710, -1673,
   -1710, -1710,  -196, -1710, -1710,  -348,  -524,  1578, -1710,  -339,
    -285,  -771,  -769,  -762, -1710,  -334,  -751, -1710,  1466,  1335,
    -772, -1710, -1399,  -288,   216,  1677, -1710,   -30, -1710,   284,
   -1710, -1321, -1710,   505, -1710, -1710, -1710, -1710, -1710,   750,
    -236,   768,  1349,   927,  1684,  1687, -1710, -1710,  -511,   104,
   -1710, -1710, -1710,  1003, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,  1384,  -992,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
     916, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710, -1710,
   -1710, -1710, -1710, -1710, -1710, -1710
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,  1650,   779,  1049,  1242,  1379,  1052,  1132,   739,
    1077,  1114,   873,  1050,  1240,   726,  1364,  1051,  1113,   874,
    1686,  1112,  1295,  1297,  1241,  1043,  1045,   724,   717,   480,
     987,  1117,  1116,   727,   803,  1878,  1366,  1972,   988,   875,
     800,   487,  1104,  1098,  1420,   989,  1022,   721,   853,  1027,
    1014,  1053,  1054,   791,   854,  1651,    45,    46,    47,    72,
      85,    87,   414,   418,   423,   410,   101,   103,   130,   993,
     442,   135,   143,   145,   260,   263,   266,   267,   735,  1144,
     261,   211,   382,  1604,  1333,   383,  1334,  1512,  2056,  1827,
     386,  1335,   387,  1849,  1850,   390,  2065,  1336,  1630,  1834,
     392,  1337,  1993,  2079,  2080,  1842,  1843,  1959,  1443,  1448,
    1693,  1691,  1692,  1446,  1451,  1331,  1844,  1612,  1991,  2072,
    2073,  2074,  2160,  1613,  1614,  1817,  1818,  1797,   212,  1722,
      48,    49,    59,   417,    51,   421,  1800,    65,   426,  1802,
      69,   431,  1805,   412,   413,  1798,   285,   286,   287,    52,
     394,  1460,   444,  1616,   320,   321,  1632,   322,   323,   324,
     325,   326,   327,  1292,  1563,  1564,   328,   329,   330,   789,
     790,   331,   332,   798,   799,  1229,  1223,  1517,  1518,   333,
    1156,  1491,  1747,   334,  1187,  1742,  1485,  1744,  1486,  1487,
    1924,   335,   336,  1522,   802,   337,   338,   339,   340,   341,
     859,   860,  1589,   381,  1957,  2035,   824,   825,   826,   827,
     828,   829,   830,  1542,   831,   832,   833,   834,   835,   836,
     342,   343,   866,   344,   345,   871,   346,   347,   867,   868,
     348,   349,   350,   879,   880,  1190,  1191,  1192,   881,   882,
    1167,  1168,   351,   352,   353,   354,   355,   888,   889,   356,
     357,   213,   837,   885,   924,   838,   358,   217,  1020,   506,
     507,   839,   514,   359,   360,   361,   362,   363,   364,   926,
     927,   928,   365,   366,   367,   780,   781,  1473,  1474,  1205,
    1206,  1207,  1461,  1462,  1508,  1503,  1504,  1509,  1208,  1757,
     946,  1699,   740,  1711,   742,  1717,   743,   435,   465,  2005,
    1906,  2139,  2140,  1889,  1899,  1146,  1706,   741,   368,   947,
     948,   932,   933,   934,   935,  1209,   937,   840,   841,   842,
     940,   941,   369,   754,   843,   675,   676,   220,   371,   844,
     512,  1433,   705,   845,  1142,   718,  1437,  1690,   223,   846,
     637,   848,   638,   849,   700,   701,  1129,  1130,   702,   850,
     851,   372,   373,   852,   228,   500,   229,   521,   230,   527,
     231,   535,   232,   549,  1040,  1378,   233,   557,   234,   562,
     235,   572,   236,   581,   237,   587,   238,   590,   239,   594,
     240,   599,   241,   604,   242,   375,   616,  1073,  1467,  1075,
    1391,  1383,  1388,  1381,  1385,   243,   620,  1096,  1419,  1402,
    1408,  1397,  1097,   244,   626,   245,   517,   246,   247,   651,
     248,   639,   249,   641,   250,   643,   251,   648,   252,   658,
     253,   661,   254,   667,   255,   714
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      43,   214,    53,   706,   216,   964,   786,   977,   388,   389,
     785,   755,  1159,   218,   269,  1006,   966,    44,   319,   965,
     967,   784,   370,   214,  1048,   270,  1102,   930,  1210,   384,
    1302,  1196,   936,   370,   214,   218,  1304,  1236,   991,  1275,
     886,  1119,  1493,   949,   949,   279,   218,   982,  1644,   984,
     264,  1213,   748,   376,   719,  1445,  1642,   379,   872,  1657,
     990,  1560,  1224,  1497,  1499,  1654,  1228,  1655,  1028,  1656,
    1510,  1559,  1665,  1004,   953,   268,  1633,  1633,  1700,  1306,
     777,   931,   971,   963,   861,  1874,   869,   443,  1701,   443,
    1872,   443,   443,  1561,  1712,   443,  1873,   443,  1055,  1759,
     443,   443,   847,  1858,   443,   443,   443,   443,  1029,  1682,
    1684,   443,  1855,   986,   443,   443,  1789,  1248,  1285,  1286,
    1293,  1287,   443,  1005,  1288,  1980,  1119,   778,   778,  1211,
     883,   883,  1289,  1011,  1109,  1723,  1042,  -693,  1790,  2135,
     883,  1880,  2171,   999,  1044,  -693,  -693,  -693,  -693,   677,
    1562,   887,   951,   256,  1724,    56,    57,    58,  1008,  1047,
    1105,   150,  1107,  -688,  1108,  1909,  1173,  1151,  1785,  1111,
    -688,  -688,  -688,  -688,  2068,    55,   214,  -654,  1468,  1178,
     214,  1765,  1766,  1910,  1738,  1751,  1489,   262,   218,  1131,
    1157,   628,   218,  1815,  1816,   630,   280,  1024,   281,  1937,
    1160,  2077,  2078,  1076,  1262,  1017,  1263,   380,  -786,  -688,
    1118,   773,   385,  1099,   282,   631,   391,  1154,   649,  2010,
      63,   756,   151,   152,   153,  1435,  2077,  2078,  2019,   617,
    1136,  1149,  1316,  1432,   318,   747,  1683,  1120,  1152,   210,
     753,   753,  -695,   650,  1159,  1317,   219,  1318,  1115,  -695,
    -695,  -695,  -695,  1181,  2069,  2070,  -655,   757,    54,     3,
    1726,  1727,     4,  1458,     5,    55,  1459,  1739,   219,   151,
     152,   153,  1787,   677,   722,  -786,   628,   862,  1505,   219,
     725,  1506,   618,   619,     6,     7,     8,     9,  -695,   774,
    1153,   370,   214,   409,   884,  1829,  1830,   776,  1150,  1645,
     631,  1633,  1646,     3,   218,  1525,     4,  1528,     5,   929,
    1416,  1856,   456,   457,   221,  -703,   770,   771,   634,   635,
     929,   929,  -703,  -703,  -703,  -703,   978,  1417,     6,     7,
       8,     9,    66,    67,    71,   591,   221,   923,   628,  1031,
      86,   388,   389,  1018,  1019,   592,   593,   221,   923,   923,
    1950,  1526,  1296,   659,  1032,  1298,  1264,  1025,   720,  1418,
     960,  -703,   631,   102,   370,   214,  1155,   144,  1644,   388,
     389,  1490,    55,  1854,  1359,   976,  1642,   218,   660,   370,
     214,  1179,   388,   389,  -953,  1854,  -953,  1672,   628,  1147,
    1148,  1457,   218,   398,   399,   400,  1633,  1633,   636,  1347,
     870,  1938,   508,   509,   510,   151,   152,   153,   630,   425,
     427,   428,   631,  1740,   588,  1436,  1265,   265,   210,   440,
    1647,   219,  1349,  1724,  1174,   219,  2141,  1752,   451,   452,
     589,   210,  1350,  1626,   458,  -704,   257,   258,   259,   443,
    1469,  1470,  -704,  -704,  -704,  -704,   752,   752,   305,  1348,
     758,     4,  1740,     5,  2066,   490,   491,   210,  2088,   494,
    1283,  1291,   318,  2057,   307,   461,   210,   210,   952,   308,
     257,   258,   259,  1158,   210,     3,   210,  1212,     4,  1326,
       5,  -704,   210,  1543,  2204,   493,  1791,  2136,  -693,   221,
    2172,  2104,   271,   221,     3,   283,   284,     4,   979,     5,
       6,     7,     8,     9,  2071,   769,  1428,  1429,  1789,  2105,
       4,   210,     5,  -688,   782,   863,   864,   865,  1786,     6,
       7,     8,     9,   770,   771,   210,  2125,  1041,  1854,   782,
     377,   634,   635,  2122,  1309,   222,  -953,   219,  1916,  1569,
    1570,  2153,  1571,  1648,    55,  1572,  2151,  1310,  1169,  1261,
    1332,  1540,  2152,  1573,  2033,  1478,  1231,   222,  2001,   380,
    1046,  1246,  1247,   280,   703,  1949,  2008,   281,   222,  1232,
     402,  1195,   385,  2026,   704,  1035,  2020,   391,  1368,  1675,
    1370,   432,  1890,  2027,   886,  1233,  1725,  1900,  1308,  1891,
     980,  -954,  -695,  -954,  1774,   981,  -953,  1775,  1807,   983,
    1777,   378,  1320,  1363,  1515,   221,  1100,  1322,  -222,  1516,
     219,   636,  1294,  1649,  1033,  1626,  1936,   511,   767,  1101,
    1344,  1345,  1346,   895,  1421,   219,  1892,   855,   856,  1034,
     305,  1258,   515,   516,   768,  1773,    55,  1357,   863,   864,
     865,    55,   782,  1013,  -241,    55,   307,     3,   513,  1954,
       4,   308,     5,   462,  1338,   434,  1321,  1713,  1309,     3,
    1893,   640,     4,  1307,     5,  -703,  1940,  1999,   642,   897,
     463,  1580,     6,     7,     8,     9,     3,  1259,   221,     4,
     272,     5,   273,    62,     6,     7,     8,     9,   708,  1494,
     895,   558,  1992,   221,   600,   723,   951,   210,  1452,   744,
    1951,     6,     7,     8,     9,   278,   896,   501,   502,     4,
     222,     5,   559,  1519,   222,  1193,  1645,   749,  2003,  1646,
    1159,   214,  1204,  2004,  1500,   560,   601,   602,  1985,   603,
    2032,  1975,  1234,   218,  1666,  1988,   897,  1667,  1981,  1501,
    1290,   745,   393,  -954,   214,  1901,   397,  1758,  -508,  1256,
     923,  1257,  1447,  1145,   662,  1502,   218,  2113,   403,  1210,
     404,   420,   393,  2110,  1894,  2111,  1513,  2112,   663,   433,
       4,   434,     5,   995,   857,   664,  1626,   858,  2133,   447,
     224,  2134,  1483,  2021,  2022,  -704,  1808,   792,  1809,  1243,
    1328,   305,  1318,   665,  1249,   291,   292,  1218,   225,   666,
     293,  1244,   224,  -954,  1450,  1583,  1316,   307,   489,   746,
    1012,  1210,   308,   224,   782,   495,   715,   716,  1276,  1328,
     225,  1318,   751,   401,  1644,   644,   222,  1687,  1444,  1902,
    1219,   225,  1642,   782,   766,  1488,   775,  1647,   884,   430,
     929,  1731,  1732,   503,   504,  1220,     3,   505,  1895,     4,
     778,     5,  1633,  1633,  1733,   886,  1734,  1735,   454,  1737,
     782,   923,   923,   923,   923,   923,   214,     4,   923,     5,
    1983,     6,     7,     8,     9,   561,   150,  1520,   218,   801,
    1810,  1811,  1812,  1813,   929,   892,  1645,  1521,  1664,  1646,
    1221,     3,  2107,   923,     4,   887,     5,   787,   788,   222,
     645,   646,   647,   893,     3,   131,  1438,     4,  1674,     5,
     132,   133,   923,   894,   222,  1934,     6,     7,     8,     9,
     954,   793,   794,   795,   796,   797,   134,   150,  1520,     6,
       7,     8,     9,   150,  1520,  1652,   955,     3,  1524,  1316,
       4,   942,     5,  1836,  1527,  1327,  1662,   886,   621,  1329,
     770,   771,  1581,   943,  1318,   224,   956,   226,  1670,   224,
    1648,   957,     6,     7,     8,     9,   219,   958,  1837,  1838,
     282,  1567,  1472,   225,  1222,  1566,  1568,   225,  1576,   226,
     622,   623,  1574,  1568,  1577,  1579,   782,  1557,  -786,   219,
     226,  -303,  1890,   992,  2168,  1015,  1663,  1372,   782,  1891,
    -795,  1707,  1673,  1900,   890,   891,   652,  1647,  1945,  1196,
    1369,   624,  1371,  1585,  1373,   627,  -796,   215,  1377,  1374,
    1375,   998,   653,  1016,  1661,  1890,  1021,   628,  1023,   629,
    1871,  1280,  1891,   227,   221,  1282,  1892,     3,  1106,   215,
       4,  1110,     5,   654,   625,  1185,  1186,   630,   595,   655,
     215,   631,   596,  1135,   632,   374,  1141,   221,  1968,  1969,
    1970,  1971,     6,     7,     8,     9,   374,  1534,  1535,  1892,
    1893,   224,     3,  1133,  1541,     4,  1134,     5,  1143,  1169,
    1881,  1472,   877,   878,   597,   598,   449,  1839,  1165,   225,
    1965,  1966,  1967,  1036,  1037,  1038,  1039,     6,     7,     8,
       9,    18,    19,  1893,  1170,   709,  1171,  1840,  1841,   884,
     573,   219,   574,  1918,   710,   711,  1172,  2090,   712,   713,
    1376,     3,  2091,     4,     4,     5,     5,  1975,  1189,  1882,
    1648,   575,   226,   656,   657,  1202,   226,   633,  1175,   576,
     577,  1176,   578,  1214,   224,   214,     6,     7,     8,     9,
      34,  1177,   579,   580,  1251,  1252,   582,   218,  1180,   224,
      35,  1901,   225,  1845,  1846,  1847,    36,  1848,  1182,  1883,
     634,   635,   583,  1729,  1894,  1311,  1312,   225,  1762,   221,
    1393,   584,  1796,  1215,  1394,  1395,  1396,   585,   586,   550,
      37,   551,   215,  1820,   496,  1253,   215,   497,   498,   499,
    1875,   884,  1544,  1210,   770,   771,     3,  1894,   227,     4,
     552,     5,   227,  2042,  2043,  2044,  1386,  1387,   553,   554,
     555,   556,  1249,  1249,  1249,  1249,  1249,  1216,  1249,  1249,
     782,     6,     7,     8,     9,  1389,  1390,  1550,  1550,  1424,
    1425,  1792,  1793,   782,  1225,  1998,  1568,  1568,   226,  1226,
     636,  1914,   214,  1227,   214,   222,  1426,  1427,  1997,   778,
    1498,  1788,  2007,  1239,   218,  1245,   218,  1783,  1254,  1784,
    1255,     4,  2013,     5,  1260,  1981,   782,  -998,  1197,  2016,
     923,   923,   923,   923,   923,   923,   923,   923,   923,   923,
     923,  2180,  1398,  1399,  1400,  1401,  1964,  -999,  1882,  1266,
    1911,   518,   519,   520,   370,   214,  1267,  1884,   215,  1688,
    1689,  1974,   150,  1520,  2023,  1832,  1833,   218,   257,   258,
     259,   226,  2158,  2159,   374,  1866,   876,   877,   878,  1584,
    1586,  1268,  1602,  1610,  1628,  1640,   226,  1269,  1883,  1271,
    1926,     3,  1929,  1272,     4,  1279,     5,  1587,  1281,  1603,
    1611,  1629,  1641,   925,  1912,  1314,  1671,  1315,  1149,  1313,
    1943,  1944,  1319,  1946,   925,   925,     6,     7,     8,     9,
    1552,  1553,  1908,  2084,  2085,  2086,   782,  1325,   951,     3,
    1309,   215,     4,  1330,     5,   951,  2028,  2029,  1339,  1159,
     219,  1885,  1340,  1341,  1932,  1149,   215,   374,  1342,  1152,
     222,  2102,  1678,  1679,     6,     7,     8,     9,  1343,  1961,
    1351,   782,   374,     3,  1680,  1681,     4,  1352,     5,  1962,
    1353,  1354,  1152,  1355,  1963,  1358,  1695,  2186,   151,   152,
     153,   783,  1361,  1356,  1749,   863,   864,   865,     6,     7,
       8,     9,  1696,   370,   214,  1703,  1365,  1708,  1380,  1382,
    1714,  1979,  1384,  1204,    18,    19,   218,  1392,   221,  1697,
     782,  1410,  1704,  1411,  1709,  1412,  1422,  1715,   755,   923,
    1413,  1423,  1414,   467,  2054,  1702,  1884,  1987,  1770,  2067,
    -508,   923,   370,   214,  1415,  2083,  1960,  1430,    18,    19,
    1431,  1432,  1771,  1434,  1439,   218,  1454,   219,  1440,   219,
     224,  1442,     3,    34,  1441,     4,   469,     5,  1449,  1471,
     895,  1769,  1479,    35,  1481,   151,   152,   153,   225,    36,
    1484,    18,    19,  1198,  1495,  1492,  1472,     6,     7,     8,
       9,   370,   214,   923,  1514,  1530,  1539,    34,  1547,  1533,
    1548,   225,  1549,    37,   218,   782,  1565,    35,  1819,  1195,
     219,   923,   923,    36,  1582,  -654,  2196,  2197,  2198,   728,
    1996,   729,  -655,   730,   731,   221,  1658,   221,  2052,  1659,
      34,   732,   733,   436,   437,   438,   439,    37,  1660,  1668,
      35,  1676,  1694,  1169,  1677,  1685,    36,  1720,   214,  1721,
    1741,  1828,  1743,   522,  1882,   523,   524,   525,   526,  1728,
     218,  1745,  2129,  1814,   734,   214,  1754,  1746,  1828,  1760,
      37,  1822,  1823,  1824,  1825,  1826,  2017,   218,   221,  1761,
    1772,  1776,  1763,     3,   370,   214,     4,  1781,     5,  1851,
    1852,  1853,  1779,  1316,  1883,  1795,  1794,   218,  1806,  1821,
    1835,  1860,  1861,  1862,  1831,   224,   762,  1863,     6,     7,
       8,     9,  1602,  1610,  1628,  1640,  1857,  1864,  1865,     3,
    2127,  1876,     4,   225,     5,  1917,  1877,  1879,  1907,  1603,
    1611,  1629,  1641,  -526,  1948,  1925,  1927,   226,  1931,   222,
     782,  1939,   770,   771,     6,     7,     8,     9,  1952,   219,
    1886,  1896,  1903,  1193,  1276,  2146,  1953,   753,   753,   214,
     226,   939,  2143,  2144,  1956,  1958,  1990,  1887,  1897,  1904,
    2128,   218,   939,   939,  1973,  2002,  2164,  2165,   782,  2000,
    2009,  2011,   214,  2202,  2014,  2018,  2024,  2025,   219,  2031,
    2034,  2037,  2087,  2156,   218,  1900,  2038,   215,  2126,  1799,
    1801,  1801,  1804,  1403,  1404,  1405,  1406,  1407,  1169,  2190,
    2039,  2040,   782,   227,  2041,  2045,   214,   221,  2046,  2194,
    1194,  1919,  1920,  1921,  1922,  1923,   925,  2157,   218,  1941,
    2047,  2195,  1884,   388,   389,  2048,   227,   219,  2049,  2089,
    2142,  2200,  2099,  2103,     3,  2101,   222,     4,   222,     5,
       3,  2106,  2121,     4,  2114,     5,   221,  2115,  2116,   388,
     389,  2124,  -255,  2132,  2137,  2166,  1994,  2138,  2167,     6,
       7,     8,     9,  2170,  -256,     6,     7,     8,     9,  2191,
    2188,  1299,   226,  1305,  1645,  1301,  1976,  1646,  1977,  1978,
    1139,  2201,   938,   219,  1300,  1138,   471,  1137,  1030,   222,
    1367,   782,  1982,   938,   938,   221,  1140,  1669,  1984,    70,
     219,    18,    19,  1653,  1303,  1986,  2179,  2053,  1989,  2199,
    1026,   962,  1859,  1277,  1995,  2169,  2006,  2203,  2051,  2117,
     219,  1886,  1896,  1903,  2119,  2118,  2012,   925,   925,   925,
     925,   925,   215,  2015,   925,  2120,   370,   214,  1887,  1897,
    1904,   473,  2109,  1901,  2189,   422,   415,  1803,   227,   218,
      34,   221,   959,   752,   752,  1230,    18,    19,  1955,   925,
      35,  1217,  1942,   772,  1913,  1928,    36,  1270,   221,  2108,
    1930,   214,   961,  2145,   224,   923,  1536,  1538,   925,  1555,
    1537,  1278,  2075,   218,  2081,  1748,  1933,  1199,   221,  1558,
      37,   750,   225,  1284,   219,  1647,   214,  1362,  1578,  2050,
     707,  1730,  1764,  1511,   478,    34,   214,  2036,   218,  2058,
    1152,  1767,   782,   950,   974,    35,    73,   219,   218,    18,
      19,    36,   972,   464,    74,  2192,    75,  2181,   222,   975,
     760,    76,    77,    78,   973,    79,  1103,   136,   764,   137,
    2076,   765,  2082,     0,   138,    37,   139,  1409,   782,  1074,
     140,   219,  1121,  1122,  2092,  2093,     0,  2094,  2095,  2096,
    2097,  2098,   221,  2155,  2100,     0,  1123,   222,    34,     0,
       0,     0,     0,     0,  1124,     0,   214,     0,    35,  1828,
       0,   224,     0,   224,    36,   221,     0,     0,   218,   141,
    1125,   142,     0,     0,    88,     0,     0,     0,     0,   225,
       0,   225,    89,     0,    90,  2123,    91,     0,    37,    92,
      93,    94,     0,    95,  2130,  2131,   222,     0,  1648,   221,
     295,   296,   297,  1755,  1756,   300,  1149,  2081,   528,   529,
     370,   214,     0,     0,   224,     0,     0,   530,   531,   532,
     533,   534,     0,   218,     0,   214,     0,     0,  2162,     0,
    2187,   226,   225,     0,     0,     0,     0,   218,  1586,  1610,
    1628,  1640,     0,     0,   939,     0,  1696,  1703,  1708,  1714,
       0,  2161,   222,     0,  2163,  1587,  1611,  1629,  1641,   782,
    1994,     0,   219,  1697,  1704,  1709,  1715,     0,  2150,   222,
    2173,     0,     0,     0,  2154,  2174,  2175,  2176,  2177,  2178,
    1886,  1896,  1903,  2182,  2183,  2184,  2185,     0,  1250,   222,
       0,   215,  1126,  1127,  1128,   686,   219,  1887,  1897,  1904,
       0,  2193,     0,     0,   482,     0,     0,   227,   696,   697,
     698,     0,     0,     0,     0,     0,     0,     0,     0,    18,
      19,   219,     0,     0,     0,  2205,  2206,  2207,     0,    80,
     221,   219,     0,     0,     0,     0,     0,     0,   226,     0,
     226,     0,     0,     0,    81,     0,  1056,     0,  1523,  1523,
       0,  1523,     0,   224,     0,   939,   939,   939,   939,   939,
      82,  1057,   939,   222,   221,    83,     0,    84,    34,     0,
       0,   225,     0,     0,     0,   938,     0,     0,    35,     0,
     484,     0,     0,     0,    36,     0,  1197,   939,     0,   221,
       0,   226,   224,     0,     0,    18,    19,     0,   215,   221,
     215,   219,     0,     0,     0,     0,   939,    96,    37,     0,
     225,     0,     0,     0,   227,     0,   227,     0,     0,     0,
     222,     0,    97,     0,     0,  1058,   925,   925,   925,   925,
     925,   925,   925,   925,   925,   925,   925,     0,    98,     0,
       0,   224,     0,    99,    34,   100,     0,     0,  1507,     0,
       0,   215,     3,  1463,    35,     4,   219,     5,     0,   225,
      36,     0,     0,     0,     0,     0,     0,   374,     0,   221,
     219,     0,     0,     0,     0,     0,     0,     6,     7,     8,
       9,     0,     0,     0,    37,     0,   938,   938,   938,   938,
     938,  1617,     0,   938,     0,     0,  1059,   224,     0,  1060,
    1061,  1062,  1063,  1064,  1065,  1066,  1067,  1068,  1069,  1070,
    1071,  1072,     0,     0,   224,   225,     0,  1058,   938,     0,
       0,     0,   605,     0,   221,     0,     0,     0,     0,     0,
     226,   606,   225,     0,   224,     0,     0,   938,   221,  1618,
    1619,  1620,  1621,     0,  1622,     0,   607,  1623,     0,     0,
     608,   222,   225,     0,   609,   610,     0,     0,     0,   611,
     612,   613,   614,   615,  1624,     0,     0,     0,     0,   226,
       0,     0,  1588,     0,  1605,  1615,  1631,  1643,     0,     0,
       0,     0,     0,     0,     0,   222,     0,     0,  1464,     0,
     215,  1060,  1061,  1465,  1063,  1064,  1065,  1066,  1067,  1068,
    1069,  1070,  1071,  1466,     0,     0,   374,     0,   224,  1750,
     222,     0,     0,  1625,     0,   925,     0,     0,   226,     0,
     222,     0,     0,     0,     0,     0,   225,   925,     0,   215,
       0,  1198,     0,     0,     0,     0,     0,     0,  1523,     0,
       0,  1523,     0,     0,  1523,   374,     0,     0,     0,   225,
       0,     0,     0,     0,     0,   446,     0,   448,   450,     0,
       0,   453,     0,   455,     0,   224,   459,   460,     0,     0,
     468,   470,   472,   474,   226,     0,     0,   479,   215,   925,
     483,   485,     0,   225,  1698,   563,   564,  1705,   492,  1710,
     222,   226,  1716,     0,   374,     0,     0,   925,   925,     0,
     565,     0,   566,   567,   568,     0,     0,     0,     0,     0,
       0,   226,     0,     0,     0,     0,  1250,  1250,  1250,  1250,
    1250,  1626,  1250,  1250,     0,   569,   570,   571,     0,     0,
       0,  1551,  1551,     0,   215,     0,   305,     0,     0,     0,
       0,     0,     0,     0,     0,   222,    10,     0,     0,  1507,
     227,   215,   307,     0,    11,     0,    12,   308,    13,   222,
       0,    14,    15,    16,     0,    17,     0,   227,     0,    18,
      19,   215,     0,     0,   939,   939,   939,   939,   939,   939,
     939,   939,   939,   939,   939,   226,     0,   374,  2059,     0,
       0,   782,     0,  1164,     0,  1166,   224,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1627,     0,   226,     0,
       0,     0,  1183,  1184,   225,  1188,     0,     0,    34,     0,
       0,  1200,  1201,     0,     0,     0,     0,     0,    35,     0,
     224,     0,     0,     0,    36,     0,     0,  2060,     0,     0,
       0,     0,   226,     0,     0,   215,     0,     0,   225,     0,
    1235,     0,  1238,     0,  1836,   224,     0,     0,    37,     0,
       0,   227,     0,     0,     0,   224,     0,     0,  1194,  1277,
       0,     0,     0,   225,     0,     0,     0,     0,     0,  1837,
    1838,     0,     0,   225,   227,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1605,  1615,  1631,  1643,     0,     0,
       0,     0,   215,     0,     0,   938,   938,   938,   938,   938,
     938,   938,   938,   938,   938,   938,  1274,   805,   227,     0,
       0,   536,   537,     0,     0,   538,     0,     0,     0,     0,
       0,     0,  1888,  1898,  1905,   224,   539,   540,   541,   542,
     543,   544,   545,   546,   547,     0,     0,     0,     0,     0,
     151,   152,   153,   225,     0,     0,     0,   905,     0,     0,
       0,   906,     0,   939,     0,     0,   907,     0,     0,     0,
       0,     0,   908,   226,     0,   939,   812,     0,     0,    38,
       0,     0,     0,     0,     0,   548,     0,     0,  1323,  1324,
     224,     0,     0,     0,    39,     0,   911,     0,  1839,     0,
       0,     0,     0,     0,   224,   996,     0,   226,   225,     0,
      40,     0,     0,     0,     0,    41,     0,    42,  1840,  1841,
       0,     0,   225,     0,     0,     0,     0,   939,     0,     0,
       0,     0,   226,   215,     0,     0,  2061,     0,     0,     0,
       0,     0,   226,     0,     0,   939,   939,     0,     0,   374,
       0,     0,  2062,     0,     0,     0,     0,  2063,     0,  2064,
       0,     0,     0,     0,     0,     0,     0,   215,     0,    60,
       0,   925,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   227,     0,     0,     0,     0,     0,     0,
       0,     0,   215,     0,   938,     0,     0,     0,   274,   275,
     276,   277,   215,     0,     0,     0,   938,     0,   227,     0,
       0,     0,   226,  1888,  1898,  1905,     0,     0,   227,     0,
       0,     0,     0,     0,     0,     0,     0,   815,     0,     0,
     816,   817,   395,     0,     0,     0,     0,     0,     0,     0,
     405,   406,   407,   408,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   938,     0,
       0,     0,     0,   441,     0,   819,   820,   226,     0,     0,
       0,     0,   215,     0,     0,     0,   938,   938,     0,     0,
     466,   226,     0,     0,     0,   475,   476,   477,   227,     0,
     481,     0,     0,   486,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   210,  1453,     0,     3,
       0,  1456,     4,     0,     5,     0,     0,     0,     0,     0,
       0,  1475,  1476,  1477,     0,     0,  1480,   215,  1482,     0,
       0,     0,     0,     0,     6,     7,     8,     9,     0,     0,
       0,   215,     0,   374,     0,  1590,     0,     0,  1496,     0,
       0,     0,     0,     3,     0,  1591,     4,   227,     5,     0,
    1592,     0,  1593,     0,  1594,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     6,     7,
       8,     9,     0,  1529,     0,  1531,     0,     0,     0,     0,
       0,  1532,  1617,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1545,     0,  1546,     0,
      50,     0,     0,     0,     0,     0,     0,  1554,     0,  1556,
      61,    50,    50,    64,    64,    64,    68,     0,     0,    50,
     736,   737,   738,     0,     0,     0,     0,     0,     0,     0,
    1618,  1619,  1620,  1621,     0,  1622,     0,     0,  1623,     0,
    1588,  1615,  1631,  1643,     0,     0,     0,     0,  1698,  1705,
    1710,  1716,     0,     0,     0,  1624,     0,     0,     0,     0,
    1595,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      50,     0,     0,   396,    50,    64,    64,    64,    68,     0,
       0,     0,  1888,  1898,  1905,    50,     0,   411,   416,   419,
      50,   424,    64,    64,   429,     0,   411,   411,   411,   411,
    1078,    64,     0,     0,  1625,   445,     0,    50,     0,   939,
      64,    64,     0,    68,     0,     0,    64,     0,  1079,  1080,
    1081,  1082,  1083,  1084,  1085,  1086,  1087,  1088,  1089,  1090,
    1091,  1092,  1093,  1094,  1095,   488,    50,    64,    64,     0,
       0,    64,     0,    50,     0,     0,     0,     0,     0,     0,
       0,   968,   969,   970,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1596,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1597,   985,     0,     0,     0,     0,     0,
    1718,  1719,     0,     0,     0,     0,     0,     0,     0,  1598,
       0,     0,     0,     0,  1599,     0,  1600,     0,     0,     0,
       0,     0,  1626,   997,     0,     0,     0,  1736,     0,     3,
       0,     0,     4,     0,     5,     0,     0,   305,     0,     0,
       0,  1753,     0,     0,     0,     0,     0,     0,   782,     0,
     938,     0,     0,   307,     6,     7,     8,     9,   308,     0,
       0,     0,     0,  1601,     0,     0,     0,     0,  1617,     0,
       0,     0,     0,     0,     0,     0,  1778,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   782,  1780,     0,  1782,     0,     0,     0,     0,
       3,     0,     0,     4,     0,     5,     0,  1869,     0,     0,
       0,     0,     0,     0,     0,     0,  1618,  1619,  1620,  1621,
       0,  1622,   146,     0,  1623,     6,     7,     8,     9,   147,
     148,     0,     0,     0,   288,   149,   289,     0,     0,     0,
       0,  1624,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   290,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   291,   292,     0,     0,     0,   293,     0,     0,   294,
       0,     0,     0,     0,     0,     0,     0,   295,   296,   297,
     298,   299,   300,     0,     0,     0,     0,     0,     0,     0,
    1625,     0,  1606,     0,     0,     0,   301,     0,   302,     0,
       0,   150,   151,   152,   153,     0,     0,   154,     0,   155,
       0,     0,  1607,   156,     0,     0,     0,     0,   157,     0,
       0,     0,     0,     0,   158,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     159,     0,     0,     0,     0,   160,     0,     0,   161,     0,
       0,     0,     0,   162,     0,     0,   163,     0,   994,   164,
     165,  1608,     0,     0,   166,    64,  1915,   167,     0,   168,
       0,     0,     0,     0,     0,     0,  1000,     0,  1001,     0,
    1002,     0,  1003,     3,     0,     0,     4,  1007,     5,     0,
       0,  1009,   761,  1010,   169,   170,     0,     0,  1626,     0,
    1935,     0,    64,     0,     0,     0,     0,     0,     6,     7,
       8,     9,     0,   305,   678,   679,     0,     0,   680,     0,
       0,     0,  1634,     0,     0,  1947,     0,     0,     0,   307,
     171,   172,   173,   174,   308,     0,   681,     0,     0,   175,
     176,     0,     0,   177,   178,   303,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,     0,   782,   304,
       0,  1635,     0,  1636,     0,  1622,     0,     0,  1623,     0,
       0,     0,     0,  2148,   305,     0,     0,     0,     0,     0,
       0,     0,   762,     0,   306,  1637,     0,     0,     0,     0,
     307,     0,     0,     0,     0,   308,     0,     0,     0,     0,
       0,   202,   203,     0,   309,     0,     0,   310,   311,   312,
     313,     0,     0,     0,   314,     0,     0,     0,   315,   316,
     204,     0,     0,     0,   205,   206,     0,     3,     0,   782,
       4,     0,     5,   317,  1638,     0,   207,   208,     0,     0,
     682,     0,     0,   318,  1609,   209,     0,     0,   210,   146,
       0,     0,     6,     7,     8,     9,   147,   148,     0,     0,
       0,   288,   149,   289,   683,   684,   685,   686,   687,   688,
     763,   689,   690,   691,   692,   693,   694,   695,   290,     0,
     696,   697,   698,     0,  2030,     0,     0,     0,   291,   292,
       0,     0,     0,   293,     0,     0,   294,     0,     0,     0,
       0,   699,     0,     0,   295,   296,   297,   298,   299,   300,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1606,
       0,     0,     0,   301,     0,   302,     0,     0,   150,   151,
     152,   153,     0,     0,   154,     0,   155,     0,     0,  1607,
     156,     0,  1626,     0,     0,   157,     0,     0,     0,     0,
       0,   158,     0,     0,     0,     0,     0,   305,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   159,     0,     0,
       0,     0,   160,   307,     0,   161,     0,     0,   308,     0,
     162,     0,     0,   163,     0,     0,   164,   165,  1608,     0,
       0,   166,     0,     0,   167,     0,   168,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       3,     0,   782,     4,     0,     5,     0,     0,     0,     0,
       0,   169,   170,     0,     0,     0,     0,  1639,     0,     0,
       0,     0,     0,     0,     0,     6,     7,     8,     9,     0,
       0,   678,   679,     0,     0,   680,     0,     0,     0,  1634,
       0,     0,     0,     0,     0,     0,     0,   171,   172,   173,
     174,     0,     0,   681,     0,     0,   175,   176,     0,     0,
     177,   178,   303,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,     0,     0,   304,     0,  1635,     0,
    1636,     0,  1622,     0,     0,  1623,     0,     0,     0,     0,
       0,   305,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   306,  1637,     0,     0,     0,     0,   307,     0,     0,
       0,     0,   308,     0,     0,     0,     0,     0,   202,   203,
       0,   309,     0,     0,   310,   311,   312,   313,     0,     0,
       0,   314,     0,     0,     0,   315,   316,   204,     0,     0,
       0,   205,   206,     0,     3,     0,   782,     4,     0,     5,
     317,  1638,     0,   207,   208,     0,     0,   682,     0,     0,
     318,  1868,   209,     0,     0,   210,   146,     0,     0,     6,
       7,     8,     9,   147,   148,     0,     0,  1360,   288,   149,
     289,   683,   684,   685,   686,   687,   688,     0,   689,   690,
     691,   692,   693,   694,   695,   290,     0,   696,   697,   698,
       0,     0,     0,     0,     0,   291,   292,     0,     0,     0,
     293,     0,     0,   294,     0,     0,     0,     0,   699,     0,
       0,   295,   296,   297,   298,   299,   300,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1606,     0,     0,     0,
     301,     0,   302,     0,     0,   150,   151,   152,   153,     0,
       0,   154,     0,   155,     0,     0,  1607,   156,     0,  1626,
       0,     0,   157,     0,     0,     0,     0,     0,   158,     0,
       0,     0,     0,     0,   305,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   159,     0,     0,     0,     0,   160,
     307,     0,   161,     0,     0,   308,     0,   162,     0,     0,
     163,     0,     0,   164,   165,  1608,     0,     0,   166,     0,
       0,   167,     0,   168,     0,     3,     0,     0,     4,     0,
       5,     0,     0,     0,     0,     0,     0,     0,     0,   782,
       0,     0,     0,     0,     0,     0,     0,     0,   169,   170,
       6,     7,     8,     9,  1870,     0,     0,     0,     0,     0,
       0,  1590,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1591,     0,     0,     0,     0,  1592,     0,  1593,     0,
    1594,     0,     0,     0,   171,   172,   173,   174,     0,     0,
       0,     0,     0,   175,   176,     0,     0,   177,   178,   303,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,     0,     0,   304,     0,     3,     0,     0,     4,     0,
       5,     0,     0,     0,     0,     0,     0,     0,   305,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   306,     0,
       6,     7,     8,     9,   307,     0,     0,     0,     0,   308,
       0,     0,     0,     0,  1634,   202,   203,     0,   309,     0,
       0,   310,   311,   312,   313,     0,     0,     0,   314,     0,
       0,     0,   315,   316,   204,     0,  1595,     0,   205,   206,
       0,     0,     4,   782,     5,     0,     0,   317,     0,     0,
     207,   208,     0,     0,     0,     0,     0,   318,  2147,   209,
       0,   146,   210,  1635,     0,  1636,     0,  1622,   147,   148,
    1623,     0,     0,   288,   149,   289,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1637,     0,     0,
     290,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     291,   292,     0,     0,     0,   293,     0,     0,   294,     0,
       0,     0,     0,     0,     0,     0,   295,   296,   297,   298,
     299,   300,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   301,  1638,   302,     0,     0,
     150,   151,   152,   153,  1596,     0,   154,     0,   155,     0,
       0,     0,   156,     0,     0,     0,     0,   157,     0,  1597,
       0,     0,     0,   158,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1598,     0,     0,     0,   159,
    1599,     0,  1600,     0,   160,     0,     0,   161,     0,     0,
       0,     0,   162,     0,     0,   163,     0,     0,   164,   165,
       0,     0,     0,   166,     0,     0,   167,     0,   168,     0,
       0,     0,     0,     0,   782,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1867,
       0,     0,     0,   169,   170,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1626,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   305,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   171,
     172,   173,   174,     0,     0,   307,     0,     0,   175,   176,
     308,     0,   177,   178,   303,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,     0,     0,   304,     0,
     804,     0,     0,     0,   782,     0,     0,     0,     0,     0,
       0,     0,     0,   305,     0,     0,     0,     0,   146,  2149,
       0,     0,     0,   306,     0,     0,   148,     0,     0,   307,
       0,   149,     0,     0,   308,     0,     0,     0,     0,     0,
     202,   203,     0,   309,     0,     0,   310,   311,   312,   313,
       0,     0,     0,   314,     0,     0,     0,   315,   316,   204,
       0,     0,     0,   205,   206,   805,     0,     0,     0,     0,
       0,     0,   317,     0,     0,   207,   208,     0,     0,   806,
       0,   807,   318,  1768,   209,     0,     0,   210,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   150,   151,   152,
     153,     0,     0,   154,     0,   808,     0,     0,     0,   809,
       0,     0,     0,     0,   810,     0,     0,     0,     0,     0,
     811,     0,     0,     0,   812,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   159,     0,     0,     0,
       0,   160,     0,     0,   813,     0,     0,     0,     0,   162,
       0,     0,   163,     0,     0,   164,   165,     0,     0,     0,
     166,     0,     0,   167,     0,   168,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     169,   170,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1161,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   146,
       0,     0,     0,     0,     0,     0,   814,   148,     0,     0,
       0,     0,   149,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,     0,     0,   201,   805,     0,     0,     0,
       0,     0,     0,     0,     0,   815,     0,     0,   816,   817,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   818,     0,     0,     0,     0,     0,     0,   150,   151,
     152,   153,     0,     0,   154,     0,   808,   202,   203,     0,
     809,     0,     0,   819,   820,   810,     0,     0,     0,     0,
       0,  1162,     0,     0,     0,   812,   204,     0,     0,     0,
     205,   206,     0,     0,     0,     0,     0,   159,     0,     0,
     821,   822,   160,   674,     0,  1163,     0,     0,     0,     0,
     162,   823,     0,   163,   210,     0,   164,   165,     0,     0,
       0,   166,     0,     0,   167,     0,   168,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   169,   170,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1237,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     146,     0,     0,     0,     0,     0,     0,   814,   148,     0,
       0,     0,     0,   149,     0,     0,     0,     0,     0,     0,
       0,     0,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,     0,     0,   201,   805,     0,     0,
       0,     0,     0,     0,     0,     0,   815,     0,     0,   816,
     817,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   150,
     151,   152,   153,     0,     0,   154,     0,   808,   202,   203,
       0,   809,     0,     0,   819,   820,   810,     0,     0,     0,
       0,     0,  1162,     0,     0,     0,   812,   204,     0,     0,
       0,   205,   206,     0,     0,     0,     0,     0,   159,     0,
       0,   821,   822,   160,   674,     0,  1163,     0,     0,     0,
       0,   162,   823,     0,   163,   210,     0,   164,   165,     0,
       0,     0,   166,     0,     0,   167,     0,   168,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   169,   170,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1273,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   146,     0,     0,     0,     0,     0,     0,   814,   148,
       0,     0,     0,     0,   149,     0,     0,     0,     0,     0,
       0,     0,     0,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,     0,     0,   201,   805,     0,
       0,     0,     0,     0,     0,     0,     0,   815,     0,     0,
     816,   817,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     150,   151,   152,   153,     0,     0,   154,     0,   808,   202,
     203,     0,   809,     0,     0,   819,   820,   810,     0,     0,
       0,     0,     0,  1162,     0,     0,     0,   812,   204,     0,
       0,     0,   205,   206,     0,     0,     0,     0,     0,   159,
       0,     0,   821,   822,   160,   674,     0,  1163,     0,     0,
       0,     0,   162,   823,     0,   163,   210,     0,   164,   165,
       0,     0,     0,   166,     0,     0,   167,     0,   168,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   169,   170,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1455,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   146,     0,     0,     0,     0,     0,     0,   814,
     148,     0,     0,     0,     0,   149,     0,     0,     0,     0,
       0,     0,     0,     0,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,     0,     0,   201,   805,
       0,     0,     0,     0,     0,     0,     0,     0,   815,     0,
       0,   816,   817,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   150,   151,   152,   153,     0,     0,   154,     0,   808,
     202,   203,     0,   809,     0,     0,   819,   820,   810,     0,
       0,     0,     0,     0,  1162,     0,     0,     0,   812,   204,
       0,     0,     0,   205,   206,     0,     0,     0,     0,     0,
     159,     0,     0,   821,   822,   160,   674,     0,  1163,     0,
       0,     0,     0,   162,   823,     0,   163,   210,     0,   164,
     165,     0,     0,     0,   166,     0,     0,   167,     0,   168,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   169,   170,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   898,
     899,   900,   901,   902,   903,     0,     0,     0,     0,   904,
       0,     0,     0,   883,     0,     0,     0,     0,     0,     0,
     814,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,     0,     0,   201,
     805,     0,     0,     0,     0,     0,     0,     0,     0,   815,
       0,     0,   816,   817,     0,     0,     0,     0,     0,     0,
    1203,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   150,   151,   152,   153,     0,     0,     0,     0,
     905,   202,   203,     0,   906,     0,     0,   819,   820,   907,
       0,     0,     0,     0,     0,   908,     0,     0,     0,   812,
     204,     0,     0,     0,   205,   206,     0,     0,  1203,   805,
       0,   909,     0,     0,   821,   822,   910,   674,     0,   911,
       0,     0,     0,     0,   912,   823,     0,   913,   210,     0,
     914,   915,     0,     0,     0,   916,     0,     0,   917,     0,
     918,   150,   151,   152,   153,     0,     0,     0,     0,   905,
       0,     0,     0,   906,     0,     0,     0,   805,   907,     0,
       0,     0,     0,     0,   908,   919,   920,     0,   812,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     909,     0,     0,     0,     0,   910,     0,     0,   911,   150,
     151,   152,   153,   912,     0,     0,   913,   905,     0,   914,
     915,   906,     0,     0,   916,     0,   907,   917,     0,   918,
       0,     0,   908,     0,     0,     0,   812,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   909,     0,
       0,     0,     0,   910,   919,   920,   911,     0,     0,     0,
       0,   912,     0,     0,   913,     0,     0,   914,   915,     0,
     815,     0,   916,   816,   817,   917,     0,   918,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   919,   920,     0,     0,     0,     0,   819,   820,
       0,   921,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   821,   822,     0,     0,   815,
       0,     0,   816,   817,   318,     0,   922,     0,     0,   210,
       0,   944,   883,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   819,   820,     0,
     921,     0,     0,     0,     0,     0,     0,   815,     0,     0,
     816,   817,     0,     0,     0,     0,     0,     0,     0,   805,
       0,     0,     0,     0,   821,   822,     0,     0,     0,     0,
       0,     0,     0,   318,     0,   922,     0,     0,   210,     0,
       0,     0,     0,     0,     0,   819,   820,     0,   921,     0,
       0,   150,   151,   152,   153,     0,     0,     0,     0,   905,
       0,     0,     0,   906,     0,     0,     0,     0,   907,     0,
       0,     0,   821,   822,   908,     0,     0,     0,   812,     0,
       0,     0,     0,   922,     0,     0,   210,     0,     0,     0,
     909,     0,     0,     0,     0,   910,     0,     0,   911,     0,
       0,     0,     0,   912,     0,     0,   913,     0,   146,   914,
     915,     0,     0,     0,   916,   147,   148,   917,     0,   918,
     288,   149,   289,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   290,     0,     0,
       0,     0,     0,     0,   919,   920,     0,   291,   292,     0,
       0,     0,   293,     0,     0,   294,     0,     0,     0,     0,
       0,     0,     0,   295,   296,   297,   298,   299,   300,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   301,     0,   302,     0,     0,   150,   151,   152,
     153,     0,     0,   154,     0,   155,     0,     0,     0,   156,
       0,     0,     0,     0,   157,     0,     0,     0,     0,     0,
     158,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   159,     0,     0,   815,
       0,   160,   816,   817,   161,     0,     0,     0,     0,   162,
       0,     0,   163,     0,     0,   164,   165,     0,     0,     0,
     166,     0,     0,   167,     0,   168,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   819,   820,     0,
     921,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     169,   170,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   821,   822,     0,     0,     0,     0,
       0,     0,     0,   945,     0,   922,     0,     0,   210,     0,
       0,     0,     0,     0,     0,     0,   171,   172,   173,   174,
       0,     0,     0,     0,     0,   175,   176,     0,     0,   177,
     178,   303,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,     0,     0,   304,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   146,     0,     0,     0,
     305,     0,     0,   147,   148,     0,     0,     0,     0,   149,
     306,     0,     0,     0,     0,     0,   307,     0,     0,     0,
       0,   308,     0,     0,     0,     0,     0,   202,   203,     0,
     309,     0,     0,   310,   311,   312,   313,     0,     0,     0,
     314,     0,     0,     0,   315,   316,   204,     0,     0,     0,
     205,   206,     0,     0,     0,     0,     0,     0,     0,   317,
       0,     0,   207,   208,     0,     0,     0,     0,     0,   318,
       0,   209,     0,     0,   210,   150,   151,   152,   153,     0,
       0,   154,     0,   155,     0,     0,     0,   156,     0,     0,
       0,     0,   157,     0,     0,     0,     0,     0,   158,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   159,     0,     0,     0,     0,   160,
       0,     0,   161,     0,     0,     0,     0,   162,     0,     0,
     163,     0,     0,   164,   165,     0,     0,     0,   166,     0,
       0,   167,     0,   168,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   169,   170,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   146,     0,
       0,     0,     0,     0,     0,   147,   148,     0,     0,     0,
       0,   149,     0,     0,   171,   172,   173,   174,     0,     0,
       0,     0,     0,   175,   176,     0,     0,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,     0,     0,   201,     0,     0,     0,     0,     0,     0,
       0,  2055,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   150,   151,   152,
     153,     0,     0,   154,     0,   155,     0,     0,     0,   156,
       0,     0,     0,     0,   157,   202,   203,     0,     0,     0,
     158,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   204,     0,   159,     0,   205,   206,
       0,   160,     0,     0,   161,     0,     0,     0,     0,   162,
     207,   208,   163,     0,     0,   164,   165,   318,     0,   209,
     166,     0,   210,   167,     0,   168,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   146,     0,     0,
     169,   170,     0,     0,   147,   148,     0,     0,     0,     0,
     149,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   171,   172,   173,   174,
       0,     0,     0,     0,     0,   175,   176,     0,     0,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,     0,     0,   201,   150,   151,   152,   153,
       0,     0,   154,     0,   155,     0,     0,   104,   156,     0,
       0,     0,   105,   157,     0,   106,   107,   108,   109,   158,
       0,   110,   111,     0,   112,   113,   114,     0,   115,     0,
       0,     0,     0,     0,     0,   159,     0,   202,   203,     0,
     160,     0,     0,   161,     0,     0,     0,     0,   162,     0,
       0,   163,     0,     0,   164,   165,   204,     0,     0,   166,
     205,   206,   167,     0,   168,     0,     0,   116,     0,   117,
     118,   119,   207,   208,     0,     0,     0,     0,     0,     0,
       0,   209,     0,     0,   210,     0,   146,     0,     0,   169,
     170,     0,     0,     0,   148,     0,     0,     0,     0,   149,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   171,   172,   173,   174,     0,
       0,     0,     0,   805,   175,   176,     0,     0,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,     0,     0,   201,   150,   151,   152,   153,     0,
       0,   154,     0,   808,     0,     0,     0,   809,     0,     0,
       0,     0,   810,     0,     0,     0,     0,     0,  1162,     0,
       0,     0,   812,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   159,     0,   202,   203,     0,   160,
       0,     0,  1163,     0,     0,     0,     0,   162,     0,     0,
     163,     0,     0,   164,   165,   204,     0,     0,   166,   205,
     206,   167,     0,   168,     0,     0,     0,     0,     0,     0,
     120,   207,   208,     0,     0,     0,     0,     0,     0,     0,
     209,     0,     0,   210,     0,   121,     0,     0,   169,   170,
     122,   123,   124,   125,     0,     0,     0,     0,     0,     0,
       0,   126,     0,     0,     0,     0,   127,     0,   128,   129,
       0,     0,     0,     0,     0,     0,     0,   883,     0,     0,
       0,     0,     0,     0,   814,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,     0,     0,   201,   805,     0,     0,     0,     0,     0,
       0,     0,     0,   815,     0,     0,   816,   817,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   150,   151,   152,   153,
       0,     0,     0,     0,   905,   202,   203,     0,   906,     0,
       0,   819,   820,   907,     0,     0,     0,     0,     0,   908,
       0,     0,     0,   812,   204,     0,     0,     0,   205,   206,
       0,     0,     0,     0,     0,   909,     0,     0,   821,   822,
     910,   674,     0,   911,     0,     0,     0,     0,   912,   823,
       0,   913,   210,     0,   914,   915,     0,     2,     3,   916,
       0,     4,   917,     5,   918,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     6,     7,     8,     9,     0,     0,   919,
     920,     0,     0,     0,    10,     0,     0,     0,     0,     0,
       0,     0,    11,     0,    12,     0,    13,     0,     0,    14,
      15,    16,     0,    17,     0,     0,     0,    18,    19,    20,
       0,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -395,     0,     0,     0,     0,     0,    34,     0,     0,     0,
       0,     0,     0,     0,   815,     0,    35,   816,   817,     0,
       0,     0,    36,     0,     0,     0,   805,     0,     0,     0,
       0,     0,     0,     0,   295,   296,   297,  1755,  1756,   300,
       0,     0,     0,     0,     0,     0,    37,     0,     0,     0,
       0,     0,   819,   820,     0,   921,     0,     0,   150,   151,
     152,   153,     0,     0,     0,     0,   905,     0,     0,     0,
     906,     0,     0,     0,   805,   907,     0,     0,     0,   821,
     822,   908,     0,     0,     0,   812,     0,     0,   318,     0,
     922,     0,     0,   210,     0,     0,     0,   909,     0,     0,
       0,     0,   910,     0,     0,   911,   150,   151,   152,   153,
     912,     0,     0,   913,   905,     0,   914,   915,   906,     0,
       0,   916,     0,   907,   917,     0,   918,     0,     0,   908,
       0,     0,     0,   812,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   909,     0,     0,     0,     0,
     910,   919,   920,   911,     0,     0,     0,     0,   912,     0,
       0,   913,     0,     0,   914,   915,     0,    38,     0,   916,
       0,     0,   917,     0,   918,     0,     0,     0,   805,     0,
       0,     0,    39,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    40,   919,
     920,     0,     0,    41,     0,    42,     0,     0,     0,     0,
    1575,   151,   152,   153,     0,     0,     0,     0,   905,     0,
       0,     0,   906,   668,     0,     0,     0,   907,     0,     0,
       0,     0,     0,   908,     0,     0,   815,   812,     0,   816,
     817,     0,     0,     0,     0,     0,     0,     0,     0,   909,
     669,     0,   759,   670,   910,     0,     0,   911,     0,     0,
       0,     0,   912,     0,     0,   913,     0,     0,   914,   915,
     671,     0,     0,   916,   819,   820,   917,   921,   918,   669,
       0,     0,   670,     0,   815,     0,     0,   816,   817,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   671,
       0,   821,   822,   919,   920,     0,     0,     0,     0,     0,
       0,     0,   922,     0,     0,   210,     0,     0,     0,     0,
       0,     0,   819,   820,     0,   921,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   821,
     822,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     922,     0,     0,   210,     0,     0,     0,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   815,     0,
       0,   816,   817,     0,     0,     0,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   819,   820,   672,   921,
       0,     0,   202,   203,     0,     0,     0,     0,     0,     0,
       0,     0,   673,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   821,   822,     0,     0,   672,     0,     0,
       0,   202,   203,     0,   922,     0,     0,   210,   674,     0,
       0,   673,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   674
};

static const yytype_int16 yycheck[] =
{
       1,    30,     3,   205,    30,   398,   329,   410,    66,    66,
     324,   299,   743,    30,    36,   477,   400,     1,    52,   399,
     401,   321,    52,    52,   581,    37,   636,   366,   779,    63,
     916,   773,   366,    63,    63,    52,   918,   805,   441,   876,
     352,   675,  1192,   377,   378,    46,    63,   427,  1336,   430,
      32,   782,   288,    54,   209,  1152,  1336,    58,   345,  1343,
     440,  1281,   793,  1204,  1205,  1340,   797,  1341,   521,  1342,
    1211,  1280,  1356,   475,   381,    35,  1335,  1336,  1445,   920,
     316,   366,   405,   397,   342,  1660,   344,   105,  1445,   107,
    1658,   109,   110,  1283,  1449,   113,  1659,   115,   608,  1498,
     118,   119,   338,  1637,   122,   123,   124,   125,   522,  1430,
    1431,   129,  1634,   436,   132,   133,  1562,   823,   890,   891,
     896,   892,   140,   476,   893,  1834,   760,     4,     4,     7,
      23,    23,   894,   486,   644,  1459,   550,     7,     7,     7,
      23,  1684,     7,   466,   558,    15,    16,    17,    18,   179,
    1291,    45,    38,    58,  1461,     7,     8,     9,   481,   573,
     639,   102,   641,     8,   643,  1700,    96,   740,     5,   648,
      15,    16,    17,    18,    22,   103,   205,    22,  1170,   102,
     209,  1505,  1506,  1701,    20,    84,   103,    58,   205,   700,
     742,   125,   209,    78,    79,   145,    48,   126,    50,   102,
     743,   103,   104,   617,    84,   113,    86,    59,   287,    54,
     672,    84,    64,   627,    57,   149,    68,   741,   127,  1892,
      13,    62,   103,   104,   105,   159,   103,   104,   103,   113,
     709,   287,     7,   334,   354,    84,   337,   699,   302,   359,
     298,   299,     8,   152,   975,    20,    30,    22,   662,    15,
      16,    17,    18,   764,   102,   103,    22,    98,   104,     1,
    1469,  1470,     4,    89,     6,   103,    92,   103,    52,   103,
     104,   105,  1560,   303,   256,   354,   125,    84,    89,    63,
     262,    92,   166,   167,    26,    27,    28,    29,    54,   162,
     354,   321,   321,    86,   352,  1619,  1620,    84,   354,    41,
     149,  1560,    44,     1,   321,  1232,     4,  1234,     6,   366,
     113,  1635,    45,    46,    30,     8,   304,   305,   268,   269,
     377,   378,    15,    16,    17,    18,    57,   130,    26,    27,
      28,    29,    15,    16,    49,   152,    52,   366,   125,   110,
      49,   399,   399,   251,   252,   162,   163,    63,   377,   378,
       5,  1233,   909,   127,   125,   912,   236,   286,   210,   162,
     394,    54,   149,    53,   394,   394,   354,    47,  1656,   427,
     427,   288,   103,  1632,   997,   409,  1656,   394,   152,   409,
     409,   304,   440,   440,    84,  1644,    86,  1379,   125,   737,
     738,  1167,   409,    76,    77,    78,  1655,  1656,   348,   972,
      84,   304,    32,    33,    34,   103,   104,   105,   145,    92,
      93,    94,   149,   288,   143,   349,   296,   103,   359,   102,
     162,   205,   974,  1730,   354,   209,  2099,   326,   111,   112,
     159,   359,   975,   270,   117,     8,   341,   342,   343,   457,
    1171,  1172,    15,    16,    17,    18,   298,   299,   285,   973,
     302,     4,   288,     6,  1988,   138,   139,   359,  2001,   142,
     354,   354,   354,  1985,   301,    89,   359,   359,   354,   306,
     341,   342,   343,   354,   359,     1,   359,   355,     4,   355,
       6,    54,   359,  1259,  2193,    45,   355,   355,   358,   205,
     355,  2026,    89,   209,     1,   338,   339,     4,    57,     6,
      26,    27,    28,    29,   352,   286,  1129,  1130,  1954,  2027,
       4,   359,     6,   358,   340,   322,   323,   324,   355,    26,
      27,    28,    29,   304,   305,   359,  2060,   549,  1787,   340,
     353,   268,   269,  2055,     7,    30,   236,   321,  1728,  1311,
    1312,  2116,  1313,   285,   103,  1314,  2114,    20,   750,   836,
     953,  1257,  2115,  1315,  1953,  1178,   141,    52,  1879,   411,
     572,   819,   820,   415,   322,  1785,  1890,   419,    63,   154,
      89,   773,   424,  1940,   332,   535,  1931,   429,  1031,  1420,
    1033,    89,    89,  1940,   896,   170,  1468,   113,   922,    96,
      57,    84,   358,    86,  1521,    57,   296,  1524,    89,    57,
    1527,   353,   941,  1017,   143,   321,   628,   941,   354,   148,
     394,   348,   897,   355,   110,   270,  1757,   247,   286,   631,
     968,   969,   970,     8,  1103,   409,   133,    38,    39,   125,
     285,     8,   166,   167,   302,  1517,   103,   985,   322,   323,
     324,   103,   340,   495,   354,   103,   301,     1,   132,  1790,
       4,   306,     6,   277,   961,   279,   941,   355,     7,     1,
     167,   236,     4,   921,     6,   358,  1763,  1876,   236,    54,
     294,    20,    26,    27,    28,    29,     1,    54,   394,     4,
     277,     6,   279,    12,    26,    27,    28,    29,   333,  1199,
       8,   113,   354,   409,   134,   342,    38,   359,  1160,   102,
     355,    26,    27,    28,    29,     0,    24,   111,   112,     4,
     205,     6,   134,  1223,   209,   773,    41,   286,   165,    44,
    1451,   750,   779,   170,   270,   147,   166,   167,     5,   169,
    1950,     8,   317,   750,  1357,     5,    54,  1360,     8,   285,
     895,   102,    71,   236,   773,   271,    75,  1498,   102,    20,
     779,    22,  1154,   735,   113,   301,   773,  2041,   277,  1510,
     279,    90,    91,  2038,   271,  2039,  1219,  2040,   127,   277,
       4,   279,     6,   456,   185,   134,   270,   188,    22,   108,
      30,    25,  1184,  1933,  1934,   358,   277,   162,   279,   811,
      20,   285,    22,   152,   823,    62,    63,    77,    30,   158,
      67,   813,    52,   296,  1157,    39,     7,   301,   137,   102,
     493,  1562,   306,    63,   340,   144,   351,   352,   876,    20,
      52,    22,   170,    79,  2112,   162,   321,  1437,  1151,   355,
     110,    63,  2112,   340,   286,  1188,    84,   162,   896,    95,
     897,  1475,  1476,   247,   248,   125,     1,   251,   355,     4,
       4,     6,  2111,  2112,  1477,  1167,  1479,  1480,   114,  1482,
     340,   890,   891,   892,   893,   894,   895,     4,   897,     6,
       7,    26,    27,    28,    29,   297,   102,   103,   895,   316,
    1597,  1598,  1599,  1600,   941,    17,    41,   113,  1350,    44,
     170,     1,  2033,   922,     4,    45,     6,   287,   288,   394,
     237,   238,   239,    18,     1,   285,  1142,     4,  1418,     6,
     290,   291,   941,   358,   409,  1752,    26,    27,    28,    29,
     354,   296,   297,   298,   299,   300,   306,   102,   103,    26,
      27,    28,    29,   102,   103,  1338,   354,     1,   113,     7,
       4,    52,     6,   103,   113,   946,  1348,  1259,   134,   950,
     304,   305,    20,    52,    22,   205,   354,    30,  1372,   209,
     285,   354,    26,    27,    28,    29,   750,   354,   128,   129,
      57,  1310,  1174,   205,   254,  1309,  1310,   209,  1317,    52,
     166,   167,  1316,  1317,  1318,  1319,   340,  1274,   287,   773,
      63,   354,    89,    43,  2135,     7,  1349,   113,   340,    96,
      45,   355,  1416,   113,    15,    16,   115,   162,  1776,  1751,
    1032,   197,  1034,   355,   130,   113,    45,    30,  1040,   135,
     136,    45,   131,     8,  1347,    89,   300,   125,   103,   127,
     355,   883,    96,    30,   750,   887,   133,     1,   360,    52,
       4,   360,     6,   152,   230,   304,   305,   145,   130,   158,
      63,   149,   134,   102,   152,    52,   350,   773,  1810,  1811,
    1812,  1813,    26,    27,    28,    29,    63,  1251,  1252,   133,
     167,   321,     1,   333,  1258,     4,   333,     6,   357,  1281,
    1690,  1283,   327,   328,   166,   167,    45,   247,    84,   321,
    1807,  1808,  1809,   137,   138,   139,   140,    26,    27,    28,
      29,    60,    61,   167,   175,   231,    23,   267,   268,  1167,
     113,   895,   115,  1736,   240,   241,    23,  2003,   244,   245,
     236,     1,  2004,     4,     4,     6,     6,     8,   102,    93,
     285,   134,   205,   242,   243,    84,   209,   235,    52,   142,
     143,    52,   145,   354,   394,  1174,    26,    27,    28,    29,
     109,    52,   155,   156,    15,    16,   127,  1174,    52,   409,
     119,   271,   394,    85,    86,    87,   125,    89,    52,   133,
     268,   269,   143,  1473,   271,    15,    16,   409,  1501,   895,
     220,   152,  1585,   102,   224,   225,   226,   158,   159,   113,
     149,   115,   205,  1607,   103,    17,   209,   106,   107,   108,
     355,  1259,  1259,  1954,   304,   305,     1,   271,   205,     4,
     134,     6,   209,  1965,  1966,  1967,   192,   193,   142,   143,
     144,   145,  1251,  1252,  1253,  1254,  1255,   102,  1257,  1258,
     340,    26,    27,    28,    29,   190,   191,  1266,  1267,   111,
     112,  1580,  1581,   340,   102,   355,  1580,  1581,   321,   102,
     348,  1713,  1281,   102,  1283,   750,   111,   112,   355,     4,
       5,  1561,  1885,    84,  1281,    84,  1283,  1554,    18,  1556,
     358,     4,  1895,     6,     5,     8,   340,     7,   773,  1902,
    1309,  1310,  1311,  1312,  1313,  1314,  1315,  1316,  1317,  1318,
    1319,   355,   215,   216,   217,   218,  1806,     7,    93,     7,
    1702,   111,   112,   113,  1334,  1334,     7,   271,   321,   111,
     112,  1821,   102,   103,  1937,    94,    95,  1334,   341,   342,
     343,   394,    43,    44,   321,  1648,   326,   327,   328,  1330,
    1331,   102,  1333,  1334,  1335,  1336,   409,   102,   133,     5,
    1742,     1,  1744,     7,     4,     7,     6,  1331,   354,  1333,
    1334,  1335,  1336,   366,  1707,    18,  1378,   358,   287,    17,
    1774,  1775,     8,  1777,   377,   378,    26,    27,    28,    29,
    1266,  1267,  1695,  1996,  1997,  1998,   340,   102,    38,     1,
       7,   394,     4,   102,     6,    38,  1943,  1944,   354,  2120,
    1174,   355,   354,   354,  1747,   287,   409,   394,   354,   302,
     895,  2024,  1424,  1425,    26,    27,    28,    29,   354,  1802,
     102,   340,   409,     1,  1426,  1427,     4,   102,     6,  1803,
     102,   102,   302,   102,  1805,   103,   355,  2158,   103,   104,
     105,   320,   102,   354,  1492,   322,   323,   324,    26,    27,
      28,    29,  1443,  1473,  1473,  1446,   102,  1448,    96,   189,
    1451,  1831,   194,  1510,    60,    61,  1473,   130,  1174,  1443,
     340,   224,  1446,   227,  1448,   227,   102,  1451,  1756,  1498,
     228,   102,   229,    45,  1984,   355,   271,  1857,  1512,  1989,
     102,  1510,  1512,  1512,   228,  1995,  1800,   102,    60,    61,
     102,   334,  1514,   102,   354,  1512,     5,  1281,   354,  1283,
     750,   102,     1,   109,   354,     4,    45,     6,   102,   102,
       8,  1512,   102,   119,    52,   103,   104,   105,   750,   125,
     102,    60,    61,   773,    43,    22,  1728,    26,    27,    28,
      29,  1561,  1561,  1562,   148,     5,   102,   109,    84,   357,
       8,   773,    84,   149,  1561,   340,   357,   119,  1606,  1751,
    1334,  1580,  1581,   125,   102,    22,  2179,  2180,  2181,    46,
     355,    48,    22,    50,    51,  1281,   354,  1283,  1982,   354,
     109,    58,    59,    97,    98,    99,   100,   149,   354,     7,
     119,   361,   288,  1785,   361,   102,   125,     5,  1617,   103,
     303,  1617,   303,   113,    93,   115,   116,   117,   118,   354,
    1617,    20,  2064,  1604,    91,  1634,    43,   307,  1634,   189,
     149,  1612,  1613,  1614,  1615,  1616,  1916,  1634,  1334,   287,
       7,    70,   302,     1,  1654,  1654,     4,   164,     6,  1630,
    1631,  1632,     5,     7,   133,    39,   102,  1654,   103,    98,
     102,  1642,  1643,  1644,   353,   895,   189,   103,    26,    27,
      28,    29,  1653,  1654,  1655,  1656,   353,   353,    98,     1,
    2062,   102,     4,   895,     6,   355,     7,   102,   102,  1653,
    1654,  1655,  1656,    20,    84,   102,   102,   750,   102,  1174,
     340,   102,   304,   305,    26,    27,    28,    29,   355,  1473,
    1691,  1692,  1693,  1751,  1752,   355,     5,  1755,  1756,  1728,
     773,   366,  2104,  2105,   188,   102,   150,  1691,  1692,  1693,
    2063,  1728,   377,   378,   103,   353,  2130,  2131,   340,   102,
     102,   102,  1751,  2185,   104,   287,   102,   288,  1512,   355,
     185,   354,     7,   355,  1751,   113,   354,   750,  2061,  1591,
    1592,  1593,  1594,   219,   220,   221,   222,   223,  1950,  2163,
     354,   354,   340,   750,   354,   354,  1785,  1473,   354,  2173,
     773,   310,   311,   312,   313,   314,   779,   355,  1785,  1770,
     354,  2174,   271,  1831,  1831,   354,   773,  1561,    41,   354,
    2103,  2183,   133,   102,     1,   355,  1281,     4,  1283,     6,
       1,   355,   103,     4,   354,     6,  1512,   354,   354,  1857,
    1857,     8,     8,   102,   102,   102,  1864,   103,   102,    26,
      27,    28,    29,     5,     8,    26,    27,    28,    29,   102,
     353,   913,   895,   919,    41,   915,  1827,    44,  1829,  1830,
     712,  2184,   366,  1617,   914,   711,    45,   710,   527,  1334,
    1025,   340,  1843,   377,   378,  1561,   713,  1365,  1849,    20,
    1634,    60,    61,  1339,   917,  1856,   355,  1983,  1859,  2182,
     517,   396,  1638,   876,  1865,  2135,  1884,  2188,  1981,  2045,
    1654,  1872,  1873,  1874,  2047,  2046,  1894,   890,   891,   892,
     893,   894,   895,  1901,   897,  2048,  1916,  1916,  1872,  1873,
    1874,    45,  2037,   271,  2160,    91,    89,  1593,   895,  1916,
     109,  1617,   393,  1755,  1756,   798,    60,    61,  1790,   922,
     119,   789,  1772,   308,  1712,  1743,   125,   859,  1634,  2036,
    1745,  1950,   395,  2108,  1174,  1954,  1253,  1255,   941,  1272,
    1254,   879,  1990,  1950,  1992,  1492,  1751,   774,  1654,  1279,
     149,   290,  1174,   888,  1728,   162,  1975,  1016,  1319,  1975,
     205,  1474,  1503,  1211,    45,   109,  1985,  1958,  1975,  1985,
     302,  1508,   340,   378,   407,   119,    37,  1751,  1985,    60,
      61,   125,   405,   120,    45,  2171,    47,   355,  1473,   408,
     303,    52,    53,    54,   406,    56,   637,    45,   304,    47,
    1991,   304,  1993,    -1,    52,   149,    54,  1081,   340,   615,
      58,  1785,   111,   112,  2005,  2006,    -1,  2008,  2009,  2010,
    2011,  2012,  1728,   355,  2015,    -1,   125,  1512,   109,    -1,
      -1,    -1,    -1,    -1,   133,    -1,  2055,    -1,   119,  2055,
      -1,  1281,    -1,  1283,   125,  1751,    -1,    -1,  2055,    97,
     149,    99,    -1,    -1,    37,    -1,    -1,    -1,    -1,  1281,
      -1,  1283,    45,    -1,    47,  2056,    49,    -1,   149,    52,
      53,    54,    -1,    56,  2065,  2066,  1561,    -1,   285,  1785,
      78,    79,    80,    81,    82,    83,   287,  2135,   111,   112,
    2110,  2110,    -1,    -1,  1334,    -1,    -1,   120,   121,   122,
     123,   124,    -1,  2110,    -1,  2124,    -1,    -1,  2124,    -1,
    2158,  1174,  1334,    -1,    -1,    -1,    -1,  2124,  2109,  2110,
    2111,  2112,    -1,    -1,   779,    -1,  2117,  2118,  2119,  2120,
      -1,  2122,  1617,    -1,  2125,  2109,  2110,  2111,  2112,   340,
    2188,    -1,  1916,  2117,  2118,  2119,  2120,    -1,   355,  1634,
    2141,    -1,    -1,    -1,   355,  2146,  2147,  2148,  2149,  2150,
    2151,  2152,  2153,  2154,  2155,  2156,  2157,    -1,   823,  1654,
      -1,  1174,   271,   272,   273,   274,  1950,  2151,  2152,  2153,
      -1,  2172,    -1,    -1,    45,    -1,    -1,  1174,   287,   288,
     289,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,
      61,  1975,    -1,    -1,    -1,  2196,  2197,  2198,    -1,   270,
    1916,  1985,    -1,    -1,    -1,    -1,    -1,    -1,  1281,    -1,
    1283,    -1,    -1,    -1,   285,    -1,    23,    -1,  1231,  1232,
      -1,  1234,    -1,  1473,    -1,   890,   891,   892,   893,   894,
     301,    38,   897,  1728,  1950,   306,    -1,   308,   109,    -1,
      -1,  1473,    -1,    -1,    -1,   779,    -1,    -1,   119,    -1,
      45,    -1,    -1,    -1,   125,    -1,  1751,   922,    -1,  1975,
      -1,  1334,  1512,    -1,    -1,    60,    61,    -1,  1281,  1985,
    1283,  2055,    -1,    -1,    -1,    -1,   941,   270,   149,    -1,
    1512,    -1,    -1,    -1,  1281,    -1,  1283,    -1,    -1,    -1,
    1785,    -1,   285,    -1,    -1,   102,  1309,  1310,  1311,  1312,
    1313,  1314,  1315,  1316,  1317,  1318,  1319,    -1,   301,    -1,
      -1,  1561,    -1,   306,   109,   308,    -1,    -1,  1207,    -1,
      -1,  1334,     1,    38,   119,     4,  2110,     6,    -1,  1561,
     125,    -1,    -1,    -1,    -1,    -1,    -1,  1334,    -1,  2055,
    2124,    -1,    -1,    -1,    -1,    -1,    -1,    26,    27,    28,
      29,    -1,    -1,    -1,   149,    -1,   890,   891,   892,   893,
     894,    40,    -1,   897,    -1,    -1,   173,  1617,    -1,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,    -1,    -1,  1634,  1617,    -1,   102,   922,    -1,
      -1,    -1,   134,    -1,  2110,    -1,    -1,    -1,    -1,    -1,
    1473,   143,  1634,    -1,  1654,    -1,    -1,   941,  2124,    88,
      89,    90,    91,    -1,    93,    -1,   158,    96,    -1,    -1,
     162,  1916,  1654,    -1,   166,   167,    -1,    -1,    -1,   171,
     172,   173,   174,   175,   113,    -1,    -1,    -1,    -1,  1512,
      -1,    -1,  1331,    -1,  1333,  1334,  1335,  1336,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1950,    -1,    -1,   173,    -1,
    1473,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,    -1,    -1,  1473,    -1,  1728,  1492,
    1975,    -1,    -1,   162,    -1,  1498,    -1,    -1,  1561,    -1,
    1985,    -1,    -1,    -1,    -1,    -1,  1728,  1510,    -1,  1512,
      -1,  1751,    -1,    -1,    -1,    -1,    -1,    -1,  1521,    -1,
      -1,  1524,    -1,    -1,  1527,  1512,    -1,    -1,    -1,  1751,
      -1,    -1,    -1,    -1,    -1,   107,    -1,   109,   110,    -1,
      -1,   113,    -1,   115,    -1,  1785,   118,   119,    -1,    -1,
     122,   123,   124,   125,  1617,    -1,    -1,   129,  1561,  1562,
     132,   133,    -1,  1785,  1443,   111,   112,  1446,   140,  1448,
    2055,  1634,  1451,    -1,  1561,    -1,    -1,  1580,  1581,    -1,
     126,    -1,   128,   129,   130,    -1,    -1,    -1,    -1,    -1,
      -1,  1654,    -1,    -1,    -1,    -1,  1251,  1252,  1253,  1254,
    1255,   270,  1257,  1258,    -1,   151,   152,   153,    -1,    -1,
      -1,  1266,  1267,    -1,  1617,    -1,   285,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  2110,    37,    -1,    -1,  1508,
    1617,  1634,   301,    -1,    45,    -1,    47,   306,    49,  2124,
      -1,    52,    53,    54,    -1,    56,    -1,  1634,    -1,    60,
      61,  1654,    -1,    -1,  1309,  1310,  1311,  1312,  1313,  1314,
    1315,  1316,  1317,  1318,  1319,  1728,    -1,  1654,    37,    -1,
      -1,   340,    -1,   747,    -1,   749,  1916,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   355,    -1,  1751,    -1,
      -1,    -1,   766,   767,  1916,   769,    -1,    -1,   109,    -1,
      -1,   775,   776,    -1,    -1,    -1,    -1,    -1,   119,    -1,
    1950,    -1,    -1,    -1,   125,    -1,    -1,    86,    -1,    -1,
      -1,    -1,  1785,    -1,    -1,  1728,    -1,    -1,  1950,    -1,
     804,    -1,   806,    -1,   103,  1975,    -1,    -1,   149,    -1,
      -1,  1728,    -1,    -1,    -1,  1985,    -1,    -1,  1751,  1752,
      -1,    -1,    -1,  1975,    -1,    -1,    -1,    -1,    -1,   128,
     129,    -1,    -1,  1985,  1751,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1653,  1654,  1655,  1656,    -1,    -1,
      -1,    -1,  1785,    -1,    -1,  1309,  1310,  1311,  1312,  1313,
    1314,  1315,  1316,  1317,  1318,  1319,   870,    70,  1785,    -1,
      -1,   111,   112,    -1,    -1,   115,    -1,    -1,    -1,    -1,
      -1,    -1,  1691,  1692,  1693,  2055,   126,   127,   128,   129,
     130,   131,   132,   133,   134,    -1,    -1,    -1,    -1,    -1,
     103,   104,   105,  2055,    -1,    -1,    -1,   110,    -1,    -1,
      -1,   114,    -1,  1498,    -1,    -1,   119,    -1,    -1,    -1,
      -1,    -1,   125,  1916,    -1,  1510,   129,    -1,    -1,   270,
      -1,    -1,    -1,    -1,    -1,   175,    -1,    -1,   942,   943,
    2110,    -1,    -1,    -1,   285,    -1,   149,    -1,   247,    -1,
      -1,    -1,    -1,    -1,  2124,   457,    -1,  1950,  2110,    -1,
     301,    -1,    -1,    -1,    -1,   306,    -1,   308,   267,   268,
      -1,    -1,  2124,    -1,    -1,    -1,    -1,  1562,    -1,    -1,
      -1,    -1,  1975,  1916,    -1,    -1,   285,    -1,    -1,    -1,
      -1,    -1,  1985,    -1,    -1,  1580,  1581,    -1,    -1,  1916,
      -1,    -1,   301,    -1,    -1,    -1,    -1,   306,    -1,   308,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1950,    -1,    10,
      -1,  1954,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1950,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1975,    -1,  1498,    -1,    -1,    -1,    39,    40,
      41,    42,  1985,    -1,    -1,    -1,  1510,    -1,  1975,    -1,
      -1,    -1,  2055,  1872,  1873,  1874,    -1,    -1,  1985,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   280,    -1,    -1,
     283,   284,    73,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      81,    82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1562,    -1,
      -1,    -1,    -1,   104,    -1,   318,   319,  2110,    -1,    -1,
      -1,    -1,  2055,    -1,    -1,    -1,  1580,  1581,    -1,    -1,
     121,  2124,    -1,    -1,    -1,   126,   127,   128,  2055,    -1,
     131,    -1,    -1,   134,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   359,  1161,    -1,     1,
      -1,  1165,     4,    -1,     6,    -1,    -1,    -1,    -1,    -1,
      -1,  1175,  1176,  1177,    -1,    -1,  1180,  2110,  1182,    -1,
      -1,    -1,    -1,    -1,    26,    27,    28,    29,    -1,    -1,
      -1,  2124,    -1,  2110,    -1,    37,    -1,    -1,  1202,    -1,
      -1,    -1,    -1,     1,    -1,    47,     4,  2124,     6,    -1,
      52,    -1,    54,    -1,    56,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,    27,
      28,    29,    -1,  1237,    -1,  1239,    -1,    -1,    -1,    -1,
      -1,  1245,    40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1260,    -1,  1262,    -1,
       1,    -1,    -1,    -1,    -1,    -1,    -1,  1271,    -1,  1273,
      11,    12,    13,    14,    15,    16,    17,    -1,    -1,    20,
     271,   272,   273,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      88,    89,    90,    91,    -1,    93,    -1,    -1,    96,    -1,
    2109,  2110,  2111,  2112,    -1,    -1,    -1,    -1,  2117,  2118,
    2119,  2120,    -1,    -1,    -1,   113,    -1,    -1,    -1,    -1,
     162,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      71,    -1,    -1,    74,    75,    76,    77,    78,    79,    -1,
      -1,    -1,  2151,  2152,  2153,    86,    -1,    88,    89,    90,
      91,    92,    93,    94,    95,    -1,    97,    98,    99,   100,
     180,   102,    -1,    -1,   162,   106,    -1,   108,    -1,  1954,
     111,   112,    -1,   114,    -1,    -1,   117,    -1,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   136,   137,   138,   139,    -1,
      -1,   142,    -1,   144,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   402,   403,   404,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   270,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   285,   435,    -1,    -1,    -1,    -1,    -1,
    1454,  1455,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   301,
      -1,    -1,    -1,    -1,   306,    -1,   308,    -1,    -1,    -1,
      -1,    -1,   270,   464,    -1,    -1,    -1,  1481,    -1,     1,
      -1,    -1,     4,    -1,     6,    -1,    -1,   285,    -1,    -1,
      -1,  1495,    -1,    -1,    -1,    -1,    -1,    -1,   340,    -1,
    1954,    -1,    -1,   301,    26,    27,    28,    29,   306,    -1,
      -1,    -1,    -1,   355,    -1,    -1,    -1,    -1,    40,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,  1530,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   340,  1547,    -1,  1549,    -1,    -1,    -1,    -1,
       1,    -1,    -1,     4,    -1,     6,    -1,   355,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    88,    89,    90,    91,
      -1,    93,    23,    -1,    96,    26,    27,    28,    29,    30,
      31,    -1,    -1,    -1,    35,    36,    37,    -1,    -1,    -1,
      -1,   113,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    62,    63,    -1,    -1,    -1,    67,    -1,    -1,    70,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,    80,
      81,    82,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     162,    -1,    93,    -1,    -1,    -1,    97,    -1,    99,    -1,
      -1,   102,   103,   104,   105,    -1,    -1,   108,    -1,   110,
      -1,    -1,   113,   114,    -1,    -1,    -1,    -1,   119,    -1,
      -1,    -1,    -1,    -1,   125,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     141,    -1,    -1,    -1,    -1,   146,    -1,    -1,   149,    -1,
      -1,    -1,    -1,   154,    -1,    -1,   157,    -1,   449,   160,
     161,   162,    -1,    -1,   165,   456,  1720,   168,    -1,   170,
      -1,    -1,    -1,    -1,    -1,    -1,   467,    -1,   469,    -1,
     471,    -1,   473,     1,    -1,    -1,     4,   478,     6,    -1,
      -1,   482,    89,   484,   195,   196,    -1,    -1,   270,    -1,
    1754,    -1,   493,    -1,    -1,    -1,    -1,    -1,    26,    27,
      28,    29,    -1,   285,   111,   112,    -1,    -1,   115,    -1,
      -1,    -1,    40,    -1,    -1,  1779,    -1,    -1,    -1,   301,
     231,   232,   233,   234,   306,    -1,   133,    -1,    -1,   240,
     241,    -1,    -1,   244,   245,   246,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,    -1,   340,   270,
      -1,    89,    -1,    91,    -1,    93,    -1,    -1,    96,    -1,
      -1,    -1,    -1,   355,   285,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   189,    -1,   295,   113,    -1,    -1,    -1,    -1,
     301,    -1,    -1,    -1,    -1,   306,    -1,    -1,    -1,    -1,
      -1,   312,   313,    -1,   315,    -1,    -1,   318,   319,   320,
     321,    -1,    -1,    -1,   325,    -1,    -1,    -1,   329,   330,
     331,    -1,    -1,    -1,   335,   336,    -1,     1,    -1,   340,
       4,    -1,     6,   344,   162,    -1,   347,   348,    -1,    -1,
     247,    -1,    -1,   354,   355,   356,    -1,    -1,   359,    23,
      -1,    -1,    26,    27,    28,    29,    30,    31,    -1,    -1,
      -1,    35,    36,    37,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   282,   283,   284,    52,    -1,
     287,   288,   289,    -1,  1948,    -1,    -1,    -1,    62,    63,
      -1,    -1,    -1,    67,    -1,    -1,    70,    -1,    -1,    -1,
      -1,   308,    -1,    -1,    78,    79,    80,    81,    82,    83,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    93,
      -1,    -1,    -1,    97,    -1,    99,    -1,    -1,   102,   103,
     104,   105,    -1,    -1,   108,    -1,   110,    -1,    -1,   113,
     114,    -1,   270,    -1,    -1,   119,    -1,    -1,    -1,    -1,
      -1,   125,    -1,    -1,    -1,    -1,    -1,   285,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   141,    -1,    -1,
      -1,    -1,   146,   301,    -1,   149,    -1,    -1,   306,    -1,
     154,    -1,    -1,   157,    -1,    -1,   160,   161,   162,    -1,
      -1,   165,    -1,    -1,   168,    -1,   170,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       1,    -1,   340,     4,    -1,     6,    -1,    -1,    -1,    -1,
      -1,   195,   196,    -1,    -1,    -1,    -1,   355,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    26,    27,    28,    29,    -1,
      -1,   111,   112,    -1,    -1,   115,    -1,    -1,    -1,    40,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   231,   232,   233,
     234,    -1,    -1,   133,    -1,    -1,   240,   241,    -1,    -1,
     244,   245,   246,   247,   248,   249,   250,   251,   252,   253,
     254,   255,   256,   257,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   267,    -1,    -1,   270,    -1,    89,    -1,
      91,    -1,    93,    -1,    -1,    96,    -1,    -1,    -1,    -1,
      -1,   285,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   295,   113,    -1,    -1,    -1,    -1,   301,    -1,    -1,
      -1,    -1,   306,    -1,    -1,    -1,    -1,    -1,   312,   313,
      -1,   315,    -1,    -1,   318,   319,   320,   321,    -1,    -1,
      -1,   325,    -1,    -1,    -1,   329,   330,   331,    -1,    -1,
      -1,   335,   336,    -1,     1,    -1,   340,     4,    -1,     6,
     344,   162,    -1,   347,   348,    -1,    -1,   247,    -1,    -1,
     354,   355,   356,    -1,    -1,   359,    23,    -1,    -1,    26,
      27,    28,    29,    30,    31,    -1,    -1,   998,    35,    36,
      37,   271,   272,   273,   274,   275,   276,    -1,   278,   279,
     280,   281,   282,   283,   284,    52,    -1,   287,   288,   289,
      -1,    -1,    -1,    -1,    -1,    62,    63,    -1,    -1,    -1,
      67,    -1,    -1,    70,    -1,    -1,    -1,    -1,   308,    -1,
      -1,    78,    79,    80,    81,    82,    83,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    93,    -1,    -1,    -1,
      97,    -1,    99,    -1,    -1,   102,   103,   104,   105,    -1,
      -1,   108,    -1,   110,    -1,    -1,   113,   114,    -1,   270,
      -1,    -1,   119,    -1,    -1,    -1,    -1,    -1,   125,    -1,
      -1,    -1,    -1,    -1,   285,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   141,    -1,    -1,    -1,    -1,   146,
     301,    -1,   149,    -1,    -1,   306,    -1,   154,    -1,    -1,
     157,    -1,    -1,   160,   161,   162,    -1,    -1,   165,    -1,
      -1,   168,    -1,   170,    -1,     1,    -1,    -1,     4,    -1,
       6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   340,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   195,   196,
      26,    27,    28,    29,   355,    -1,    -1,    -1,    -1,    -1,
      -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    47,    -1,    -1,    -1,    -1,    52,    -1,    54,    -1,
      56,    -1,    -1,    -1,   231,   232,   233,   234,    -1,    -1,
      -1,    -1,    -1,   240,   241,    -1,    -1,   244,   245,   246,
     247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,    -1,    -1,   270,    -1,     1,    -1,    -1,     4,    -1,
       6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   285,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   295,    -1,
      26,    27,    28,    29,   301,    -1,    -1,    -1,    -1,   306,
      -1,    -1,    -1,    -1,    40,   312,   313,    -1,   315,    -1,
      -1,   318,   319,   320,   321,    -1,    -1,    -1,   325,    -1,
      -1,    -1,   329,   330,   331,    -1,   162,    -1,   335,   336,
      -1,    -1,     4,   340,     6,    -1,    -1,   344,    -1,    -1,
     347,   348,    -1,    -1,    -1,    -1,    -1,   354,   355,   356,
      -1,    23,   359,    89,    -1,    91,    -1,    93,    30,    31,
      96,    -1,    -1,    35,    36,    37,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,    -1,    -1,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      62,    63,    -1,    -1,    -1,    67,    -1,    -1,    70,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    78,    79,    80,    81,
      82,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    97,   162,    99,    -1,    -1,
     102,   103,   104,   105,   270,    -1,   108,    -1,   110,    -1,
      -1,    -1,   114,    -1,    -1,    -1,    -1,   119,    -1,   285,
      -1,    -1,    -1,   125,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   301,    -1,    -1,    -1,   141,
     306,    -1,   308,    -1,   146,    -1,    -1,   149,    -1,    -1,
      -1,    -1,   154,    -1,    -1,   157,    -1,    -1,   160,   161,
      -1,    -1,    -1,   165,    -1,    -1,   168,    -1,   170,    -1,
      -1,    -1,    -1,    -1,   340,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   355,
      -1,    -1,    -1,   195,   196,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   270,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   285,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   231,
     232,   233,   234,    -1,    -1,   301,    -1,    -1,   240,   241,
     306,    -1,   244,   245,   246,   247,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,    -1,    -1,   270,    -1,
       5,    -1,    -1,    -1,   340,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   285,    -1,    -1,    -1,    -1,    23,   355,
      -1,    -1,    -1,   295,    -1,    -1,    31,    -1,    -1,   301,
      -1,    36,    -1,    -1,   306,    -1,    -1,    -1,    -1,    -1,
     312,   313,    -1,   315,    -1,    -1,   318,   319,   320,   321,
      -1,    -1,    -1,   325,    -1,    -1,    -1,   329,   330,   331,
      -1,    -1,    -1,   335,   336,    70,    -1,    -1,    -1,    -1,
      -1,    -1,   344,    -1,    -1,   347,   348,    -1,    -1,    84,
      -1,    86,   354,   355,   356,    -1,    -1,   359,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   102,   103,   104,
     105,    -1,    -1,   108,    -1,   110,    -1,    -1,    -1,   114,
      -1,    -1,    -1,    -1,   119,    -1,    -1,    -1,    -1,    -1,
     125,    -1,    -1,    -1,   129,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   141,    -1,    -1,    -1,
      -1,   146,    -1,    -1,   149,    -1,    -1,    -1,    -1,   154,
      -1,    -1,   157,    -1,    -1,   160,   161,    -1,    -1,    -1,
     165,    -1,    -1,   168,    -1,   170,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     195,   196,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     5,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    23,
      -1,    -1,    -1,    -1,    -1,    -1,   231,    31,    -1,    -1,
      -1,    -1,    36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,    -1,    -1,   270,    70,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   280,    -1,    -1,   283,   284,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   296,    -1,    -1,    -1,    -1,    -1,    -1,   102,   103,
     104,   105,    -1,    -1,   108,    -1,   110,   312,   313,    -1,
     114,    -1,    -1,   318,   319,   119,    -1,    -1,    -1,    -1,
      -1,   125,    -1,    -1,    -1,   129,   331,    -1,    -1,    -1,
     335,   336,    -1,    -1,    -1,    -1,    -1,   141,    -1,    -1,
     345,   346,   146,   348,    -1,   149,    -1,    -1,    -1,    -1,
     154,   356,    -1,   157,   359,    -1,   160,   161,    -1,    -1,
      -1,   165,    -1,    -1,   168,    -1,   170,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   195,   196,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     5,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    -1,    -1,    -1,   231,    31,    -1,
      -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   246,   247,   248,   249,   250,   251,   252,   253,
     254,   255,   256,   257,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   267,    -1,    -1,   270,    70,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   280,    -1,    -1,   283,
     284,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   102,
     103,   104,   105,    -1,    -1,   108,    -1,   110,   312,   313,
      -1,   114,    -1,    -1,   318,   319,   119,    -1,    -1,    -1,
      -1,    -1,   125,    -1,    -1,    -1,   129,   331,    -1,    -1,
      -1,   335,   336,    -1,    -1,    -1,    -1,    -1,   141,    -1,
      -1,   345,   346,   146,   348,    -1,   149,    -1,    -1,    -1,
      -1,   154,   356,    -1,   157,   359,    -1,   160,   161,    -1,
      -1,    -1,   165,    -1,    -1,   168,    -1,   170,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   195,   196,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     5,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,   231,    31,
      -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   246,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,    -1,    -1,   270,    70,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   280,    -1,    -1,
     283,   284,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     102,   103,   104,   105,    -1,    -1,   108,    -1,   110,   312,
     313,    -1,   114,    -1,    -1,   318,   319,   119,    -1,    -1,
      -1,    -1,    -1,   125,    -1,    -1,    -1,   129,   331,    -1,
      -1,    -1,   335,   336,    -1,    -1,    -1,    -1,    -1,   141,
      -1,    -1,   345,   346,   146,   348,    -1,   149,    -1,    -1,
      -1,    -1,   154,   356,    -1,   157,   359,    -1,   160,   161,
      -1,    -1,    -1,   165,    -1,    -1,   168,    -1,   170,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   195,   196,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     5,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,   231,
      31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   246,   247,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,    -1,    -1,   270,    70,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   280,    -1,
      -1,   283,   284,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   102,   103,   104,   105,    -1,    -1,   108,    -1,   110,
     312,   313,    -1,   114,    -1,    -1,   318,   319,   119,    -1,
      -1,    -1,    -1,    -1,   125,    -1,    -1,    -1,   129,   331,
      -1,    -1,    -1,   335,   336,    -1,    -1,    -1,    -1,    -1,
     141,    -1,    -1,   345,   346,   146,   348,    -1,   149,    -1,
      -1,    -1,    -1,   154,   356,    -1,   157,   359,    -1,   160,
     161,    -1,    -1,    -1,   165,    -1,    -1,   168,    -1,   170,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   195,   196,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,
      -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,
     231,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   246,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,    -1,    -1,   270,
      70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   280,
      -1,    -1,   283,   284,    -1,    -1,    -1,    -1,    -1,    -1,
      21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   102,   103,   104,   105,    -1,    -1,    -1,    -1,
     110,   312,   313,    -1,   114,    -1,    -1,   318,   319,   119,
      -1,    -1,    -1,    -1,    -1,   125,    -1,    -1,    -1,   129,
     331,    -1,    -1,    -1,   335,   336,    -1,    -1,    21,    70,
      -1,   141,    -1,    -1,   345,   346,   146,   348,    -1,   149,
      -1,    -1,    -1,    -1,   154,   356,    -1,   157,   359,    -1,
     160,   161,    -1,    -1,    -1,   165,    -1,    -1,   168,    -1,
     170,   102,   103,   104,   105,    -1,    -1,    -1,    -1,   110,
      -1,    -1,    -1,   114,    -1,    -1,    -1,    70,   119,    -1,
      -1,    -1,    -1,    -1,   125,   195,   196,    -1,   129,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     141,    -1,    -1,    -1,    -1,   146,    -1,    -1,   149,   102,
     103,   104,   105,   154,    -1,    -1,   157,   110,    -1,   160,
     161,   114,    -1,    -1,   165,    -1,   119,   168,    -1,   170,
      -1,    -1,   125,    -1,    -1,    -1,   129,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   141,    -1,
      -1,    -1,    -1,   146,   195,   196,   149,    -1,    -1,    -1,
      -1,   154,    -1,    -1,   157,    -1,    -1,   160,   161,    -1,
     280,    -1,   165,   283,   284,   168,    -1,   170,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   195,   196,    -1,    -1,    -1,    -1,   318,   319,
      -1,   321,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   345,   346,    -1,    -1,   280,
      -1,    -1,   283,   284,   354,    -1,   356,    -1,    -1,   359,
      -1,    22,    23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   318,   319,    -1,
     321,    -1,    -1,    -1,    -1,    -1,    -1,   280,    -1,    -1,
     283,   284,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,
      -1,    -1,    -1,    -1,   345,   346,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   354,    -1,   356,    -1,    -1,   359,    -1,
      -1,    -1,    -1,    -1,    -1,   318,   319,    -1,   321,    -1,
      -1,   102,   103,   104,   105,    -1,    -1,    -1,    -1,   110,
      -1,    -1,    -1,   114,    -1,    -1,    -1,    -1,   119,    -1,
      -1,    -1,   345,   346,   125,    -1,    -1,    -1,   129,    -1,
      -1,    -1,    -1,   356,    -1,    -1,   359,    -1,    -1,    -1,
     141,    -1,    -1,    -1,    -1,   146,    -1,    -1,   149,    -1,
      -1,    -1,    -1,   154,    -1,    -1,   157,    -1,    23,   160,
     161,    -1,    -1,    -1,   165,    30,    31,   168,    -1,   170,
      35,    36,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,
      -1,    -1,    -1,    -1,   195,   196,    -1,    62,    63,    -1,
      -1,    -1,    67,    -1,    -1,    70,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    78,    79,    80,    81,    82,    83,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    97,    -1,    99,    -1,    -1,   102,   103,   104,
     105,    -1,    -1,   108,    -1,   110,    -1,    -1,    -1,   114,
      -1,    -1,    -1,    -1,   119,    -1,    -1,    -1,    -1,    -1,
     125,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   141,    -1,    -1,   280,
      -1,   146,   283,   284,   149,    -1,    -1,    -1,    -1,   154,
      -1,    -1,   157,    -1,    -1,   160,   161,    -1,    -1,    -1,
     165,    -1,    -1,   168,    -1,   170,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   318,   319,    -1,
     321,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     195,   196,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   345,   346,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   354,    -1,   356,    -1,    -1,   359,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   231,   232,   233,   234,
      -1,    -1,    -1,    -1,    -1,   240,   241,    -1,    -1,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,    -1,    -1,   270,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,
     285,    -1,    -1,    30,    31,    -1,    -1,    -1,    -1,    36,
     295,    -1,    -1,    -1,    -1,    -1,   301,    -1,    -1,    -1,
      -1,   306,    -1,    -1,    -1,    -1,    -1,   312,   313,    -1,
     315,    -1,    -1,   318,   319,   320,   321,    -1,    -1,    -1,
     325,    -1,    -1,    -1,   329,   330,   331,    -1,    -1,    -1,
     335,   336,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   344,
      -1,    -1,   347,   348,    -1,    -1,    -1,    -1,    -1,   354,
      -1,   356,    -1,    -1,   359,   102,   103,   104,   105,    -1,
      -1,   108,    -1,   110,    -1,    -1,    -1,   114,    -1,    -1,
      -1,    -1,   119,    -1,    -1,    -1,    -1,    -1,   125,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   141,    -1,    -1,    -1,    -1,   146,
      -1,    -1,   149,    -1,    -1,    -1,    -1,   154,    -1,    -1,
     157,    -1,    -1,   160,   161,    -1,    -1,    -1,   165,    -1,
      -1,   168,    -1,   170,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   195,   196,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    23,    -1,
      -1,    -1,    -1,    -1,    -1,    30,    31,    -1,    -1,    -1,
      -1,    36,    -1,    -1,   231,   232,   233,   234,    -1,    -1,
      -1,    -1,    -1,   240,   241,    -1,    -1,   244,   245,   246,
     247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,    -1,    -1,   270,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   102,   103,   104,
     105,    -1,    -1,   108,    -1,   110,    -1,    -1,    -1,   114,
      -1,    -1,    -1,    -1,   119,   312,   313,    -1,    -1,    -1,
     125,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   331,    -1,   141,    -1,   335,   336,
      -1,   146,    -1,    -1,   149,    -1,    -1,    -1,    -1,   154,
     347,   348,   157,    -1,    -1,   160,   161,   354,    -1,   356,
     165,    -1,   359,   168,    -1,   170,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,
     195,   196,    -1,    -1,    30,    31,    -1,    -1,    -1,    -1,
      36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   231,   232,   233,   234,
      -1,    -1,    -1,    -1,    -1,   240,   241,    -1,    -1,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,    -1,    -1,   270,   102,   103,   104,   105,
      -1,    -1,   108,    -1,   110,    -1,    -1,    37,   114,    -1,
      -1,    -1,    42,   119,    -1,    45,    46,    47,    48,   125,
      -1,    51,    52,    -1,    54,    55,    56,    -1,    58,    -1,
      -1,    -1,    -1,    -1,    -1,   141,    -1,   312,   313,    -1,
     146,    -1,    -1,   149,    -1,    -1,    -1,    -1,   154,    -1,
      -1,   157,    -1,    -1,   160,   161,   331,    -1,    -1,   165,
     335,   336,   168,    -1,   170,    -1,    -1,    97,    -1,    99,
     100,   101,   347,   348,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   356,    -1,    -1,   359,    -1,    23,    -1,    -1,   195,
     196,    -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   231,   232,   233,   234,    -1,
      -1,    -1,    -1,    70,   240,   241,    -1,    -1,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,    -1,    -1,   270,   102,   103,   104,   105,    -1,
      -1,   108,    -1,   110,    -1,    -1,    -1,   114,    -1,    -1,
      -1,    -1,   119,    -1,    -1,    -1,    -1,    -1,   125,    -1,
      -1,    -1,   129,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   141,    -1,   312,   313,    -1,   146,
      -1,    -1,   149,    -1,    -1,    -1,    -1,   154,    -1,    -1,
     157,    -1,    -1,   160,   161,   331,    -1,    -1,   165,   335,
     336,   168,    -1,   170,    -1,    -1,    -1,    -1,    -1,    -1,
     270,   347,   348,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     356,    -1,    -1,   359,    -1,   285,    -1,    -1,   195,   196,
     290,   291,   292,   293,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   301,    -1,    -1,    -1,    -1,   306,    -1,   308,   309,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,
      -1,    -1,    -1,    -1,   231,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   246,
     247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,    -1,    -1,   270,    70,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   280,    -1,    -1,   283,   284,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   102,   103,   104,   105,
      -1,    -1,    -1,    -1,   110,   312,   313,    -1,   114,    -1,
      -1,   318,   319,   119,    -1,    -1,    -1,    -1,    -1,   125,
      -1,    -1,    -1,   129,   331,    -1,    -1,    -1,   335,   336,
      -1,    -1,    -1,    -1,    -1,   141,    -1,    -1,   345,   346,
     146,   348,    -1,   149,    -1,    -1,    -1,    -1,   154,   356,
      -1,   157,   359,    -1,   160,   161,    -1,     0,     1,   165,
      -1,     4,   168,     6,   170,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    26,    27,    28,    29,    -1,    -1,   195,
     196,    -1,    -1,    -1,    37,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    47,    -1,    49,    -1,    -1,    52,
      53,    54,    -1,    56,    -1,    -1,    -1,    60,    61,    62,
      -1,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     103,    -1,    -1,    -1,    -1,    -1,   109,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   280,    -1,   119,   283,   284,    -1,
      -1,    -1,   125,    -1,    -1,    -1,    70,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    78,    79,    80,    81,    82,    83,
      -1,    -1,    -1,    -1,    -1,    -1,   149,    -1,    -1,    -1,
      -1,    -1,   318,   319,    -1,   321,    -1,    -1,   102,   103,
     104,   105,    -1,    -1,    -1,    -1,   110,    -1,    -1,    -1,
     114,    -1,    -1,    -1,    70,   119,    -1,    -1,    -1,   345,
     346,   125,    -1,    -1,    -1,   129,    -1,    -1,   354,    -1,
     356,    -1,    -1,   359,    -1,    -1,    -1,   141,    -1,    -1,
      -1,    -1,   146,    -1,    -1,   149,   102,   103,   104,   105,
     154,    -1,    -1,   157,   110,    -1,   160,   161,   114,    -1,
      -1,   165,    -1,   119,   168,    -1,   170,    -1,    -1,   125,
      -1,    -1,    -1,   129,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   141,    -1,    -1,    -1,    -1,
     146,   195,   196,   149,    -1,    -1,    -1,    -1,   154,    -1,
      -1,   157,    -1,    -1,   160,   161,    -1,   270,    -1,   165,
      -1,    -1,   168,    -1,   170,    -1,    -1,    -1,    70,    -1,
      -1,    -1,   285,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   301,   195,
     196,    -1,    -1,   306,    -1,   308,    -1,    -1,    -1,    -1,
     102,   103,   104,   105,    -1,    -1,    -1,    -1,   110,    -1,
      -1,    -1,   114,   103,    -1,    -1,    -1,   119,    -1,    -1,
      -1,    -1,    -1,   125,    -1,    -1,   280,   129,    -1,   283,
     284,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   141,
     130,    -1,   103,   133,   146,    -1,    -1,   149,    -1,    -1,
      -1,    -1,   154,    -1,    -1,   157,    -1,    -1,   160,   161,
     150,    -1,    -1,   165,   318,   319,   168,   321,   170,   130,
      -1,    -1,   133,    -1,   280,    -1,    -1,   283,   284,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   150,
      -1,   345,   346,   195,   196,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   356,    -1,    -1,   359,    -1,    -1,    -1,    -1,
      -1,    -1,   318,   319,    -1,   321,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   345,
     346,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     356,    -1,    -1,   359,    -1,    -1,    -1,   247,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   280,    -1,
      -1,   283,   284,    -1,    -1,    -1,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   318,   319,   308,   321,
      -1,    -1,   312,   313,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   322,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   345,   346,    -1,    -1,   308,    -1,    -1,
      -1,   312,   313,    -1,   356,    -1,    -1,   359,   348,    -1,
      -1,   322,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   348
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,   363,     0,     1,     4,     6,    26,    27,    28,    29,
      37,    45,    47,    49,    52,    53,    54,    56,    60,    61,
      62,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,   109,   119,   125,   149,   270,   285,
     301,   306,   308,   364,   417,   418,   419,   420,   492,   493,
     494,   496,   511,   364,   104,   103,   489,   489,   489,   494,
     505,   494,   496,   511,   494,   499,   499,   499,   494,   502,
     420,    49,   421,    37,    45,    47,    52,    53,    54,    56,
     270,   285,   301,   306,   308,   422,    49,   423,    37,    45,
      47,    49,    52,    53,    54,    56,   270,   285,   301,   306,
     308,   428,    53,   429,    37,    42,    45,    46,    47,    48,
      51,    52,    54,    55,    56,    58,    97,    99,   100,   101,
     270,   285,   290,   291,   292,   293,   301,   306,   308,   309,
     430,   285,   290,   291,   306,   433,    45,    47,    52,    54,
      58,    97,    99,   434,    47,   435,    23,    30,    31,    36,
     102,   103,   104,   105,   108,   110,   114,   119,   125,   141,
     146,   149,   154,   157,   160,   161,   165,   168,   170,   195,
     196,   231,   232,   233,   234,   240,   241,   244,   245,   246,
     247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   270,   312,   313,   331,   335,   336,   347,   348,   356,
     359,   443,   490,   613,   614,   617,   618,   619,   623,   686,
     689,   691,   695,   700,   701,   703,   705,   715,   716,   718,
     720,   722,   724,   728,   730,   732,   734,   736,   738,   740,
     742,   744,   746,   757,   765,   767,   769,   770,   772,   774,
     776,   778,   780,   782,   784,   786,    58,   341,   342,   343,
     436,   442,    58,   437,   442,   103,   438,   439,   367,   384,
     385,    89,   277,   279,   505,   505,   505,   505,     0,   364,
     489,   489,    57,   338,   339,   508,   509,   510,    35,    37,
      52,    62,    63,    67,    70,    78,    79,    80,    81,    82,
      83,    97,    99,   246,   270,   285,   295,   301,   306,   315,
     318,   319,   320,   321,   325,   329,   330,   344,   354,   515,
     516,   517,   519,   520,   521,   522,   523,   524,   528,   529,
     530,   533,   534,   541,   545,   553,   554,   557,   558,   559,
     560,   561,   582,   583,   585,   586,   588,   589,   592,   593,
     594,   604,   605,   606,   607,   608,   611,   612,   618,   625,
     626,   627,   628,   629,   630,   634,   635,   636,   670,   684,
     689,   690,   713,   714,   715,   747,   364,   353,   353,   364,
     489,   565,   444,   447,   515,   489,   452,   454,   613,   636,
     457,   489,   462,   496,   512,   505,   494,   496,   499,   499,
     499,   502,    89,   277,   279,   505,   505,   505,   505,   511,
     427,   494,   505,   506,   424,   492,   494,   495,   425,   494,
     496,   497,   512,   426,   494,   499,   500,   499,   499,   494,
     502,   503,    89,   277,   279,   659,   427,   427,   427,   427,
     499,   505,   432,   493,   514,   494,   514,   496,   514,    45,
     514,   499,   499,   514,   502,   514,    45,    46,   499,   514,
     514,    89,   277,   294,   659,   660,   505,    45,   514,    45,
     514,    45,   514,    45,   514,   505,   505,   505,    45,   514,
     391,   505,    45,   514,    45,   514,   505,   403,   494,   496,
     499,   499,   514,    45,   499,   496,   103,   106,   107,   108,
     717,   111,   112,   247,   248,   251,   621,   622,    32,    33,
      34,   247,   692,   132,   624,   166,   167,   768,   111,   112,
     113,   719,   113,   115,   116,   117,   118,   721,   111,   112,
     120,   121,   122,   123,   124,   723,   111,   112,   115,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   175,   725,
     113,   115,   134,   142,   143,   144,   145,   729,   113,   134,
     147,   297,   731,   111,   112,   126,   128,   129,   130,   151,
     152,   153,   733,   113,   115,   134,   142,   143,   145,   155,
     156,   735,   127,   143,   152,   158,   159,   737,   143,   159,
     739,   152,   162,   163,   741,   130,   134,   166,   167,   743,
     134,   166,   167,   169,   745,   134,   143,   158,   162,   166,
     167,   171,   172,   173,   174,   175,   748,   113,   166,   167,
     758,   134,   166,   167,   197,   230,   766,   113,   125,   127,
     145,   149,   152,   235,   268,   269,   348,   702,   704,   773,
     236,   775,   236,   777,   162,   237,   238,   239,   779,   127,
     152,   771,   115,   131,   152,   158,   242,   243,   781,   127,
     152,   783,   113,   127,   134,   152,   158,   785,   103,   130,
     133,   150,   308,   322,   348,   687,   688,   689,   111,   112,
     115,   133,   247,   271,   272,   273,   274,   275,   276,   278,
     279,   280,   281,   282,   283,   284,   287,   288,   289,   308,
     706,   707,   710,   322,   332,   694,   630,   635,   333,   231,
     240,   241,   244,   245,   787,   351,   352,   390,   697,   629,
     489,   409,   442,   342,   389,   442,   377,   395,    46,    48,
      50,    51,    58,    59,    91,   440,   505,   505,   505,   371,
     654,   669,   656,   658,   102,   102,   102,    84,   702,   286,
     605,   170,   489,   613,   685,   685,    62,    98,   489,   103,
     687,    89,   189,   277,   706,   707,   286,   286,   302,   286,
     304,   305,   542,    84,   162,    84,    84,   702,     4,   365,
     637,   638,   340,   513,   520,   447,   371,   287,   288,   531,
     532,   415,   162,   296,   297,   298,   299,   300,   535,   536,
     402,   316,   556,   396,     5,    70,    84,    86,   110,   114,
     119,   125,   129,   149,   231,   280,   283,   284,   296,   318,
     319,   345,   346,   356,   568,   569,   570,   571,   572,   573,
     574,   576,   577,   578,   579,   580,   581,   614,   617,   623,
     679,   680,   681,   686,   691,   695,   701,   702,   703,   705,
     711,   712,   715,   410,   416,    38,    39,   185,   188,   562,
     563,   396,    84,   322,   323,   324,   584,   590,   591,   396,
      84,   587,   590,   374,   381,   401,   326,   327,   328,   595,
     596,   600,   601,    23,   613,   615,   616,    45,   609,   610,
      15,    16,    17,    18,   358,     8,    24,    54,     9,    10,
      11,    12,    13,    14,    19,   110,   114,   119,   125,   141,
     146,   149,   154,   157,   160,   161,   165,   168,   170,   195,
     196,   321,   356,   614,   616,   617,   631,   632,   633,   636,
     671,   672,   673,   674,   675,   676,   677,   678,   680,   681,
     682,   683,    52,    52,    22,   354,   652,   671,   672,   677,
     652,    38,   354,   564,   354,   354,   354,   354,   354,   508,
     515,   565,   444,   447,   452,   454,   457,   462,   505,   505,
     505,   371,   654,   669,   656,   658,   515,   416,    57,    57,
      57,    57,   454,    57,   462,   505,   371,   392,   400,   407,
     454,   416,    43,   431,   494,   499,   514,   505,    45,   371,
     494,   494,   494,   494,   392,   400,   407,   494,   371,   494,
     494,   400,   499,   489,   412,     7,     8,   113,   251,   252,
     620,   300,   408,   103,   126,   286,   412,   411,   376,   411,
     386,   110,   125,   110,   125,   367,   137,   138,   139,   140,
     726,   384,   411,   387,   411,   388,   385,   411,   387,   366,
     375,   379,   369,   413,   414,   379,    23,    38,   102,   173,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   749,   750,   751,   411,   372,   180,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   759,   764,   405,   411,
     384,   385,   390,   704,   404,   404,   360,   404,   404,   379,
     360,   404,   383,   380,   373,   411,   394,   393,   407,   393,
     407,   111,   112,   125,   133,   149,   271,   272,   273,   708,
     709,   710,   370,   333,   333,   102,   404,   383,   380,   373,
     394,   350,   696,   357,   441,   442,   667,   667,   667,   287,
     354,   653,   302,   354,   668,   354,   542,   655,   354,   490,
     657,     5,   125,   149,   580,    84,   580,   602,   603,   630,
     175,    23,    23,    96,   354,    52,    52,    52,   102,   304,
      52,   710,    52,   580,   580,   304,   305,   546,   580,   102,
     597,   598,   599,   613,   617,   630,   634,   695,   701,   600,
     580,   580,    84,    21,   636,   641,   642,   643,   650,   677,
     678,     7,   355,   490,   354,   102,   102,   532,    77,   110,
     125,   170,   254,   538,   490,   102,   102,   102,   490,   537,
     536,   141,   154,   170,   317,   580,   403,     5,   580,    84,
     376,   386,   367,   384,   385,    84,   396,   396,   573,   614,
     681,    15,    16,    17,    18,   358,    20,    22,     8,    54,
       5,   590,    84,    86,   236,   296,     7,     7,   102,   102,
     563,     5,     7,     5,   580,   598,   613,   617,   596,     7,
     489,   354,   489,   354,   610,   682,   682,   673,   674,   675,
     629,   354,   525,   615,   672,   384,   387,   385,   387,   366,
     375,   369,   413,   414,   409,   372,   405,   396,   677,     7,
      20,    15,    16,    17,    18,   358,     7,    20,    22,     8,
     671,   672,   677,   580,   580,   102,   355,   364,    20,   364,
     102,   477,   416,   446,   448,   453,   459,   463,   564,   354,
     354,   354,   354,   354,   667,   667,   667,   653,   668,   655,
     657,   102,   102,   102,   102,   102,   354,   667,   103,   370,
     494,   102,   622,   411,   378,   102,   398,   398,   376,   384,
     376,   384,   113,   130,   135,   136,   236,   384,   727,   368,
      96,   755,   189,   753,   194,   756,   192,   193,   754,   190,
     191,   752,   130,   220,   224,   225,   226,   763,   215,   216,
     217,   218,   761,   219,   220,   221,   222,   223,   762,   762,
     224,   227,   227,   228,   229,   228,   113,   130,   162,   760,
     406,   404,   102,   102,   111,   112,   111,   112,   370,   370,
     102,   102,   334,   693,   102,   159,   349,   698,   702,   354,
     354,   354,   102,   470,   371,   546,   475,   392,   471,   102,
     400,   476,   407,   580,     5,     5,   580,   615,    89,    92,
     513,   644,   645,    38,   173,   178,   188,   750,   751,   490,
     490,   102,   630,   639,   640,   580,   580,   580,   370,   102,
     580,    52,   580,   392,   102,   548,   550,   551,   400,   103,
     288,   543,    22,   401,   379,    43,   580,   365,     5,   365,
     270,   285,   301,   647,   648,    89,    92,   513,   646,   649,
     365,   638,   449,   376,   148,   143,   148,   539,   540,   379,
     103,   113,   555,   617,   113,   555,   409,   113,   555,   580,
       5,   580,   580,   357,   568,   568,   569,   570,   571,   102,
     573,   568,   575,   615,   636,   580,   580,    84,     8,    84,
     614,   681,   711,   711,   580,   591,   580,   590,   601,   368,
     602,   639,   365,   526,   527,   357,   677,   671,   677,   682,
     682,   673,   674,   675,   677,   102,   671,   677,   633,   677,
      20,    20,   102,    39,   364,   355,   364,   417,   513,   564,
      37,    47,    52,    54,    56,   162,   270,   285,   301,   306,
     308,   355,   364,   417,   445,   513,    93,   113,   162,   355,
     364,   417,   479,   485,   486,   513,   515,    40,    88,    89,
      90,    91,    93,    96,   113,   162,   270,   355,   364,   417,
     460,   513,   518,   519,    40,    89,    91,   113,   162,   355,
     364,   417,   460,   513,   518,    41,    44,   162,   285,   355,
     364,   417,   416,   446,   448,   453,   459,   463,   354,   354,
     354,   371,   392,   400,   407,   463,   370,   370,     7,   408,
     411,   384,   751,   411,   379,   405,   361,   361,   384,   384,
     385,   385,   693,   337,   693,   102,   382,   390,   111,   112,
     699,   473,   474,   472,   288,   355,   364,   417,   513,   653,
     548,   550,   355,   364,   417,   513,   668,   355,   364,   417,
     513,   655,   543,   355,   364,   417,   513,   657,   580,   580,
       5,   103,   491,   491,   645,   409,   368,   368,   354,   520,
     644,   393,   393,   370,   370,   370,   580,   370,    20,   103,
     288,   303,   547,   303,   549,    20,   307,   544,   597,   613,
     617,    84,   326,   580,    43,    81,    82,   651,   678,   684,
     189,   287,   371,   302,   648,   491,   491,   649,   355,   364,
     515,   384,     7,   409,   555,   555,    70,   555,   580,     5,
     580,   164,   580,   590,   590,     5,   355,   518,   520,   641,
       7,   355,   671,   671,   102,    39,   416,   489,   507,   489,
     498,   489,   501,   501,   489,   504,   103,    89,   277,   279,
     507,   507,   507,   507,   364,    78,    79,   487,   488,   613,
     411,    98,   364,   364,   364,   364,   364,   451,   618,   491,
     491,   353,    94,    95,   461,   102,   103,   128,   129,   247,
     267,   268,   467,   468,   478,    85,    86,    87,    89,   455,
     456,   364,   364,   364,   519,   451,   491,   353,   468,   455,
     364,   364,   364,   103,   353,    98,   371,   355,   355,   355,
     355,   355,   473,   474,   472,   355,   102,     7,   397,   102,
     382,   390,    93,   133,   271,   355,   364,   417,   513,   665,
      89,    96,   133,   167,   271,   355,   364,   417,   513,   666,
     113,   271,   355,   364,   417,   513,   662,   102,   371,   547,
     549,   392,   400,   544,   407,   580,   639,   355,   370,   310,
     311,   312,   313,   314,   552,   102,   392,   102,   551,   392,
     552,   102,   400,   599,   598,   580,   365,   102,   304,   102,
     546,   364,   540,   411,   411,   403,   411,   580,    84,   602,
       5,   355,   355,     5,   365,   527,   188,   566,   102,   469,
     447,   452,   457,   462,   379,   507,   507,   507,   469,   469,
     469,   469,   399,   103,   379,     8,   364,   364,   364,   454,
     399,     8,   364,     7,   364,     5,   364,   454,     5,   364,
     150,   480,   354,   464,   613,   364,   355,   355,   355,   368,
     102,   693,   353,   165,   170,   661,   493,   370,   491,   102,
     661,   102,   493,   370,   104,   493,   370,   520,   287,   103,
     543,   401,   401,   370,   102,   288,   548,   550,   387,   387,
     580,   355,   602,   684,   185,   567,   364,   354,   354,   354,
     354,   354,   469,   469,   469,   354,   354,   354,   354,    41,
     618,   467,   411,   456,   379,    86,   450,   451,   618,    37,
      86,   285,   301,   306,   308,   458,   468,   379,    22,   102,
     103,   352,   481,   482,   483,   613,   364,   103,   104,   465,
     466,   613,   364,   379,   370,   370,   370,     7,   382,   354,
     413,   409,   364,   364,   364,   364,   364,   364,   364,   133,
     364,   355,   370,   102,   547,   549,   355,   365,   566,   477,
     448,   453,   459,   463,   354,   354,   354,   470,   475,   471,
     476,   103,   451,   364,     8,   468,   371,   392,   400,   407,
     364,   364,   102,    22,    25,     7,   355,   102,   103,   663,
     664,   661,   371,   392,   392,   567,   355,   355,   355,   355,
     355,   473,   474,   472,   355,   355,   355,   355,    43,    44,
     484,   364,   618,   364,   411,   411,   102,   102,   365,   466,
       5,     7,   355,   364,   364,   364,   364,   364,   364,   355,
     355,   355,   364,   364,   364,   364,   490,   613,   353,   480,
     411,   102,   664,   364,   411,   416,   370,   370,   370,   371,
     392,   400,   407,   464,   399,   364,   364,   364
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int16 yyr1[] =
{
       0,   362,   363,   363,   364,   364,   365,   365,   366,   367,
     368,   369,   370,   371,   372,   373,   374,   375,   376,   377,
     378,   379,   380,   381,   382,   383,   384,   385,   386,   387,
     388,   389,   390,   391,   392,   393,   394,   395,   396,   397,
     398,   399,   400,   401,   402,   403,   404,   405,   406,   407,
     408,   409,   410,   411,   412,   413,   414,   415,   416,   417,
     417,   417,   417,   417,   418,   418,   418,   418,   419,   419,
     419,   419,   419,   419,   419,   419,   419,   419,   419,   419,
     419,   419,   419,   420,   420,   420,   420,   420,   420,   420,
     420,   420,   420,   420,   420,   420,   420,   420,   420,   420,
     420,   420,   420,   420,   420,   420,   420,   421,   422,   422,
     422,   422,   422,   422,   422,   422,   422,   422,   422,   422,
     422,   422,   422,   422,   422,   423,   424,   424,   425,   425,
     426,   426,   427,   427,   428,   428,   428,   428,   428,   428,
     428,   428,   428,   428,   428,   428,   428,   428,   428,   429,
     430,   430,   430,   430,   430,   430,   430,   430,   430,   430,
     430,   430,   430,   430,   430,   430,   430,   430,   430,   430,
     430,   430,   430,   430,   430,   430,   430,   430,   430,   430,
     430,   430,   430,   430,   431,   432,   432,   433,   433,   433,
     433,   433,   433,   434,   434,   434,   434,   434,   434,   434,
     435,   436,   436,   437,   437,   438,   439,   439,   440,   440,
     440,   440,   440,   440,   440,   440,   441,   441,   442,   442,
     442,   443,   444,   445,   445,   446,   446,   446,   446,   446,
     446,   446,   446,   446,   446,   446,   446,   446,   446,   446,
     446,   447,   448,   448,   448,   448,   448,   448,   448,   448,
     449,   449,   449,   450,   450,   451,   451,   452,   453,   453,
     453,   453,   453,   453,   453,   453,   453,   453,   453,   453,
     453,   454,   454,   455,   455,   456,   456,   456,   456,   457,
     458,   458,   458,   458,   458,   459,   459,   459,   459,   459,
     459,   459,   459,   459,   459,   459,   459,   459,   459,   460,
     460,   461,   461,   462,   463,   463,   463,   463,   463,   463,
     463,   464,   464,   465,   465,   465,   466,   466,   466,   467,
     467,   468,   468,   469,   470,   470,   470,   470,   470,   471,
     471,   471,   471,   471,   472,   472,   472,   472,   472,   473,
     473,   473,   473,   473,   474,   474,   474,   474,   474,   475,
     475,   475,   475,   475,   476,   476,   476,   476,   476,   477,
     477,   477,   477,   477,   478,   478,   478,   478,   478,   479,
     480,   481,   481,   482,   482,   482,   482,   482,   483,   483,
     484,   484,   484,   484,   485,   486,   487,   487,   488,   488,
     489,   490,   490,   490,   491,   492,   492,   493,   493,   493,
     493,   493,   493,   494,   495,   496,   497,   498,   499,   500,
     501,   502,   503,   504,   505,   506,   507,   508,   509,   510,
     511,   511,   511,   511,   512,   513,   514,   514,   515,   515,
     516,   517,   517,   518,   518,   519,   519,   519,   519,   520,
     520,   520,   520,   520,   520,   520,   520,   520,   520,   520,
     520,   520,   520,   520,   520,   520,   520,   520,   520,   520,
     521,   521,   522,   523,   523,   524,   525,   525,   526,   526,
     526,   527,   528,   528,   529,   529,   530,   530,   531,   531,
     532,   532,   533,   533,   534,   535,   535,   536,   536,   536,
     536,   536,   536,   537,   538,   538,   538,   538,   538,   539,
     539,   540,   540,   541,   541,   541,   542,   542,   542,   543,
     543,   544,   544,   545,   545,   546,   546,   546,   547,   547,
     548,   549,   549,   550,   550,   551,   551,   552,   552,   552,
     552,   552,   553,   554,   555,   555,   556,   556,   556,   556,
     556,   556,   556,   556,   557,   558,   558,   559,   559,   559,
     559,   559,   559,   560,   560,   561,   561,   562,   562,   563,
     563,   563,   563,   564,   564,   565,   566,   566,   567,   567,
     568,   568,   568,   568,   568,   568,   568,   568,   568,   568,
     568,   568,   568,   569,   569,   569,   570,   570,   571,   571,
     572,   572,   573,   574,   574,   575,   575,   576,   576,   577,
     578,   579,   579,   580,   580,   580,   581,   581,   581,   581,
     581,   581,   581,   581,   581,   581,   581,   581,   581,   581,
     582,   582,   583,   584,   584,   584,   585,   585,   586,   587,
     587,   587,   587,   587,   588,   588,   589,   589,   590,   590,
     591,   591,   591,   592,   592,   592,   592,   593,   593,   594,
     595,   595,   596,   596,   597,   597,   598,   598,   598,   599,
     599,   599,   599,   600,   600,   601,   601,   602,   602,   603,
     604,   604,   604,   605,   605,   605,   606,   606,   607,   607,
     608,   609,   609,   610,   611,   611,   612,   613,   614,   614,
     615,   615,   616,   617,   618,   618,   618,   618,   618,   618,
     618,   618,   618,   618,   618,   618,   618,   618,   618,   619,
     620,   620,   620,   621,   621,   621,   621,   621,   622,   622,
     623,   623,   624,   624,   625,   625,   625,   626,   626,   627,
     627,   628,   628,   629,   630,   630,   631,   632,   633,   633,
     634,   635,   635,   635,   636,   637,   637,   637,   638,   638,
     638,   639,   639,   640,   641,   641,   642,   642,   643,   643,
     644,   644,   645,   645,   645,   646,   646,   647,   647,   648,
     648,   648,   648,   648,   648,   649,   649,   649,   650,   651,
     651,   652,   652,   652,   652,   653,   654,   655,   656,   657,
     658,   659,   659,   659,   660,   660,   660,   661,   661,   662,
     662,   663,   663,   664,   665,   665,   665,   666,   666,   666,
     666,   666,   667,   668,   668,   669,   670,   670,   670,   670,
     670,   670,   670,   670,   671,   671,   672,   672,   672,   673,
     673,   673,   674,   674,   675,   675,   676,   676,   677,   678,
     678,   678,   678,   679,   679,   680,   681,   681,   681,   681,
     681,   681,   681,   681,   681,   681,   681,   681,   682,   682,
     682,   682,   682,   682,   682,   682,   682,   682,   682,   682,
     682,   682,   682,   682,   682,   683,   683,   683,   683,   683,
     683,   683,   684,   684,   684,   684,   684,   684,   685,   685,
     686,   686,   686,   687,   687,   688,   688,   688,   688,   688,
     689,   689,   689,   689,   689,   689,   689,   689,   689,   689,
     689,   689,   689,   689,   689,   689,   689,   689,   689,   689,
     689,   689,   689,   689,   690,   690,   690,   690,   690,   690,
     691,   691,   692,   692,   692,   693,   693,   694,   694,   695,
     696,   696,   697,   697,   698,   698,   699,   699,   700,   700,
     701,   701,   701,   702,   702,   703,   703,   704,   704,   704,
     704,   705,   705,   705,   706,   706,   707,   707,   707,   707,
     707,   707,   707,   707,   707,   707,   707,   707,   707,   707,
     707,   707,   707,   708,   708,   708,   708,   708,   708,   708,
     709,   709,   709,   709,   710,   710,   710,   710,   711,   711,
     712,   712,   713,   713,   713,   713,   714,   715,   715,   715,
     715,   715,   715,   715,   715,   715,   715,   715,   715,   715,
     715,   715,   715,   715,   715,   716,   717,   717,   717,   717,
     718,   719,   719,   719,   720,   721,   721,   721,   721,   721,
     722,   723,   723,   723,   723,   723,   723,   723,   723,   723,
     724,   724,   724,   725,   725,   725,   725,   725,   725,   725,
     725,   725,   725,   725,   725,   726,   726,   726,   726,   727,
     727,   727,   727,   727,   728,   729,   729,   729,   729,   729,
     729,   729,   730,   731,   731,   731,   731,   732,   733,   733,
     733,   733,   733,   733,   733,   733,   733,   734,   735,   735,
     735,   735,   735,   735,   735,   735,   736,   737,   737,   737,
     737,   737,   738,   739,   739,   740,   741,   741,   741,   742,
     743,   743,   743,   743,   744,   745,   745,   745,   745,   746,
     746,   746,   746,   747,   748,   748,   748,   748,   748,   748,
     748,   748,   748,   748,   749,   749,   749,   749,   749,   749,
     750,   750,   750,   750,   750,   751,   751,   751,   751,   751,
     751,   751,   751,   751,   751,   751,   751,   752,   752,   753,
     754,   754,   755,   756,   757,   758,   758,   758,   759,   759,
     759,   759,   759,   759,   759,   759,   759,   759,   759,   759,
     759,   759,   759,   759,   759,   759,   760,   760,   760,   761,
     761,   761,   761,   762,   762,   762,   762,   762,   763,   763,
     763,   763,   764,   764,   764,   764,   764,   764,   764,   764,
     764,   764,   764,   764,   765,   765,   766,   766,   766,   766,
     767,   768,   768,   769,   769,   769,   769,   769,   769,   769,
     769,   770,   771,   771,   772,   773,   773,   773,   773,   774,
     775,   776,   777,   778,   779,   779,   779,   779,   780,   781,
     781,   781,   781,   781,   781,   782,   783,   783,   784,   785,
     785,   785,   785,   785,   786,   787,   787,   787,   787,   787
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     3,
       5,     5,     3,     2,     1,     1,     2,     2,     1,     2,
       2,     2,     2,     2,     2,     3,     3,     2,     2,     3,
       3,     3,     2,     2,     6,     2,     6,     3,     2,     6,
       6,     3,     6,     3,     5,     7,     5,     7,     8,     8,
       8,     5,     7,     5,     7,     5,     7,     3,     2,     6,
       2,     6,     6,     6,     3,     6,     3,     5,     5,     8,
       8,     8,     5,     5,     5,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     2,     2,     2,     3,
       2,     2,     6,     3,     3,     5,     3,     3,     3,     3,
       2,     2,     2,     2,     2,     3,     2,     2,     3,     3,
       2,     3,     3,     2,     3,     3,     2,     3,     3,     2,
       3,     3,     2,     3,     3,     2,     2,     2,     2,     2,
       2,     4,     5,     2,     2,     1,     2,     2,     3,     3,
       2,     3,     3,     2,     2,     2,     2,     3,     2,     2,
       3,     2,     1,     2,     1,     3,     0,     1,     0,     1,
       1,     1,     1,     1,     1,     1,     0,     1,     1,     1,
       2,     1,     0,     3,     1,     0,     2,     2,     3,     8,
       8,     8,     8,     9,     9,    10,    10,    10,     9,     9,
       9,     0,     0,     2,     2,     3,     3,     3,     3,     3,
       0,     2,     3,     1,     3,     1,     3,     0,     0,     2,
       2,     5,     4,     5,     4,     4,     3,     4,     2,     3,
       3,     1,     1,     3,     1,     1,     1,     1,     1,     0,
       2,     2,     2,     2,     1,     0,     2,     2,     4,     7,
       8,     6,     7,     7,     5,     3,     4,     3,     3,     3,
       2,     1,     1,     0,     0,     2,     2,     5,     5,     3,
       5,     3,     1,     1,     3,     3,     1,     1,     1,     1,
       1,     1,     3,     0,     0,     2,     2,     2,     2,     0,
       2,     2,     2,     2,     0,     2,     2,     2,     2,     0,
       2,     2,     2,     2,     0,     2,     2,     2,     2,     0,
       2,     2,     2,     2,     0,     2,     2,     2,     2,     0,
       2,     2,     2,     2,     1,     1,     1,     1,     1,     7,
       2,     1,     1,     1,     1,     1,     3,     3,     1,     2,
       2,     2,     3,     0,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     1,     2,     2,     1,
       2,     1,     1,     2,     3,     2,     3,     1,     2,     3,
       1,     2,     3,     1,     2,     3,     1,     2,     2,     2,
       1,     2,     2,     2,     2,     2,     0,     1,     1,     2,
       1,     1,     2,     1,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     2,     2,     1,
       1,     2,     2,     2,     2,     1,     1,     2,     1,     1,
       1,     1,     5,     1,     1,     3,     3,     1,     1,     3,
       3,     5,     4,     5,     1,     2,     1,     3,     1,     2,
       2,     2,     1,     2,     1,     1,     2,     2,     2,     2,
       2,     2,     3,     1,     3,     3,     1,     2,     1,     3,
       1,     1,     1,     6,     6,     4,     1,     1,     0,     1,
       1,     0,     3,     6,     4,     1,     1,     0,     0,     3,
       3,     0,     2,     2,     3,     2,     2,     1,     1,     1,
       1,     1,     2,     1,     1,     1,     0,     6,     3,     6,
       3,     5,     3,     5,     2,     1,     1,     3,     4,     4,
       5,     6,     5,     1,     2,     1,     3,     1,     2,     2,
       2,     1,     1,     6,     8,     0,     0,     1,     0,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     3,     3,     1,     3,     1,     3,
       1,     3,     1,     1,     3,     1,     1,     3,     1,     3,
       3,     1,     1,     1,     1,     1,     1,     2,     3,     3,
       4,     5,     2,     3,     2,     6,     4,     3,     4,     3,
       2,     1,     1,     3,     4,     1,     2,     1,     1,     2,
       3,     1,     3,     4,     3,     5,     3,     6,     1,     3,
       1,     1,     1,     2,     4,     7,     7,     1,     2,     1,
       1,     2,     2,     1,     1,     1,     1,     1,     3,     1,
       1,     1,     1,     1,     3,     1,     1,     1,     2,     1,
       4,     5,     6,     1,     1,     1,     7,     8,     6,     1,
       1,     1,     2,     2,     6,     8,     1,     2,     1,     1,
       1,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     4,
       1,     1,     2,     1,     1,     1,     1,     1,     3,     1,
       4,     4,     0,     2,     1,     3,     3,     1,     3,     1,
       3,     1,     3,     1,     1,     3,     3,     3,     1,     1,
       3,     1,     1,     1,     3,     1,     3,     3,     3,     3,
       5,     1,     2,     1,     1,     2,     1,     1,     2,     1,
       1,     2,     2,     2,     1,     1,     2,     1,     2,     2,
       6,     6,     6,     4,     5,     2,     2,     1,     1,     1,
       1,     1,     1,     2,     2,     4,     0,     4,     0,     1,
       0,     1,     1,     1,     1,     1,     1,     2,     2,     6,
       3,     1,     3,     3,     3,     7,     3,     3,     3,     3,
       3,     3,     0,     4,     4,     0,     2,     2,     4,     4,
       5,     5,     3,     3,     3,     3,     1,     1,     1,     1,
       3,     3,     1,     3,     1,     3,     1,     3,     1,     1,
       1,     3,     3,     1,     1,     1,     2,     2,     2,     2,
       2,     2,     2,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     2,     2,     1,     2,
       2,     2,     2,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     1,     1,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     1,     1,     5,     3,     5,     1,     5,     5,
       3,     5,     1,     1,     1,     0,     2,     1,     1,     6,
       2,     0,     1,     1,     1,     1,     1,     1,     5,     6,
       8,     6,     5,     2,     2,     3,     4,     1,     1,     1,
       2,     3,     4,     4,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     3,     1,     1,     1,     1,     1,     1,
       3,     3,     5,     5,     5,     6,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     7,     1,     1,     2,     1,
       3,     1,     1,     2,     3,     1,     1,     1,     1,     2,
       3,     1,     1,     1,     1,     1,     3,     3,     3,     3,
       3,     5,     4,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     3,     2,     1,     1,     1,     1,
       1,     1,     3,     2,     1,     1,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     2,     1,
       1,     1,     1,     1,     1,     1,     3,     1,     1,     1,
       1,     1,     3,     1,     1,     3,     1,     2,     1,     3,
       1,     1,     1,     1,     3,     1,     1,     1,     1,     2,
       3,     3,     9,     5,     1,     1,     1,     1,     1,     1,
       2,     1,     1,     1,     2,     2,     2,     2,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     3,     5,     1,     1,     1,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     1,     3,     1,     1,     2,     1,     3,
       4,     3,     1,     3,     1,     2,     1,     4,     3,     1,
       1,     1,     1,     1,     1,     3,     1,     1,     3,     1,
       1,     2,     1,     1,     2,     2,     2,     2,     2,     2
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, nft, scanner, state, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YY_LOCATION_PRINT
#  if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#   define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

#  else
#   define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#  endif
# endif /* !defined YY_LOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, nft, scanner, state); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, struct nft_ctx *nft, void *scanner, struct parser_state *state)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (nft);
  YY_USE (scanner);
  YY_USE (state);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, struct nft_ctx *nft, void *scanner, struct parser_state *state)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YY_LOCATION_PRINT (yyo, *yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, nft, scanner, state);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, struct nft_ctx *nft, void *scanner, struct parser_state *state)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), nft, scanner, state);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, nft, scanner, state); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, struct nft_ctx *nft, void *scanner, struct parser_state *state)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (nft);
  YY_USE (scanner);
  YY_USE (state);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yykind)
    {
    case YYSYMBOL_STRING: /* "string"  */
#line 320 "parser_bison.y"
            { xfree(((*yyvaluep).string)); }
#line 5832 "parser_bison.c"
        break;

    case YYSYMBOL_QUOTED_STRING: /* "quoted string"  */
#line 320 "parser_bison.y"
            { xfree(((*yyvaluep).string)); }
#line 5838 "parser_bison.c"
        break;

    case YYSYMBOL_ASTERISK_STRING: /* "string with a trailing asterisk"  */
#line 320 "parser_bison.y"
            { xfree(((*yyvaluep).string)); }
#line 5844 "parser_bison.c"
        break;

    case YYSYMBOL_line: /* line  */
#line 630 "parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 5850 "parser_bison.c"
        break;

    case YYSYMBOL_base_cmd: /* base_cmd  */
#line 633 "parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 5856 "parser_bison.c"
        break;

    case YYSYMBOL_add_cmd: /* add_cmd  */
#line 633 "parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 5862 "parser_bison.c"
        break;

    case YYSYMBOL_replace_cmd: /* replace_cmd  */
#line 633 "parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 5868 "parser_bison.c"
        break;

    case YYSYMBOL_create_cmd: /* create_cmd  */
#line 633 "parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 5874 "parser_bison.c"
        break;

    case YYSYMBOL_insert_cmd: /* insert_cmd  */
#line 633 "parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 5880 "parser_bison.c"
        break;

    case YYSYMBOL_table_or_id_spec: /* table_or_id_spec  */
#line 636 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 5886 "parser_bison.c"
        break;

    case YYSYMBOL_chain_or_id_spec: /* chain_or_id_spec  */
#line 638 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 5892 "parser_bison.c"
        break;

    case YYSYMBOL_set_or_id_spec: /* set_or_id_spec  */
#line 643 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 5898 "parser_bison.c"
        break;

    case YYSYMBOL_obj_or_id_spec: /* obj_or_id_spec  */
#line 645 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 5904 "parser_bison.c"
        break;

    case YYSYMBOL_delete_cmd: /* delete_cmd  */
#line 633 "parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 5910 "parser_bison.c"
        break;

    case YYSYMBOL_get_cmd: /* get_cmd  */
#line 633 "parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 5916 "parser_bison.c"
        break;

    case YYSYMBOL_list_cmd: /* list_cmd  */
#line 633 "parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 5922 "parser_bison.c"
        break;

    case YYSYMBOL_basehook_device_name: /* basehook_device_name  */
#line 657 "parser_bison.y"
            { xfree(((*yyvaluep).string)); }
#line 5928 "parser_bison.c"
        break;

    case YYSYMBOL_basehook_spec: /* basehook_spec  */
#line 651 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 5934 "parser_bison.c"
        break;

    case YYSYMBOL_reset_cmd: /* reset_cmd  */
#line 633 "parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 5940 "parser_bison.c"
        break;

    case YYSYMBOL_flush_cmd: /* flush_cmd  */
#line 633 "parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 5946 "parser_bison.c"
        break;

    case YYSYMBOL_rename_cmd: /* rename_cmd  */
#line 633 "parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 5952 "parser_bison.c"
        break;

    case YYSYMBOL_import_cmd: /* import_cmd  */
#line 633 "parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 5958 "parser_bison.c"
        break;

    case YYSYMBOL_export_cmd: /* export_cmd  */
#line 633 "parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 5964 "parser_bison.c"
        break;

    case YYSYMBOL_monitor_cmd: /* monitor_cmd  */
#line 633 "parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 5970 "parser_bison.c"
        break;

    case YYSYMBOL_monitor_event: /* monitor_event  */
#line 876 "parser_bison.y"
            { xfree(((*yyvaluep).string)); }
#line 5976 "parser_bison.c"
        break;

    case YYSYMBOL_describe_cmd: /* describe_cmd  */
#line 633 "parser_bison.y"
            { cmd_free(((*yyvaluep).cmd)); }
#line 5982 "parser_bison.c"
        break;

    case YYSYMBOL_table_block_alloc: /* table_block_alloc  */
#line 663 "parser_bison.y"
            { close_scope(state); table_free(((*yyvaluep).table)); }
#line 5988 "parser_bison.c"
        break;

    case YYSYMBOL_chain_block_alloc: /* chain_block_alloc  */
#line 665 "parser_bison.y"
            { close_scope(state); chain_free(((*yyvaluep).chain)); }
#line 5994 "parser_bison.c"
        break;

    case YYSYMBOL_typeof_data_expr: /* typeof_data_expr  */
#line 737 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6000 "parser_bison.c"
        break;

    case YYSYMBOL_typeof_expr: /* typeof_expr  */
#line 737 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6006 "parser_bison.c"
        break;

    case YYSYMBOL_set_block_alloc: /* set_block_alloc  */
#line 674 "parser_bison.y"
            { set_free(((*yyvaluep).set)); }
#line 6012 "parser_bison.c"
        break;

    case YYSYMBOL_set_block_expr: /* set_block_expr  */
#line 778 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6018 "parser_bison.c"
        break;

    case YYSYMBOL_map_block_alloc: /* map_block_alloc  */
#line 677 "parser_bison.y"
            { set_free(((*yyvaluep).set)); }
#line 6024 "parser_bison.c"
        break;

    case YYSYMBOL_flowtable_block_alloc: /* flowtable_block_alloc  */
#line 681 "parser_bison.y"
            { flowtable_free(((*yyvaluep).flowtable)); }
#line 6030 "parser_bison.c"
        break;

    case YYSYMBOL_flowtable_expr: /* flowtable_expr  */
#line 778 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6036 "parser_bison.c"
        break;

    case YYSYMBOL_flowtable_list_expr: /* flowtable_list_expr  */
#line 778 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6042 "parser_bison.c"
        break;

    case YYSYMBOL_flowtable_expr_member: /* flowtable_expr_member  */
#line 778 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6048 "parser_bison.c"
        break;

    case YYSYMBOL_data_type_atom_expr: /* data_type_atom_expr  */
#line 627 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6054 "parser_bison.c"
        break;

    case YYSYMBOL_data_type_expr: /* data_type_expr  */
#line 627 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6060 "parser_bison.c"
        break;

    case YYSYMBOL_obj_block_alloc: /* obj_block_alloc  */
#line 684 "parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 6066 "parser_bison.c"
        break;

    case YYSYMBOL_type_identifier: /* type_identifier  */
#line 622 "parser_bison.y"
            { xfree(((*yyvaluep).string)); }
#line 6072 "parser_bison.c"
        break;

    case YYSYMBOL_extended_prio_name: /* extended_prio_name  */
#line 657 "parser_bison.y"
            { xfree(((*yyvaluep).string)); }
#line 6078 "parser_bison.c"
        break;

    case YYSYMBOL_dev_spec: /* dev_spec  */
#line 660 "parser_bison.y"
            { xfree(((*yyvaluep).expr)); }
#line 6084 "parser_bison.c"
        break;

    case YYSYMBOL_policy_expr: /* policy_expr  */
#line 735 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6090 "parser_bison.c"
        break;

    case YYSYMBOL_identifier: /* identifier  */
#line 622 "parser_bison.y"
            { xfree(((*yyvaluep).string)); }
#line 6096 "parser_bison.c"
        break;

    case YYSYMBOL_string: /* string  */
#line 622 "parser_bison.y"
            { xfree(((*yyvaluep).string)); }
#line 6102 "parser_bison.c"
        break;

    case YYSYMBOL_table_spec: /* table_spec  */
#line 636 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6108 "parser_bison.c"
        break;

    case YYSYMBOL_tableid_spec: /* tableid_spec  */
#line 636 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6114 "parser_bison.c"
        break;

    case YYSYMBOL_chain_spec: /* chain_spec  */
#line 638 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6120 "parser_bison.c"
        break;

    case YYSYMBOL_chainid_spec: /* chainid_spec  */
#line 638 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6126 "parser_bison.c"
        break;

    case YYSYMBOL_chain_identifier: /* chain_identifier  */
#line 641 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6132 "parser_bison.c"
        break;

    case YYSYMBOL_set_spec: /* set_spec  */
#line 643 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6138 "parser_bison.c"
        break;

    case YYSYMBOL_setid_spec: /* setid_spec  */
#line 643 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6144 "parser_bison.c"
        break;

    case YYSYMBOL_set_identifier: /* set_identifier  */
#line 648 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6150 "parser_bison.c"
        break;

    case YYSYMBOL_flowtable_spec: /* flowtable_spec  */
#line 641 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6156 "parser_bison.c"
        break;

    case YYSYMBOL_flowtableid_spec: /* flowtableid_spec  */
#line 648 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6162 "parser_bison.c"
        break;

    case YYSYMBOL_obj_spec: /* obj_spec  */
#line 645 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6168 "parser_bison.c"
        break;

    case YYSYMBOL_objid_spec: /* objid_spec  */
#line 645 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6174 "parser_bison.c"
        break;

    case YYSYMBOL_obj_identifier: /* obj_identifier  */
#line 648 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6180 "parser_bison.c"
        break;

    case YYSYMBOL_handle_spec: /* handle_spec  */
#line 641 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6186 "parser_bison.c"
        break;

    case YYSYMBOL_position_spec: /* position_spec  */
#line 641 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6192 "parser_bison.c"
        break;

    case YYSYMBOL_index_spec: /* index_spec  */
#line 641 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6198 "parser_bison.c"
        break;

    case YYSYMBOL_rule_position: /* rule_position  */
#line 641 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6204 "parser_bison.c"
        break;

    case YYSYMBOL_ruleid_spec: /* ruleid_spec  */
#line 641 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6210 "parser_bison.c"
        break;

    case YYSYMBOL_comment_spec: /* comment_spec  */
#line 622 "parser_bison.y"
            { xfree(((*yyvaluep).string)); }
#line 6216 "parser_bison.c"
        break;

    case YYSYMBOL_ruleset_spec: /* ruleset_spec  */
#line 641 "parser_bison.y"
            { handle_free(&((*yyvaluep).handle)); }
#line 6222 "parser_bison.c"
        break;

    case YYSYMBOL_rule: /* rule  */
#line 667 "parser_bison.y"
            { rule_free(((*yyvaluep).rule)); }
#line 6228 "parser_bison.c"
        break;

    case YYSYMBOL_stmt_list: /* stmt_list  */
#line 687 "parser_bison.y"
            { stmt_list_free(((*yyvaluep).list)); xfree(((*yyvaluep).list)); }
#line 6234 "parser_bison.c"
        break;

    case YYSYMBOL_stateful_stmt_list: /* stateful_stmt_list  */
#line 687 "parser_bison.y"
            { stmt_list_free(((*yyvaluep).list)); xfree(((*yyvaluep).list)); }
#line 6240 "parser_bison.c"
        break;

    case YYSYMBOL_stateful_stmt: /* stateful_stmt  */
#line 691 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6246 "parser_bison.c"
        break;

    case YYSYMBOL_stmt: /* stmt  */
#line 689 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6252 "parser_bison.c"
        break;

    case YYSYMBOL_chain_stmt: /* chain_stmt  */
#line 714 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6258 "parser_bison.c"
        break;

    case YYSYMBOL_verdict_stmt: /* verdict_stmt  */
#line 689 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6264 "parser_bison.c"
        break;

    case YYSYMBOL_verdict_map_stmt: /* verdict_map_stmt  */
#line 772 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6270 "parser_bison.c"
        break;

    case YYSYMBOL_verdict_map_expr: /* verdict_map_expr  */
#line 775 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6276 "parser_bison.c"
        break;

    case YYSYMBOL_verdict_map_list_expr: /* verdict_map_list_expr  */
#line 775 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6282 "parser_bison.c"
        break;

    case YYSYMBOL_verdict_map_list_member_expr: /* verdict_map_list_member_expr  */
#line 775 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6288 "parser_bison.c"
        break;

    case YYSYMBOL_connlimit_stmt: /* connlimit_stmt  */
#line 702 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6294 "parser_bison.c"
        break;

    case YYSYMBOL_counter_stmt: /* counter_stmt  */
#line 691 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6300 "parser_bison.c"
        break;

    case YYSYMBOL_counter_stmt_alloc: /* counter_stmt_alloc  */
#line 691 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6306 "parser_bison.c"
        break;

    case YYSYMBOL_log_stmt: /* log_stmt  */
#line 699 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6312 "parser_bison.c"
        break;

    case YYSYMBOL_log_stmt_alloc: /* log_stmt_alloc  */
#line 699 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6318 "parser_bison.c"
        break;

    case YYSYMBOL_limit_stmt: /* limit_stmt  */
#line 702 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6324 "parser_bison.c"
        break;

    case YYSYMBOL_quota_unit: /* quota_unit  */
#line 657 "parser_bison.y"
            { xfree(((*yyvaluep).string)); }
#line 6330 "parser_bison.c"
        break;

    case YYSYMBOL_quota_stmt: /* quota_stmt  */
#line 702 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6336 "parser_bison.c"
        break;

    case YYSYMBOL_reject_stmt: /* reject_stmt  */
#line 705 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6342 "parser_bison.c"
        break;

    case YYSYMBOL_reject_stmt_alloc: /* reject_stmt_alloc  */
#line 705 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6348 "parser_bison.c"
        break;

    case YYSYMBOL_reject_with_expr: /* reject_with_expr  */
#line 720 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6354 "parser_bison.c"
        break;

    case YYSYMBOL_nat_stmt: /* nat_stmt  */
#line 707 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6360 "parser_bison.c"
        break;

    case YYSYMBOL_nat_stmt_alloc: /* nat_stmt_alloc  */
#line 707 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6366 "parser_bison.c"
        break;

    case YYSYMBOL_tproxy_stmt: /* tproxy_stmt  */
#line 710 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6372 "parser_bison.c"
        break;

    case YYSYMBOL_synproxy_stmt: /* synproxy_stmt  */
#line 712 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6378 "parser_bison.c"
        break;

    case YYSYMBOL_synproxy_stmt_alloc: /* synproxy_stmt_alloc  */
#line 712 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6384 "parser_bison.c"
        break;

    case YYSYMBOL_synproxy_obj: /* synproxy_obj  */
#line 798 "parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 6390 "parser_bison.c"
        break;

    case YYSYMBOL_primary_stmt_expr: /* primary_stmt_expr  */
#line 759 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6396 "parser_bison.c"
        break;

    case YYSYMBOL_shift_stmt_expr: /* shift_stmt_expr  */
#line 761 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6402 "parser_bison.c"
        break;

    case YYSYMBOL_and_stmt_expr: /* and_stmt_expr  */
#line 763 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6408 "parser_bison.c"
        break;

    case YYSYMBOL_exclusive_or_stmt_expr: /* exclusive_or_stmt_expr  */
#line 763 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6414 "parser_bison.c"
        break;

    case YYSYMBOL_inclusive_or_stmt_expr: /* inclusive_or_stmt_expr  */
#line 763 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6420 "parser_bison.c"
        break;

    case YYSYMBOL_basic_stmt_expr: /* basic_stmt_expr  */
#line 759 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6426 "parser_bison.c"
        break;

    case YYSYMBOL_concat_stmt_expr: /* concat_stmt_expr  */
#line 751 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6432 "parser_bison.c"
        break;

    case YYSYMBOL_map_stmt_expr_set: /* map_stmt_expr_set  */
#line 751 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6438 "parser_bison.c"
        break;

    case YYSYMBOL_map_stmt_expr: /* map_stmt_expr  */
#line 751 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6444 "parser_bison.c"
        break;

    case YYSYMBOL_prefix_stmt_expr: /* prefix_stmt_expr  */
#line 756 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6450 "parser_bison.c"
        break;

    case YYSYMBOL_range_stmt_expr: /* range_stmt_expr  */
#line 756 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6456 "parser_bison.c"
        break;

    case YYSYMBOL_multiton_stmt_expr: /* multiton_stmt_expr  */
#line 754 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6462 "parser_bison.c"
        break;

    case YYSYMBOL_stmt_expr: /* stmt_expr  */
#line 751 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6468 "parser_bison.c"
        break;

    case YYSYMBOL_masq_stmt: /* masq_stmt  */
#line 707 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6474 "parser_bison.c"
        break;

    case YYSYMBOL_masq_stmt_alloc: /* masq_stmt_alloc  */
#line 707 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6480 "parser_bison.c"
        break;

    case YYSYMBOL_redir_stmt: /* redir_stmt  */
#line 707 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6486 "parser_bison.c"
        break;

    case YYSYMBOL_redir_stmt_alloc: /* redir_stmt_alloc  */
#line 707 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6492 "parser_bison.c"
        break;

    case YYSYMBOL_dup_stmt: /* dup_stmt  */
#line 723 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6498 "parser_bison.c"
        break;

    case YYSYMBOL_fwd_stmt: /* fwd_stmt  */
#line 725 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6504 "parser_bison.c"
        break;

    case YYSYMBOL_queue_stmt: /* queue_stmt  */
#line 718 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6510 "parser_bison.c"
        break;

    case YYSYMBOL_queue_stmt_compat: /* queue_stmt_compat  */
#line 718 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6516 "parser_bison.c"
        break;

    case YYSYMBOL_queue_stmt_alloc: /* queue_stmt_alloc  */
#line 718 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6522 "parser_bison.c"
        break;

    case YYSYMBOL_queue_expr: /* queue_expr  */
#line 720 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6528 "parser_bison.c"
        break;

    case YYSYMBOL_queue_stmt_expr_simple: /* queue_stmt_expr_simple  */
#line 720 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6534 "parser_bison.c"
        break;

    case YYSYMBOL_queue_stmt_expr: /* queue_stmt_expr  */
#line 720 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6540 "parser_bison.c"
        break;

    case YYSYMBOL_set_elem_expr_stmt: /* set_elem_expr_stmt  */
#line 782 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6546 "parser_bison.c"
        break;

    case YYSYMBOL_set_elem_expr_stmt_alloc: /* set_elem_expr_stmt_alloc  */
#line 782 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6552 "parser_bison.c"
        break;

    case YYSYMBOL_set_stmt: /* set_stmt  */
#line 727 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6558 "parser_bison.c"
        break;

    case YYSYMBOL_map_stmt: /* map_stmt  */
#line 730 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6564 "parser_bison.c"
        break;

    case YYSYMBOL_meter_stmt: /* meter_stmt  */
#line 732 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6570 "parser_bison.c"
        break;

    case YYSYMBOL_flow_stmt_legacy_alloc: /* flow_stmt_legacy_alloc  */
#line 732 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6576 "parser_bison.c"
        break;

    case YYSYMBOL_meter_stmt_alloc: /* meter_stmt_alloc  */
#line 732 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6582 "parser_bison.c"
        break;

    case YYSYMBOL_match_stmt: /* match_stmt  */
#line 689 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6588 "parser_bison.c"
        break;

    case YYSYMBOL_variable_expr: /* variable_expr  */
#line 735 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6594 "parser_bison.c"
        break;

    case YYSYMBOL_symbol_expr: /* symbol_expr  */
#line 735 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6600 "parser_bison.c"
        break;

    case YYSYMBOL_set_ref_expr: /* set_ref_expr  */
#line 743 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6606 "parser_bison.c"
        break;

    case YYSYMBOL_set_ref_symbol_expr: /* set_ref_symbol_expr  */
#line 743 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6612 "parser_bison.c"
        break;

    case YYSYMBOL_integer_expr: /* integer_expr  */
#line 735 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6618 "parser_bison.c"
        break;

    case YYSYMBOL_primary_expr: /* primary_expr  */
#line 737 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6624 "parser_bison.c"
        break;

    case YYSYMBOL_fib_expr: /* fib_expr  */
#line 867 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6630 "parser_bison.c"
        break;

    case YYSYMBOL_osf_expr: /* osf_expr  */
#line 872 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6636 "parser_bison.c"
        break;

    case YYSYMBOL_shift_expr: /* shift_expr  */
#line 737 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6642 "parser_bison.c"
        break;

    case YYSYMBOL_and_expr: /* and_expr  */
#line 737 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6648 "parser_bison.c"
        break;

    case YYSYMBOL_exclusive_or_expr: /* exclusive_or_expr  */
#line 739 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6654 "parser_bison.c"
        break;

    case YYSYMBOL_inclusive_or_expr: /* inclusive_or_expr  */
#line 739 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6660 "parser_bison.c"
        break;

    case YYSYMBOL_basic_expr: /* basic_expr  */
#line 741 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6666 "parser_bison.c"
        break;

    case YYSYMBOL_concat_expr: /* concat_expr  */
#line 766 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6672 "parser_bison.c"
        break;

    case YYSYMBOL_prefix_rhs_expr: /* prefix_rhs_expr  */
#line 748 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6678 "parser_bison.c"
        break;

    case YYSYMBOL_range_rhs_expr: /* range_rhs_expr  */
#line 748 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6684 "parser_bison.c"
        break;

    case YYSYMBOL_multiton_rhs_expr: /* multiton_rhs_expr  */
#line 746 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6690 "parser_bison.c"
        break;

    case YYSYMBOL_map_expr: /* map_expr  */
#line 769 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6696 "parser_bison.c"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 788 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6702 "parser_bison.c"
        break;

    case YYSYMBOL_set_expr: /* set_expr  */
#line 778 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6708 "parser_bison.c"
        break;

    case YYSYMBOL_set_list_expr: /* set_list_expr  */
#line 778 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6714 "parser_bison.c"
        break;

    case YYSYMBOL_set_list_member_expr: /* set_list_member_expr  */
#line 778 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6720 "parser_bison.c"
        break;

    case YYSYMBOL_meter_key_expr: /* meter_key_expr  */
#line 785 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6726 "parser_bison.c"
        break;

    case YYSYMBOL_meter_key_expr_alloc: /* meter_key_expr_alloc  */
#line 785 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6732 "parser_bison.c"
        break;

    case YYSYMBOL_set_elem_expr: /* set_elem_expr  */
#line 780 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6738 "parser_bison.c"
        break;

    case YYSYMBOL_set_elem_key_expr: /* set_elem_key_expr  */
#line 910 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6744 "parser_bison.c"
        break;

    case YYSYMBOL_set_elem_expr_alloc: /* set_elem_expr_alloc  */
#line 780 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6750 "parser_bison.c"
        break;

    case YYSYMBOL_set_elem_stmt_list: /* set_elem_stmt_list  */
#line 687 "parser_bison.y"
            { stmt_list_free(((*yyvaluep).list)); xfree(((*yyvaluep).list)); }
#line 6756 "parser_bison.c"
        break;

    case YYSYMBOL_set_elem_stmt: /* set_elem_stmt  */
#line 689 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6762 "parser_bison.c"
        break;

    case YYSYMBOL_set_lhs_expr: /* set_lhs_expr  */
#line 780 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6768 "parser_bison.c"
        break;

    case YYSYMBOL_set_rhs_expr: /* set_rhs_expr  */
#line 780 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6774 "parser_bison.c"
        break;

    case YYSYMBOL_initializer_expr: /* initializer_expr  */
#line 788 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6780 "parser_bison.c"
        break;

    case YYSYMBOL_counter_obj: /* counter_obj  */
#line 798 "parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 6786 "parser_bison.c"
        break;

    case YYSYMBOL_quota_obj: /* quota_obj  */
#line 798 "parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 6792 "parser_bison.c"
        break;

    case YYSYMBOL_secmark_obj: /* secmark_obj  */
#line 798 "parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 6798 "parser_bison.c"
        break;

    case YYSYMBOL_timeout_states: /* timeout_states  */
#line 903 "parser_bison.y"
            { xfree(((*yyvaluep).list)); }
#line 6804 "parser_bison.c"
        break;

    case YYSYMBOL_timeout_state: /* timeout_state  */
#line 903 "parser_bison.y"
            { xfree(((*yyvaluep).list)); }
#line 6810 "parser_bison.c"
        break;

    case YYSYMBOL_ct_obj_alloc: /* ct_obj_alloc  */
#line 798 "parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 6816 "parser_bison.c"
        break;

    case YYSYMBOL_limit_obj: /* limit_obj  */
#line 798 "parser_bison.y"
            { obj_free(((*yyvaluep).obj)); }
#line 6822 "parser_bison.c"
        break;

    case YYSYMBOL_relational_expr: /* relational_expr  */
#line 801 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6828 "parser_bison.c"
        break;

    case YYSYMBOL_list_rhs_expr: /* list_rhs_expr  */
#line 793 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6834 "parser_bison.c"
        break;

    case YYSYMBOL_rhs_expr: /* rhs_expr  */
#line 791 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6840 "parser_bison.c"
        break;

    case YYSYMBOL_shift_rhs_expr: /* shift_rhs_expr  */
#line 793 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6846 "parser_bison.c"
        break;

    case YYSYMBOL_and_rhs_expr: /* and_rhs_expr  */
#line 795 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6852 "parser_bison.c"
        break;

    case YYSYMBOL_exclusive_or_rhs_expr: /* exclusive_or_rhs_expr  */
#line 795 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6858 "parser_bison.c"
        break;

    case YYSYMBOL_inclusive_or_rhs_expr: /* inclusive_or_rhs_expr  */
#line 795 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6864 "parser_bison.c"
        break;

    case YYSYMBOL_basic_rhs_expr: /* basic_rhs_expr  */
#line 791 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6870 "parser_bison.c"
        break;

    case YYSYMBOL_concat_rhs_expr: /* concat_rhs_expr  */
#line 791 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6876 "parser_bison.c"
        break;

    case YYSYMBOL_boolean_expr: /* boolean_expr  */
#line 893 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6882 "parser_bison.c"
        break;

    case YYSYMBOL_keyword_expr: /* keyword_expr  */
#line 788 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6888 "parser_bison.c"
        break;

    case YYSYMBOL_primary_rhs_expr: /* primary_rhs_expr  */
#line 793 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6894 "parser_bison.c"
        break;

    case YYSYMBOL_verdict_expr: /* verdict_expr  */
#line 735 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6900 "parser_bison.c"
        break;

    case YYSYMBOL_chain_expr: /* chain_expr  */
#line 735 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6906 "parser_bison.c"
        break;

    case YYSYMBOL_meta_expr: /* meta_expr  */
#line 849 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6912 "parser_bison.c"
        break;

    case YYSYMBOL_meta_stmt: /* meta_stmt  */
#line 697 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6918 "parser_bison.c"
        break;

    case YYSYMBOL_socket_expr: /* socket_expr  */
#line 853 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6924 "parser_bison.c"
        break;

    case YYSYMBOL_numgen_expr: /* numgen_expr  */
#line 814 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6930 "parser_bison.c"
        break;

    case YYSYMBOL_xfrm_expr: /* xfrm_expr  */
#line 907 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6936 "parser_bison.c"
        break;

    case YYSYMBOL_hash_expr: /* hash_expr  */
#line 814 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6942 "parser_bison.c"
        break;

    case YYSYMBOL_rt_expr: /* rt_expr  */
#line 859 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6948 "parser_bison.c"
        break;

    case YYSYMBOL_ct_expr: /* ct_expr  */
#line 863 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6954 "parser_bison.c"
        break;

    case YYSYMBOL_symbol_stmt_expr: /* symbol_stmt_expr  */
#line 793 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6960 "parser_bison.c"
        break;

    case YYSYMBOL_list_stmt_expr: /* list_stmt_expr  */
#line 761 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6966 "parser_bison.c"
        break;

    case YYSYMBOL_ct_stmt: /* ct_stmt  */
#line 695 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6972 "parser_bison.c"
        break;

    case YYSYMBOL_payload_stmt: /* payload_stmt  */
#line 693 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 6978 "parser_bison.c"
        break;

    case YYSYMBOL_payload_expr: /* payload_expr  */
#line 805 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6984 "parser_bison.c"
        break;

    case YYSYMBOL_payload_raw_expr: /* payload_raw_expr  */
#line 805 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6990 "parser_bison.c"
        break;

    case YYSYMBOL_eth_hdr_expr: /* eth_hdr_expr  */
#line 808 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 6996 "parser_bison.c"
        break;

    case YYSYMBOL_vlan_hdr_expr: /* vlan_hdr_expr  */
#line 808 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7002 "parser_bison.c"
        break;

    case YYSYMBOL_arp_hdr_expr: /* arp_hdr_expr  */
#line 811 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7008 "parser_bison.c"
        break;

    case YYSYMBOL_ip_hdr_expr: /* ip_hdr_expr  */
#line 814 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7014 "parser_bison.c"
        break;

    case YYSYMBOL_icmp_hdr_expr: /* icmp_hdr_expr  */
#line 814 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7020 "parser_bison.c"
        break;

    case YYSYMBOL_igmp_hdr_expr: /* igmp_hdr_expr  */
#line 814 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7026 "parser_bison.c"
        break;

    case YYSYMBOL_ip6_hdr_expr: /* ip6_hdr_expr  */
#line 818 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7032 "parser_bison.c"
        break;

    case YYSYMBOL_icmp6_hdr_expr: /* icmp6_hdr_expr  */
#line 818 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7038 "parser_bison.c"
        break;

    case YYSYMBOL_auth_hdr_expr: /* auth_hdr_expr  */
#line 821 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7044 "parser_bison.c"
        break;

    case YYSYMBOL_esp_hdr_expr: /* esp_hdr_expr  */
#line 821 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7050 "parser_bison.c"
        break;

    case YYSYMBOL_comp_hdr_expr: /* comp_hdr_expr  */
#line 821 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7056 "parser_bison.c"
        break;

    case YYSYMBOL_udp_hdr_expr: /* udp_hdr_expr  */
#line 824 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7062 "parser_bison.c"
        break;

    case YYSYMBOL_udplite_hdr_expr: /* udplite_hdr_expr  */
#line 824 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7068 "parser_bison.c"
        break;

    case YYSYMBOL_tcp_hdr_expr: /* tcp_hdr_expr  */
#line 882 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7074 "parser_bison.c"
        break;

    case YYSYMBOL_optstrip_stmt: /* optstrip_stmt  */
#line 890 "parser_bison.y"
            { stmt_free(((*yyvaluep).stmt)); }
#line 7080 "parser_bison.c"
        break;

    case YYSYMBOL_dccp_hdr_expr: /* dccp_hdr_expr  */
#line 827 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7086 "parser_bison.c"
        break;

    case YYSYMBOL_sctp_chunk_alloc: /* sctp_chunk_alloc  */
#line 827 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7092 "parser_bison.c"
        break;

    case YYSYMBOL_sctp_hdr_expr: /* sctp_hdr_expr  */
#line 827 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7098 "parser_bison.c"
        break;

    case YYSYMBOL_th_hdr_expr: /* th_hdr_expr  */
#line 833 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7104 "parser_bison.c"
        break;

    case YYSYMBOL_exthdr_expr: /* exthdr_expr  */
#line 837 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7110 "parser_bison.c"
        break;

    case YYSYMBOL_hbh_hdr_expr: /* hbh_hdr_expr  */
#line 839 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7116 "parser_bison.c"
        break;

    case YYSYMBOL_rt_hdr_expr: /* rt_hdr_expr  */
#line 842 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7122 "parser_bison.c"
        break;

    case YYSYMBOL_rt0_hdr_expr: /* rt0_hdr_expr  */
#line 842 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7128 "parser_bison.c"
        break;

    case YYSYMBOL_rt2_hdr_expr: /* rt2_hdr_expr  */
#line 842 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7134 "parser_bison.c"
        break;

    case YYSYMBOL_rt4_hdr_expr: /* rt4_hdr_expr  */
#line 842 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7140 "parser_bison.c"
        break;

    case YYSYMBOL_frag_hdr_expr: /* frag_hdr_expr  */
#line 839 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7146 "parser_bison.c"
        break;

    case YYSYMBOL_dst_hdr_expr: /* dst_hdr_expr  */
#line 839 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7152 "parser_bison.c"
        break;

    case YYSYMBOL_mh_hdr_expr: /* mh_hdr_expr  */
#line 845 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7158 "parser_bison.c"
        break;

    case YYSYMBOL_exthdr_exists_expr: /* exthdr_exists_expr  */
#line 897 "parser_bison.y"
            { expr_free(((*yyvaluep).expr)); }
#line 7164 "parser_bison.c"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (struct nft_ctx *nft, void *scanner, struct parser_state *state)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

/* User initialization code.  */
#line 159 "parser_bison.y"
{
	location_init(scanner, state, &yylloc);
	if (nft->debug_mask & NFT_DEBUG_SCANNER)
		nft_set_debug(1, scanner);
	if (nft->debug_mask & NFT_DEBUG_PARSER)
		yydebug = 1;
}

#line 7269 "parser_bison.c"

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= TOKEN_EOF)
    {
      yychar = TOKEN_EOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 3: /* input: input line  */
#line 916 "parser_bison.y"
                        {
				if ((yyvsp[0].cmd) != NULL) {
					(yyvsp[0].cmd)->location = (yylsp[0]);
					list_add_tail(&(yyvsp[0].cmd)->list, state->cmds);
				}
			}
#line 7486 "parser_bison.c"
    break;

  case 8: /* close_scope_ah: %empty  */
#line 932 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_AH); }
#line 7492 "parser_bison.c"
    break;

  case 9: /* close_scope_arp: %empty  */
#line 933 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_ARP); }
#line 7498 "parser_bison.c"
    break;

  case 10: /* close_scope_at: %empty  */
#line 934 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_AT); }
#line 7504 "parser_bison.c"
    break;

  case 11: /* close_scope_comp: %empty  */
#line 935 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_COMP); }
#line 7510 "parser_bison.c"
    break;

  case 12: /* close_scope_ct: %empty  */
#line 936 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CT); }
#line 7516 "parser_bison.c"
    break;

  case 13: /* close_scope_counter: %empty  */
#line 937 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_COUNTER); }
#line 7522 "parser_bison.c"
    break;

  case 14: /* close_scope_dccp: %empty  */
#line 938 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_DCCP); }
#line 7528 "parser_bison.c"
    break;

  case 15: /* close_scope_dst: %empty  */
#line 939 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_DST); }
#line 7534 "parser_bison.c"
    break;

  case 16: /* close_scope_dup: %empty  */
#line 940 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_DUP); }
#line 7540 "parser_bison.c"
    break;

  case 17: /* close_scope_esp: %empty  */
#line 941 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_ESP); }
#line 7546 "parser_bison.c"
    break;

  case 18: /* close_scope_eth: %empty  */
#line 942 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_ETH); }
#line 7552 "parser_bison.c"
    break;

  case 19: /* close_scope_export: %empty  */
#line 943 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CMD_EXPORT); }
#line 7558 "parser_bison.c"
    break;

  case 20: /* close_scope_fib: %empty  */
#line 944 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_FIB); }
#line 7564 "parser_bison.c"
    break;

  case 21: /* close_scope_flags: %empty  */
#line 945 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_FLAGS); }
#line 7570 "parser_bison.c"
    break;

  case 22: /* close_scope_frag: %empty  */
#line 946 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_FRAG); }
#line 7576 "parser_bison.c"
    break;

  case 23: /* close_scope_fwd: %empty  */
#line 947 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_FWD); }
#line 7582 "parser_bison.c"
    break;

  case 24: /* close_scope_hash: %empty  */
#line 948 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_HASH); }
#line 7588 "parser_bison.c"
    break;

  case 25: /* close_scope_hbh: %empty  */
#line 949 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_HBH); }
#line 7594 "parser_bison.c"
    break;

  case 26: /* close_scope_ip: %empty  */
#line 950 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_IP); }
#line 7600 "parser_bison.c"
    break;

  case 27: /* close_scope_ip6: %empty  */
#line 951 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_IP6); }
#line 7606 "parser_bison.c"
    break;

  case 28: /* close_scope_vlan: %empty  */
#line 952 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_VLAN); }
#line 7612 "parser_bison.c"
    break;

  case 29: /* close_scope_icmp: %empty  */
#line 953 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_ICMP); }
#line 7618 "parser_bison.c"
    break;

  case 30: /* close_scope_igmp: %empty  */
#line 954 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_IGMP); }
#line 7624 "parser_bison.c"
    break;

  case 31: /* close_scope_import: %empty  */
#line 955 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CMD_IMPORT); }
#line 7630 "parser_bison.c"
    break;

  case 32: /* close_scope_ipsec: %empty  */
#line 956 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_IPSEC); }
#line 7636 "parser_bison.c"
    break;

  case 33: /* close_scope_list: %empty  */
#line 957 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CMD_LIST); }
#line 7642 "parser_bison.c"
    break;

  case 34: /* close_scope_limit: %empty  */
#line 958 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_LIMIT); }
#line 7648 "parser_bison.c"
    break;

  case 35: /* close_scope_meta: %empty  */
#line 959 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_META); }
#line 7654 "parser_bison.c"
    break;

  case 36: /* close_scope_mh: %empty  */
#line 960 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_MH); }
#line 7660 "parser_bison.c"
    break;

  case 37: /* close_scope_monitor: %empty  */
#line 961 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CMD_MONITOR); }
#line 7666 "parser_bison.c"
    break;

  case 38: /* close_scope_nat: %empty  */
#line 962 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_NAT); }
#line 7672 "parser_bison.c"
    break;

  case 39: /* close_scope_numgen: %empty  */
#line 963 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_NUMGEN); }
#line 7678 "parser_bison.c"
    break;

  case 40: /* close_scope_osf: %empty  */
#line 964 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_OSF); }
#line 7684 "parser_bison.c"
    break;

  case 41: /* close_scope_policy: %empty  */
#line 965 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_POLICY); }
#line 7690 "parser_bison.c"
    break;

  case 42: /* close_scope_quota: %empty  */
#line 966 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_QUOTA); }
#line 7696 "parser_bison.c"
    break;

  case 43: /* close_scope_queue: %empty  */
#line 967 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_QUEUE); }
#line 7702 "parser_bison.c"
    break;

  case 44: /* close_scope_reject: %empty  */
#line 968 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_REJECT); }
#line 7708 "parser_bison.c"
    break;

  case 45: /* close_scope_reset: %empty  */
#line 969 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_CMD_RESET); }
#line 7714 "parser_bison.c"
    break;

  case 46: /* close_scope_rt: %empty  */
#line 970 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_RT); }
#line 7720 "parser_bison.c"
    break;

  case 47: /* close_scope_sctp: %empty  */
#line 971 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_SCTP); }
#line 7726 "parser_bison.c"
    break;

  case 48: /* close_scope_sctp_chunk: %empty  */
#line 972 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_SCTP_CHUNK); }
#line 7732 "parser_bison.c"
    break;

  case 49: /* close_scope_secmark: %empty  */
#line 973 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_SECMARK); }
#line 7738 "parser_bison.c"
    break;

  case 50: /* close_scope_socket: %empty  */
#line 974 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_SOCKET); }
#line 7744 "parser_bison.c"
    break;

  case 51: /* close_scope_tcp: %empty  */
#line 975 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_TCP); }
#line 7750 "parser_bison.c"
    break;

  case 52: /* close_scope_tproxy: %empty  */
#line 976 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_TPROXY); }
#line 7756 "parser_bison.c"
    break;

  case 53: /* close_scope_type: %empty  */
#line 977 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_TYPE); }
#line 7762 "parser_bison.c"
    break;

  case 54: /* close_scope_th: %empty  */
#line 978 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_TH); }
#line 7768 "parser_bison.c"
    break;

  case 55: /* close_scope_udp: %empty  */
#line 979 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_UDP); }
#line 7774 "parser_bison.c"
    break;

  case 56: /* close_scope_udplite: %empty  */
#line 980 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_EXPR_UDPLITE); }
#line 7780 "parser_bison.c"
    break;

  case 57: /* close_scope_log: %empty  */
#line 982 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_LOG); }
#line 7786 "parser_bison.c"
    break;

  case 58: /* close_scope_synproxy: %empty  */
#line 983 "parser_bison.y"
                          { scanner_pop_start_cond(nft->scanner, PARSER_SC_STMT_SYNPROXY); }
#line 7792 "parser_bison.c"
    break;

  case 59: /* common_block: "include" "quoted string" stmt_separator  */
#line 986 "parser_bison.y"
                        {
				if (scanner_include_file(nft, scanner, (yyvsp[-1].string), &(yyloc)) < 0) {
					xfree((yyvsp[-1].string));
					YYERROR;
				}
				xfree((yyvsp[-1].string));
			}
#line 7804 "parser_bison.c"
    break;

  case 60: /* common_block: "define" identifier '=' initializer_expr stmt_separator  */
#line 994 "parser_bison.y"
                        {
				struct scope *scope = current_scope(state);

				if (symbol_lookup(scope, (yyvsp[-3].string)) != NULL) {
					erec_queue(error(&(yylsp[-3]), "redefinition of symbol '%s'", (yyvsp[-3].string)),
						   state->msgs);
					expr_free((yyvsp[-1].expr));
					xfree((yyvsp[-3].string));
					YYERROR;
				}

				symbol_bind(scope, (yyvsp[-3].string), (yyvsp[-1].expr));
				xfree((yyvsp[-3].string));
			}
#line 7823 "parser_bison.c"
    break;

  case 61: /* common_block: "redefine" identifier '=' initializer_expr stmt_separator  */
#line 1009 "parser_bison.y"
                        {
				struct scope *scope = current_scope(state);

				symbol_bind(scope, (yyvsp[-3].string), (yyvsp[-1].expr));
				xfree((yyvsp[-3].string));
			}
#line 7834 "parser_bison.c"
    break;

  case 62: /* common_block: "undefine" identifier stmt_separator  */
#line 1016 "parser_bison.y"
                        {
				struct scope *scope = current_scope(state);

				if (symbol_unbind(scope, (yyvsp[-1].string)) < 0) {
					erec_queue(error(&(yylsp[-1]), "undefined symbol '%s'", (yyvsp[-1].string)),
						   state->msgs);
					xfree((yyvsp[-1].string));
					YYERROR;
				}
				xfree((yyvsp[-1].string));
			}
#line 7850 "parser_bison.c"
    break;

  case 63: /* common_block: error stmt_separator  */
#line 1028 "parser_bison.y"
                        {
				if (++state->nerrs == nft->parser_max_errors)
					YYABORT;
				yyerrok;
			}
#line 7860 "parser_bison.c"
    break;

  case 64: /* line: common_block  */
#line 1035 "parser_bison.y"
                                                                { (yyval.cmd) = NULL; }
#line 7866 "parser_bison.c"
    break;

  case 65: /* line: stmt_separator  */
#line 1036 "parser_bison.y"
                                                                { (yyval.cmd) = NULL; }
#line 7872 "parser_bison.c"
    break;

  case 66: /* line: base_cmd stmt_separator  */
#line 1037 "parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 7878 "parser_bison.c"
    break;

  case 67: /* line: base_cmd "end of file"  */
#line 1039 "parser_bison.y"
                        {
				/*
				 * Very hackish workaround for bison >= 2.4: previous versions
				 * terminated parsing after EOF, 2.4+ tries to get further input
				 * in 'input' and calls the scanner again, causing a crash when
				 * the final input buffer has been popped. Terminate manually to
				 * avoid this. The correct fix should be to adjust the grammar
				 * to accept EOF in input, but for unknown reasons it does not
				 * work.
				 */
				if ((yyvsp[-1].cmd) != NULL) {
					(yyvsp[-1].cmd)->location = (yylsp[-1]);
					list_add_tail(&(yyvsp[-1].cmd)->list, state->cmds);
				}
				(yyval.cmd) = NULL;
				YYACCEPT;
			}
#line 7900 "parser_bison.c"
    break;

  case 68: /* base_cmd: add_cmd  */
#line 1058 "parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 7906 "parser_bison.c"
    break;

  case 69: /* base_cmd: "add" add_cmd  */
#line 1059 "parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 7912 "parser_bison.c"
    break;

  case 70: /* base_cmd: "replace" replace_cmd  */
#line 1060 "parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 7918 "parser_bison.c"
    break;

  case 71: /* base_cmd: "create" create_cmd  */
#line 1061 "parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 7924 "parser_bison.c"
    break;

  case 72: /* base_cmd: "insert" insert_cmd  */
#line 1062 "parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 7930 "parser_bison.c"
    break;

  case 73: /* base_cmd: "delete" delete_cmd  */
#line 1063 "parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 7936 "parser_bison.c"
    break;

  case 74: /* base_cmd: "get" get_cmd  */
#line 1064 "parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 7942 "parser_bison.c"
    break;

  case 75: /* base_cmd: "list" list_cmd close_scope_list  */
#line 1065 "parser_bison.y"
                                                                                        { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 7948 "parser_bison.c"
    break;

  case 76: /* base_cmd: "reset" reset_cmd close_scope_reset  */
#line 1066 "parser_bison.y"
                                                                                        { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 7954 "parser_bison.c"
    break;

  case 77: /* base_cmd: "flush" flush_cmd  */
#line 1067 "parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 7960 "parser_bison.c"
    break;

  case 78: /* base_cmd: "rename" rename_cmd  */
#line 1068 "parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 7966 "parser_bison.c"
    break;

  case 79: /* base_cmd: "import" import_cmd close_scope_import  */
#line 1069 "parser_bison.y"
                                                                                        { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 7972 "parser_bison.c"
    break;

  case 80: /* base_cmd: "export" export_cmd close_scope_export  */
#line 1070 "parser_bison.y"
                                                                                        { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 7978 "parser_bison.c"
    break;

  case 81: /* base_cmd: "monitor" monitor_cmd close_scope_monitor  */
#line 1071 "parser_bison.y"
                                                                                        { (yyval.cmd) = (yyvsp[-1].cmd); }
#line 7984 "parser_bison.c"
    break;

  case 82: /* base_cmd: "describe" describe_cmd  */
#line 1072 "parser_bison.y"
                                                                { (yyval.cmd) = (yyvsp[0].cmd); }
#line 7990 "parser_bison.c"
    break;

  case 83: /* add_cmd: "table" table_spec  */
#line 1076 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 7998 "parser_bison.c"
    break;

  case 84: /* add_cmd: "table" table_spec table_block_alloc '{' table_block '}'  */
#line 1081 "parser_bison.y"
                        {
				handle_merge(&(yyvsp[-3].table)->handle, &(yyvsp[-4].handle));
				close_scope(state);
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_TABLE, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].table));
			}
#line 8008 "parser_bison.c"
    break;

  case 85: /* add_cmd: "chain" chain_spec  */
#line 1087 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8016 "parser_bison.c"
    break;

  case 86: /* add_cmd: "chain" chain_spec chain_block_alloc '{' chain_block '}'  */
#line 1092 "parser_bison.y"
                        {
				(yyvsp[-1].chain)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].chain)->handle, &(yyvsp[-4].handle));
				close_scope(state);
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_CHAIN, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].chain));
			}
#line 8027 "parser_bison.c"
    break;

  case 87: /* add_cmd: "rule" rule_position rule  */
#line 1099 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_RULE, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].rule));
			}
#line 8035 "parser_bison.c"
    break;

  case 88: /* add_cmd: rule_position rule  */
#line 1103 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_RULE, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].rule));
			}
#line 8043 "parser_bison.c"
    break;

  case 89: /* add_cmd: "set" set_spec set_block_alloc '{' set_block '}'  */
#line 1108 "parser_bison.y"
                        {
				(yyvsp[-1].set)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].set)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SET, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].set));
			}
#line 8053 "parser_bison.c"
    break;

  case 90: /* add_cmd: "map" set_spec map_block_alloc '{' map_block '}'  */
#line 1115 "parser_bison.y"
                        {
				(yyvsp[-1].set)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].set)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SET, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].set));
			}
#line 8063 "parser_bison.c"
    break;

  case 91: /* add_cmd: "element" set_spec set_block_expr  */
#line 1121 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_ELEMENTS, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].expr));
			}
#line 8071 "parser_bison.c"
    break;

  case 92: /* add_cmd: "flowtable" flowtable_spec flowtable_block_alloc '{' flowtable_block '}'  */
#line 1126 "parser_bison.y"
                        {
				(yyvsp[-1].flowtable)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].flowtable)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_FLOWTABLE, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].flowtable));
			}
#line 8081 "parser_bison.c"
    break;

  case 93: /* add_cmd: "counter" obj_spec close_scope_counter  */
#line 1132 "parser_bison.y"
                        {
				struct obj *obj;

				obj = obj_alloc(&(yyloc));
				obj->type = NFT_OBJECT_COUNTER;
				handle_merge(&obj->handle, &(yyvsp[-1].handle));
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_COUNTER, &(yyvsp[-1].handle), &(yyloc), obj);
			}
#line 8094 "parser_bison.c"
    break;

  case 94: /* add_cmd: "counter" obj_spec counter_obj counter_config close_scope_counter  */
#line 1141 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_COUNTER, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8102 "parser_bison.c"
    break;

  case 95: /* add_cmd: "counter" obj_spec counter_obj '{' counter_block '}' close_scope_counter  */
#line 1145 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_COUNTER, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8110 "parser_bison.c"
    break;

  case 96: /* add_cmd: "quota" obj_spec quota_obj quota_config close_scope_quota  */
#line 1149 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_QUOTA, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8118 "parser_bison.c"
    break;

  case 97: /* add_cmd: "quota" obj_spec quota_obj '{' quota_block '}' close_scope_quota  */
#line 1153 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_QUOTA, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8126 "parser_bison.c"
    break;

  case 98: /* add_cmd: "ct" "helper" obj_spec ct_obj_alloc '{' ct_helper_block '}' close_scope_ct  */
#line 1157 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_ADD, NFT_OBJECT_CT_HELPER, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8134 "parser_bison.c"
    break;

  case 99: /* add_cmd: "ct" "timeout" obj_spec ct_obj_alloc '{' ct_timeout_block '}' close_scope_ct  */
#line 1161 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_ADD, NFT_OBJECT_CT_TIMEOUT, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8142 "parser_bison.c"
    break;

  case 100: /* add_cmd: "ct" "expectation" obj_spec ct_obj_alloc '{' ct_expect_block '}' close_scope_ct  */
#line 1165 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_ADD, NFT_OBJECT_CT_EXPECT, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8150 "parser_bison.c"
    break;

  case 101: /* add_cmd: "limit" obj_spec limit_obj limit_config close_scope_limit  */
#line 1169 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_LIMIT, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8158 "parser_bison.c"
    break;

  case 102: /* add_cmd: "limit" obj_spec limit_obj '{' limit_block '}' close_scope_limit  */
#line 1173 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_LIMIT, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8166 "parser_bison.c"
    break;

  case 103: /* add_cmd: "secmark" obj_spec secmark_obj secmark_config close_scope_secmark  */
#line 1177 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SECMARK, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8174 "parser_bison.c"
    break;

  case 104: /* add_cmd: "secmark" obj_spec secmark_obj '{' secmark_block '}' close_scope_secmark  */
#line 1181 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SECMARK, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8182 "parser_bison.c"
    break;

  case 105: /* add_cmd: "synproxy" obj_spec synproxy_obj synproxy_config close_scope_synproxy  */
#line 1185 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SYNPROXY, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8190 "parser_bison.c"
    break;

  case 106: /* add_cmd: "synproxy" obj_spec synproxy_obj '{' synproxy_block '}' close_scope_synproxy  */
#line 1189 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_ADD, CMD_OBJ_SYNPROXY, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8198 "parser_bison.c"
    break;

  case 107: /* replace_cmd: "rule" ruleid_spec rule  */
#line 1195 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_REPLACE, CMD_OBJ_RULE, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].rule));
			}
#line 8206 "parser_bison.c"
    break;

  case 108: /* create_cmd: "table" table_spec  */
#line 1201 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8214 "parser_bison.c"
    break;

  case 109: /* create_cmd: "table" table_spec table_block_alloc '{' table_block '}'  */
#line 1206 "parser_bison.y"
                        {
				handle_merge(&(yyvsp[-3].table)->handle, &(yyvsp[-4].handle));
				close_scope(state);
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_TABLE, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].table));
			}
#line 8224 "parser_bison.c"
    break;

  case 110: /* create_cmd: "chain" chain_spec  */
#line 1212 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8232 "parser_bison.c"
    break;

  case 111: /* create_cmd: "chain" chain_spec chain_block_alloc '{' chain_block '}'  */
#line 1217 "parser_bison.y"
                        {
				(yyvsp[-1].chain)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].chain)->handle, &(yyvsp[-4].handle));
				close_scope(state);
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_CHAIN, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].chain));
			}
#line 8243 "parser_bison.c"
    break;

  case 112: /* create_cmd: "set" set_spec set_block_alloc '{' set_block '}'  */
#line 1225 "parser_bison.y"
                        {
				(yyvsp[-1].set)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].set)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_SET, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].set));
			}
#line 8253 "parser_bison.c"
    break;

  case 113: /* create_cmd: "map" set_spec map_block_alloc '{' map_block '}'  */
#line 1232 "parser_bison.y"
                        {
				(yyvsp[-1].set)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].set)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_SET, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].set));
			}
#line 8263 "parser_bison.c"
    break;

  case 114: /* create_cmd: "element" set_spec set_block_expr  */
#line 1238 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_ELEMENTS, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].expr));
			}
#line 8271 "parser_bison.c"
    break;

  case 115: /* create_cmd: "flowtable" flowtable_spec flowtable_block_alloc '{' flowtable_block '}'  */
#line 1243 "parser_bison.y"
                        {
				(yyvsp[-1].flowtable)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].flowtable)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_FLOWTABLE, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].flowtable));
			}
#line 8281 "parser_bison.c"
    break;

  case 116: /* create_cmd: "counter" obj_spec close_scope_counter  */
#line 1249 "parser_bison.y"
                        {
				struct obj *obj;

				obj = obj_alloc(&(yyloc));
				obj->type = NFT_OBJECT_COUNTER;
				handle_merge(&obj->handle, &(yyvsp[-1].handle));
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_COUNTER, &(yyvsp[-1].handle), &(yyloc), obj);
			}
#line 8294 "parser_bison.c"
    break;

  case 117: /* create_cmd: "counter" obj_spec counter_obj counter_config close_scope_counter  */
#line 1258 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_COUNTER, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8302 "parser_bison.c"
    break;

  case 118: /* create_cmd: "quota" obj_spec quota_obj quota_config close_scope_quota  */
#line 1262 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_QUOTA, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8310 "parser_bison.c"
    break;

  case 119: /* create_cmd: "ct" "helper" obj_spec ct_obj_alloc '{' ct_helper_block '}' close_scope_ct  */
#line 1266 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_CREATE, NFT_OBJECT_CT_HELPER, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8318 "parser_bison.c"
    break;

  case 120: /* create_cmd: "ct" "timeout" obj_spec ct_obj_alloc '{' ct_timeout_block '}' close_scope_ct  */
#line 1270 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_CREATE, NFT_OBJECT_CT_TIMEOUT, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8326 "parser_bison.c"
    break;

  case 121: /* create_cmd: "ct" "expectation" obj_spec ct_obj_alloc '{' ct_expect_block '}' close_scope_ct  */
#line 1274 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_CREATE, NFT_OBJECT_CT_EXPECT, &(yyvsp[-5].handle), &(yyloc), (yyvsp[-4].obj));
			}
#line 8334 "parser_bison.c"
    break;

  case 122: /* create_cmd: "limit" obj_spec limit_obj limit_config close_scope_limit  */
#line 1278 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_LIMIT, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8342 "parser_bison.c"
    break;

  case 123: /* create_cmd: "secmark" obj_spec secmark_obj secmark_config close_scope_secmark  */
#line 1282 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_SECMARK, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8350 "parser_bison.c"
    break;

  case 124: /* create_cmd: "synproxy" obj_spec synproxy_obj synproxy_config close_scope_synproxy  */
#line 1286 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_CREATE, CMD_OBJ_SYNPROXY, &(yyvsp[-3].handle), &(yyloc), (yyvsp[-2].obj));
			}
#line 8358 "parser_bison.c"
    break;

  case 125: /* insert_cmd: "rule" rule_position rule  */
#line 1292 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_INSERT, CMD_OBJ_RULE, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].rule));
			}
#line 8366 "parser_bison.c"
    break;

  case 134: /* delete_cmd: "table" table_or_id_spec  */
#line 1314 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8374 "parser_bison.c"
    break;

  case 135: /* delete_cmd: "chain" chain_or_id_spec  */
#line 1318 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8382 "parser_bison.c"
    break;

  case 136: /* delete_cmd: "rule" ruleid_spec  */
#line 1322 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_RULE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8390 "parser_bison.c"
    break;

  case 137: /* delete_cmd: "set" set_or_id_spec  */
#line 1326 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8398 "parser_bison.c"
    break;

  case 138: /* delete_cmd: "map" set_spec  */
#line 1330 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8406 "parser_bison.c"
    break;

  case 139: /* delete_cmd: "element" set_spec set_block_expr  */
#line 1334 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_ELEMENTS, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].expr));
			}
#line 8414 "parser_bison.c"
    break;

  case 140: /* delete_cmd: "flowtable" flowtable_spec  */
#line 1338 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_FLOWTABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8422 "parser_bison.c"
    break;

  case 141: /* delete_cmd: "flowtable" flowtableid_spec  */
#line 1342 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_FLOWTABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8430 "parser_bison.c"
    break;

  case 142: /* delete_cmd: "flowtable" flowtable_spec flowtable_block_alloc '{' flowtable_block '}'  */
#line 1347 "parser_bison.y"
                        {
				(yyvsp[-1].flowtable)->location = (yylsp[-1]);
				handle_merge(&(yyvsp[-3].flowtable)->handle, &(yyvsp[-4].handle));
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_FLOWTABLE, &(yyvsp[-4].handle), &(yyloc), (yyvsp[-1].flowtable));
			}
#line 8440 "parser_bison.c"
    break;

  case 143: /* delete_cmd: "counter" obj_or_id_spec close_scope_counter  */
#line 1353 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_COUNTER, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 8448 "parser_bison.c"
    break;

  case 144: /* delete_cmd: "quota" obj_or_id_spec close_scope_quota  */
#line 1357 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_QUOTA, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 8456 "parser_bison.c"
    break;

  case 145: /* delete_cmd: "ct" ct_obj_type obj_spec ct_obj_alloc close_scope_ct  */
#line 1361 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_DELETE, (yyvsp[-3].val), &(yyvsp[-2].handle), &(yyloc), (yyvsp[-1].obj));
				if ((yyvsp[-3].val) == NFT_OBJECT_CT_TIMEOUT)
					init_list_head(&(yyvsp[-1].obj)->ct_timeout.timeout_list);
			}
#line 8466 "parser_bison.c"
    break;

  case 146: /* delete_cmd: "limit" obj_or_id_spec close_scope_limit  */
#line 1367 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_LIMIT, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 8474 "parser_bison.c"
    break;

  case 147: /* delete_cmd: "secmark" obj_or_id_spec close_scope_secmark  */
#line 1371 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_SECMARK, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 8482 "parser_bison.c"
    break;

  case 148: /* delete_cmd: "synproxy" obj_or_id_spec close_scope_synproxy  */
#line 1375 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_DELETE, CMD_OBJ_SYNPROXY, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 8490 "parser_bison.c"
    break;

  case 149: /* get_cmd: "element" set_spec set_block_expr  */
#line 1381 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_GET, CMD_OBJ_ELEMENTS, &(yyvsp[-1].handle), &(yyloc), (yyvsp[0].expr));
			}
#line 8498 "parser_bison.c"
    break;

  case 150: /* list_cmd: "table" table_spec  */
#line 1387 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8506 "parser_bison.c"
    break;

  case 151: /* list_cmd: "tables" ruleset_spec  */
#line 1391 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8514 "parser_bison.c"
    break;

  case 152: /* list_cmd: "chain" chain_spec  */
#line 1395 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8522 "parser_bison.c"
    break;

  case 153: /* list_cmd: "chains" ruleset_spec  */
#line 1399 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_CHAINS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8530 "parser_bison.c"
    break;

  case 154: /* list_cmd: "sets" ruleset_spec  */
#line 1403 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SETS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8538 "parser_bison.c"
    break;

  case 155: /* list_cmd: "sets" "table" table_spec  */
#line 1407 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SETS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8546 "parser_bison.c"
    break;

  case 156: /* list_cmd: "set" set_spec  */
#line 1411 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8554 "parser_bison.c"
    break;

  case 157: /* list_cmd: "counters" ruleset_spec  */
#line 1415 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_COUNTERS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8562 "parser_bison.c"
    break;

  case 158: /* list_cmd: "counters" "table" table_spec  */
#line 1419 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_COUNTERS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8570 "parser_bison.c"
    break;

  case 159: /* list_cmd: "counter" obj_spec close_scope_counter  */
#line 1423 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_COUNTER, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 8578 "parser_bison.c"
    break;

  case 160: /* list_cmd: "quotas" ruleset_spec  */
#line 1427 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_QUOTAS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8586 "parser_bison.c"
    break;

  case 161: /* list_cmd: "quotas" "table" table_spec  */
#line 1431 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_QUOTAS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8594 "parser_bison.c"
    break;

  case 162: /* list_cmd: "quota" obj_spec close_scope_quota  */
#line 1435 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_QUOTA, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 8602 "parser_bison.c"
    break;

  case 163: /* list_cmd: "limits" ruleset_spec  */
#line 1439 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_LIMITS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8610 "parser_bison.c"
    break;

  case 164: /* list_cmd: "limits" "table" table_spec  */
#line 1443 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_LIMITS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8618 "parser_bison.c"
    break;

  case 165: /* list_cmd: "limit" obj_spec close_scope_limit  */
#line 1447 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_LIMIT, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 8626 "parser_bison.c"
    break;

  case 166: /* list_cmd: "secmarks" ruleset_spec  */
#line 1451 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SECMARKS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8634 "parser_bison.c"
    break;

  case 167: /* list_cmd: "secmarks" "table" table_spec  */
#line 1455 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SECMARKS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8642 "parser_bison.c"
    break;

  case 168: /* list_cmd: "secmark" obj_spec close_scope_secmark  */
#line 1459 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SECMARK, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 8650 "parser_bison.c"
    break;

  case 169: /* list_cmd: "synproxys" ruleset_spec  */
#line 1463 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SYNPROXYS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8658 "parser_bison.c"
    break;

  case 170: /* list_cmd: "synproxys" "table" table_spec  */
#line 1467 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SYNPROXYS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8666 "parser_bison.c"
    break;

  case 171: /* list_cmd: "synproxy" obj_spec close_scope_synproxy  */
#line 1471 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_SYNPROXY, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 8674 "parser_bison.c"
    break;

  case 172: /* list_cmd: "ruleset" ruleset_spec  */
#line 1475 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_RULESET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8682 "parser_bison.c"
    break;

  case 173: /* list_cmd: "flow" "tables" ruleset_spec  */
#line 1479 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_METERS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8690 "parser_bison.c"
    break;

  case 174: /* list_cmd: "flow" "table" set_spec  */
#line 1483 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_METER, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8698 "parser_bison.c"
    break;

  case 175: /* list_cmd: "meters" ruleset_spec  */
#line 1487 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_METERS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8706 "parser_bison.c"
    break;

  case 176: /* list_cmd: "meter" set_spec  */
#line 1491 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_METER, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8714 "parser_bison.c"
    break;

  case 177: /* list_cmd: "flowtables" ruleset_spec  */
#line 1495 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_FLOWTABLES, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8722 "parser_bison.c"
    break;

  case 178: /* list_cmd: "flowtable" flowtable_spec  */
#line 1499 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_FLOWTABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8730 "parser_bison.c"
    break;

  case 179: /* list_cmd: "maps" ruleset_spec  */
#line 1503 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_MAPS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8738 "parser_bison.c"
    break;

  case 180: /* list_cmd: "map" set_spec  */
#line 1507 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_MAP, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8746 "parser_bison.c"
    break;

  case 181: /* list_cmd: "ct" ct_obj_type obj_spec close_scope_ct  */
#line 1511 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc_obj_ct(CMD_LIST, (yyvsp[-2].val), &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 8754 "parser_bison.c"
    break;

  case 182: /* list_cmd: "ct" ct_cmd_type "table" table_spec close_scope_ct  */
#line 1515 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, (yyvsp[-3].val), &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 8762 "parser_bison.c"
    break;

  case 183: /* list_cmd: "hooks" basehook_spec  */
#line 1519 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_LIST, CMD_OBJ_HOOKS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8770 "parser_bison.c"
    break;

  case 184: /* basehook_device_name: "device" "string"  */
#line 1525 "parser_bison.y"
                        {
				(yyval.string) = (yyvsp[0].string);
			}
#line 8778 "parser_bison.c"
    break;

  case 185: /* basehook_spec: ruleset_spec  */
#line 1531 "parser_bison.y"
                        {
				(yyval.handle) = (yyvsp[0].handle);
			}
#line 8786 "parser_bison.c"
    break;

  case 186: /* basehook_spec: ruleset_spec basehook_device_name  */
#line 1535 "parser_bison.y"
                        {
				if ((yyvsp[0].string)) {
					(yyvsp[-1].handle).obj.name = (yyvsp[0].string);
					(yyvsp[-1].handle).obj.location = (yylsp[0]);
				}
				(yyval.handle) = (yyvsp[-1].handle);
			}
#line 8798 "parser_bison.c"
    break;

  case 187: /* reset_cmd: "counters" ruleset_spec  */
#line 1545 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_COUNTERS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8806 "parser_bison.c"
    break;

  case 188: /* reset_cmd: "counters" "table" table_spec  */
#line 1549 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_COUNTERS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8814 "parser_bison.c"
    break;

  case 189: /* reset_cmd: "counter" obj_spec close_scope_counter  */
#line 1553 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_COUNTER, &(yyvsp[-1].handle),&(yyloc), NULL);
			}
#line 8822 "parser_bison.c"
    break;

  case 190: /* reset_cmd: "quotas" ruleset_spec  */
#line 1557 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_QUOTAS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8830 "parser_bison.c"
    break;

  case 191: /* reset_cmd: "quotas" "table" table_spec  */
#line 1561 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_QUOTAS, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8838 "parser_bison.c"
    break;

  case 192: /* reset_cmd: "quota" obj_spec close_scope_quota  */
#line 1565 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RESET, CMD_OBJ_QUOTA, &(yyvsp[-1].handle), &(yyloc), NULL);
			}
#line 8846 "parser_bison.c"
    break;

  case 193: /* flush_cmd: "table" table_spec  */
#line 1571 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_TABLE, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8854 "parser_bison.c"
    break;

  case 194: /* flush_cmd: "chain" chain_spec  */
#line 1575 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_CHAIN, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8862 "parser_bison.c"
    break;

  case 195: /* flush_cmd: "set" set_spec  */
#line 1579 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_SET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8870 "parser_bison.c"
    break;

  case 196: /* flush_cmd: "map" set_spec  */
#line 1583 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_MAP, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8878 "parser_bison.c"
    break;

  case 197: /* flush_cmd: "flow" "table" set_spec  */
#line 1587 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_METER, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8886 "parser_bison.c"
    break;

  case 198: /* flush_cmd: "meter" set_spec  */
#line 1591 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_METER, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8894 "parser_bison.c"
    break;

  case 199: /* flush_cmd: "ruleset" ruleset_spec  */
#line 1595 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_FLUSH, CMD_OBJ_RULESET, &(yyvsp[0].handle), &(yyloc), NULL);
			}
#line 8902 "parser_bison.c"
    break;

  case 200: /* rename_cmd: "chain" chain_spec identifier  */
#line 1601 "parser_bison.y"
                        {
				(yyval.cmd) = cmd_alloc(CMD_RENAME, CMD_OBJ_CHAIN, &(yyvsp[-1].handle), &(yyloc), NULL);
				(yyval.cmd)->arg = (yyvsp[0].string);
			}
#line 8911 "parser_bison.c"
    break;

  case 201: /* import_cmd: "ruleset" markup_format  */
#line 1608 "parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				struct markup *markup = markup_alloc((yyvsp[0].val));
				(yyval.cmd) = cmd_alloc(CMD_IMPORT, CMD_OBJ_MARKUP, &h, &(yyloc), markup);
			}
#line 8921 "parser_bison.c"
    break;

  case 202: /* import_cmd: markup_format  */
#line 1614 "parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				struct markup *markup = markup_alloc((yyvsp[0].val));
				(yyval.cmd) = cmd_alloc(CMD_IMPORT, CMD_OBJ_MARKUP, &h, &(yyloc), markup);
			}
#line 8931 "parser_bison.c"
    break;

  case 203: /* export_cmd: "ruleset" markup_format  */
#line 1622 "parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				struct markup *markup = markup_alloc((yyvsp[0].val));
				(yyval.cmd) = cmd_alloc(CMD_EXPORT, CMD_OBJ_MARKUP, &h, &(yyloc), markup);
			}
#line 8941 "parser_bison.c"
    break;

  case 204: /* export_cmd: markup_format  */
#line 1628 "parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				struct markup *markup = markup_alloc((yyvsp[0].val));
				(yyval.cmd) = cmd_alloc(CMD_EXPORT, CMD_OBJ_MARKUP, &h, &(yyloc), markup);
			}
#line 8951 "parser_bison.c"
    break;

  case 205: /* monitor_cmd: monitor_event monitor_object monitor_format  */
#line 1636 "parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				struct monitor *m = monitor_alloc((yyvsp[0].val), (yyvsp[-1].val), (yyvsp[-2].string));
				m->location = (yylsp[-2]);
				(yyval.cmd) = cmd_alloc(CMD_MONITOR, CMD_OBJ_MONITOR, &h, &(yyloc), m);
			}
#line 8962 "parser_bison.c"
    break;

  case 206: /* monitor_event: %empty  */
#line 1644 "parser_bison.y"
                                                { (yyval.string) = NULL; }
#line 8968 "parser_bison.c"
    break;

  case 207: /* monitor_event: "string"  */
#line 1645 "parser_bison.y"
                                                { (yyval.string) = (yyvsp[0].string); }
#line 8974 "parser_bison.c"
    break;

  case 208: /* monitor_object: %empty  */
#line 1648 "parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_ANY; }
#line 8980 "parser_bison.c"
    break;

  case 209: /* monitor_object: "tables"  */
#line 1649 "parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_TABLES; }
#line 8986 "parser_bison.c"
    break;

  case 210: /* monitor_object: "chains"  */
#line 1650 "parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_CHAINS; }
#line 8992 "parser_bison.c"
    break;

  case 211: /* monitor_object: "sets"  */
#line 1651 "parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_SETS; }
#line 8998 "parser_bison.c"
    break;

  case 212: /* monitor_object: "rules"  */
#line 1652 "parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_RULES; }
#line 9004 "parser_bison.c"
    break;

  case 213: /* monitor_object: "elements"  */
#line 1653 "parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_ELEMS; }
#line 9010 "parser_bison.c"
    break;

  case 214: /* monitor_object: "ruleset"  */
#line 1654 "parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_RULESET; }
#line 9016 "parser_bison.c"
    break;

  case 215: /* monitor_object: "trace"  */
#line 1655 "parser_bison.y"
                                                { (yyval.val) = CMD_MONITOR_OBJ_TRACE; }
#line 9022 "parser_bison.c"
    break;

  case 216: /* monitor_format: %empty  */
#line 1658 "parser_bison.y"
                                                { (yyval.val) = NFTNL_OUTPUT_DEFAULT; }
#line 9028 "parser_bison.c"
    break;

  case 218: /* markup_format: "xml"  */
#line 1662 "parser_bison.y"
                                                { (yyval.val) = __NFT_OUTPUT_NOTSUPP; }
#line 9034 "parser_bison.c"
    break;

  case 219: /* markup_format: "json"  */
#line 1663 "parser_bison.y"
                                                { (yyval.val) = NFTNL_OUTPUT_JSON; }
#line 9040 "parser_bison.c"
    break;

  case 220: /* markup_format: "vm" "json"  */
#line 1664 "parser_bison.y"
                                                { (yyval.val) = NFTNL_OUTPUT_JSON; }
#line 9046 "parser_bison.c"
    break;

  case 221: /* describe_cmd: primary_expr  */
#line 1668 "parser_bison.y"
                        {
				struct handle h = { .family = NFPROTO_UNSPEC };
				(yyval.cmd) = cmd_alloc(CMD_DESCRIBE, CMD_OBJ_EXPR, &h, &(yyloc), NULL);
				(yyval.cmd)->expr = (yyvsp[0].expr);
			}
#line 9056 "parser_bison.c"
    break;

  case 222: /* table_block_alloc: %empty  */
#line 1676 "parser_bison.y"
                        {
				(yyval.table) = table_alloc();
				open_scope(state, &(yyval.table)->scope);
			}
#line 9065 "parser_bison.c"
    break;

  case 223: /* table_options: "flags" "string" close_scope_flags  */
#line 1683 "parser_bison.y"
                        {
				if (strcmp((yyvsp[-1].string), "dormant") == 0) {
					(yyvsp[-3].table)->flags |= TABLE_F_DORMANT;
					xfree((yyvsp[-1].string));
				} else if (strcmp((yyvsp[-1].string), "owner") == 0) {
					(yyvsp[-3].table)->flags |= TABLE_F_OWNER;
					xfree((yyvsp[-1].string));
				} else {
					erec_queue(error(&(yylsp[-1]), "unknown table option %s", (yyvsp[-1].string)),
						   state->msgs);
					xfree((yyvsp[-1].string));
					YYERROR;
				}
			}
#line 9084 "parser_bison.c"
    break;

  case 224: /* table_options: comment_spec  */
#line 1698 "parser_bison.y"
                        {
				if (already_set((yyvsp[-1].table)->comment, &(yyloc), state)) {
					xfree((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].table)->comment = (yyvsp[0].string);
			}
#line 9096 "parser_bison.c"
    break;

  case 225: /* table_block: %empty  */
#line 1707 "parser_bison.y"
                                                { (yyval.table) = (yyvsp[(-1) - (0)].table); }
#line 9102 "parser_bison.c"
    break;

  case 229: /* table_block: table_block "chain" chain_identifier chain_block_alloc '{' chain_block '}' stmt_separator  */
#line 1714 "parser_bison.y"
                        {
				(yyvsp[-4].chain)->location = (yylsp[-5]);
				handle_merge(&(yyvsp[-4].chain)->handle, &(yyvsp[-5].handle));
				handle_free(&(yyvsp[-5].handle));
				close_scope(state);
				list_add_tail(&(yyvsp[-4].chain)->list, &(yyvsp[-7].table)->chains);
				(yyval.table) = (yyvsp[-7].table);
			}
#line 9115 "parser_bison.c"
    break;

  case 230: /* table_block: table_block "set" set_identifier set_block_alloc '{' set_block '}' stmt_separator  */
#line 1725 "parser_bison.y"
                        {
				(yyvsp[-4].set)->location = (yylsp[-5]);
				handle_merge(&(yyvsp[-4].set)->handle, &(yyvsp[-5].handle));
				handle_free(&(yyvsp[-5].handle));
				list_add_tail(&(yyvsp[-4].set)->list, &(yyvsp[-7].table)->sets);
				(yyval.table) = (yyvsp[-7].table);
			}
#line 9127 "parser_bison.c"
    break;

  case 231: /* table_block: table_block "map" set_identifier map_block_alloc '{' map_block '}' stmt_separator  */
#line 1735 "parser_bison.y"
                        {
				(yyvsp[-4].set)->location = (yylsp[-5]);
				handle_merge(&(yyvsp[-4].set)->handle, &(yyvsp[-5].handle));
				handle_free(&(yyvsp[-5].handle));
				list_add_tail(&(yyvsp[-4].set)->list, &(yyvsp[-7].table)->sets);
				(yyval.table) = (yyvsp[-7].table);
			}
#line 9139 "parser_bison.c"
    break;

  case 232: /* table_block: table_block "flowtable" flowtable_identifier flowtable_block_alloc '{' flowtable_block '}' stmt_separator  */
#line 1746 "parser_bison.y"
                        {
				(yyvsp[-4].flowtable)->location = (yylsp[-5]);
				handle_merge(&(yyvsp[-4].flowtable)->handle, &(yyvsp[-5].handle));
				handle_free(&(yyvsp[-5].handle));
				list_add_tail(&(yyvsp[-4].flowtable)->list, &(yyvsp[-7].table)->flowtables);
				(yyval.table) = (yyvsp[-7].table);
			}
#line 9151 "parser_bison.c"
    break;

  case 233: /* table_block: table_block "counter" obj_identifier obj_block_alloc '{' counter_block '}' stmt_separator close_scope_counter  */
#line 1756 "parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_COUNTER;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-8].table)->objs);
				(yyval.table) = (yyvsp[-8].table);
			}
#line 9164 "parser_bison.c"
    break;

  case 234: /* table_block: table_block "quota" obj_identifier obj_block_alloc '{' quota_block '}' stmt_separator close_scope_quota  */
#line 1767 "parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_QUOTA;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-8].table)->objs);
				(yyval.table) = (yyvsp[-8].table);
			}
#line 9177 "parser_bison.c"
    break;

  case 235: /* table_block: table_block "ct" "helper" obj_identifier obj_block_alloc '{' ct_helper_block '}' close_scope_ct stmt_separator  */
#line 1776 "parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_CT_HELPER;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-9].table)->objs);
				(yyval.table) = (yyvsp[-9].table);
			}
#line 9190 "parser_bison.c"
    break;

  case 236: /* table_block: table_block "ct" "timeout" obj_identifier obj_block_alloc '{' ct_timeout_block '}' close_scope_ct stmt_separator  */
#line 1785 "parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_CT_TIMEOUT;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-9].table)->objs);
				(yyval.table) = (yyvsp[-9].table);
			}
#line 9203 "parser_bison.c"
    break;

  case 237: /* table_block: table_block "ct" "expectation" obj_identifier obj_block_alloc '{' ct_expect_block '}' close_scope_ct stmt_separator  */
#line 1794 "parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_CT_EXPECT;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-9].table)->objs);
				(yyval.table) = (yyvsp[-9].table);
			}
#line 9216 "parser_bison.c"
    break;

  case 238: /* table_block: table_block "limit" obj_identifier obj_block_alloc '{' limit_block '}' stmt_separator close_scope_limit  */
#line 1805 "parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_LIMIT;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-8].table)->objs);
				(yyval.table) = (yyvsp[-8].table);
			}
#line 9229 "parser_bison.c"
    break;

  case 239: /* table_block: table_block "secmark" obj_identifier obj_block_alloc '{' secmark_block '}' stmt_separator close_scope_secmark  */
#line 1816 "parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_SECMARK;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-8].table)->objs);
				(yyval.table) = (yyvsp[-8].table);
			}
#line 9242 "parser_bison.c"
    break;

  case 240: /* table_block: table_block "synproxy" obj_identifier obj_block_alloc '{' synproxy_block '}' stmt_separator close_scope_synproxy  */
#line 1827 "parser_bison.y"
                        {
				(yyvsp[-5].obj)->location = (yylsp[-6]);
				(yyvsp[-5].obj)->type = NFT_OBJECT_SYNPROXY;
				handle_merge(&(yyvsp[-5].obj)->handle, &(yyvsp[-6].handle));
				handle_free(&(yyvsp[-6].handle));
				list_add_tail(&(yyvsp[-5].obj)->list, &(yyvsp[-8].table)->objs);
				(yyval.table) = (yyvsp[-8].table);
			}
#line 9255 "parser_bison.c"
    break;

  case 241: /* chain_block_alloc: %empty  */
#line 1838 "parser_bison.y"
                        {
				(yyval.chain) = chain_alloc(NULL);
				open_scope(state, &(yyval.chain)->scope);
			}
#line 9264 "parser_bison.c"
    break;

  case 242: /* chain_block: %empty  */
#line 1844 "parser_bison.y"
                                                { (yyval.chain) = (yyvsp[(-1) - (0)].chain); }
#line 9270 "parser_bison.c"
    break;

  case 248: /* chain_block: chain_block rule stmt_separator  */
#line 1851 "parser_bison.y"
                        {
				list_add_tail(&(yyvsp[-1].rule)->list, &(yyvsp[-2].chain)->rules);
				(yyval.chain) = (yyvsp[-2].chain);
			}
#line 9279 "parser_bison.c"
    break;

  case 249: /* chain_block: chain_block comment_spec stmt_separator  */
#line 1856 "parser_bison.y"
                        {
				if (already_set((yyvsp[-2].chain)->comment, &(yylsp[-1]), state)) {
					xfree((yyvsp[-1].string));
					YYERROR;
				}
				(yyvsp[-2].chain)->comment = (yyvsp[-1].string);
			}
#line 9291 "parser_bison.c"
    break;

  case 250: /* subchain_block: %empty  */
#line 1865 "parser_bison.y"
                                                { (yyval.chain) = (yyvsp[(-1) - (0)].chain); }
#line 9297 "parser_bison.c"
    break;

  case 252: /* subchain_block: subchain_block rule stmt_separator  */
#line 1868 "parser_bison.y"
                        {
				list_add_tail(&(yyvsp[-1].rule)->list, &(yyvsp[-2].chain)->rules);
				(yyval.chain) = (yyvsp[-2].chain);
			}
#line 9306 "parser_bison.c"
    break;

  case 253: /* typeof_data_expr: primary_expr  */
#line 1875 "parser_bison.y"
                        {
				struct expr *e = (yyvsp[0].expr);

				if (e->etype == EXPR_SYMBOL &&
				    strcmp("verdict", e->identifier) == 0) {
					struct expr *v = verdict_expr_alloc(&(yylsp[0]), NF_ACCEPT, NULL);

					expr_free(e);
					v->flags &= ~EXPR_F_CONSTANT;
					e = v;
				}

				if (expr_ops(e)->build_udata == NULL) {
					erec_queue(error(&(yylsp[0]), "map data type '%s' lacks typeof serialization", expr_ops(e)->name),
						   state->msgs);
					expr_free(e);
					YYERROR;
				}
				(yyval.expr) = e;
			}
#line 9331 "parser_bison.c"
    break;

  case 254: /* typeof_data_expr: typeof_expr "." primary_expr  */
#line 1896 "parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 9344 "parser_bison.c"
    break;

  case 255: /* typeof_expr: primary_expr  */
#line 1907 "parser_bison.y"
                        {
				if (expr_ops((yyvsp[0].expr))->build_udata == NULL) {
					erec_queue(error(&(yylsp[0]), "primary expression type '%s' lacks typeof serialization", expr_ops((yyvsp[0].expr))->name),
						   state->msgs);
					expr_free((yyvsp[0].expr));
					YYERROR;
				}

				(yyval.expr) = (yyvsp[0].expr);
			}
#line 9359 "parser_bison.c"
    break;

  case 256: /* typeof_expr: typeof_expr "." primary_expr  */
#line 1918 "parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 9372 "parser_bison.c"
    break;

  case 257: /* set_block_alloc: %empty  */
#line 1930 "parser_bison.y"
                        {
				(yyval.set) = set_alloc(NULL);
			}
#line 9380 "parser_bison.c"
    break;

  case 258: /* set_block: %empty  */
#line 1935 "parser_bison.y"
                                                { (yyval.set) = (yyvsp[(-1) - (0)].set); }
#line 9386 "parser_bison.c"
    break;

  case 261: /* set_block: set_block "type" data_type_expr stmt_separator close_scope_type  */
#line 1939 "parser_bison.y"
                        {
				(yyvsp[-4].set)->key = (yyvsp[-2].expr);
				(yyval.set) = (yyvsp[-4].set);
			}
#line 9395 "parser_bison.c"
    break;

  case 262: /* set_block: set_block "typeof" typeof_expr stmt_separator  */
#line 1944 "parser_bison.y"
                        {
				(yyvsp[-3].set)->key = (yyvsp[-1].expr);
				datatype_set((yyvsp[-3].set)->key, (yyvsp[-1].expr)->dtype);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 9405 "parser_bison.c"
    break;

  case 263: /* set_block: set_block "flags" set_flag_list stmt_separator close_scope_flags  */
#line 1950 "parser_bison.y"
                        {
				(yyvsp[-4].set)->flags = (yyvsp[-2].val);
				(yyval.set) = (yyvsp[-4].set);
			}
#line 9414 "parser_bison.c"
    break;

  case 264: /* set_block: set_block "timeout" time_spec stmt_separator  */
#line 1955 "parser_bison.y"
                        {
				(yyvsp[-3].set)->timeout = (yyvsp[-1].val);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 9423 "parser_bison.c"
    break;

  case 265: /* set_block: set_block "gc-interval" time_spec stmt_separator  */
#line 1960 "parser_bison.y"
                        {
				(yyvsp[-3].set)->gc_int = (yyvsp[-1].val);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 9432 "parser_bison.c"
    break;

  case 266: /* set_block: set_block stateful_stmt_list stmt_separator  */
#line 1965 "parser_bison.y"
                        {
				list_splice_tail((yyvsp[-1].list), &(yyvsp[-2].set)->stmt_list);
				(yyval.set) = (yyvsp[-2].set);
				free((yyvsp[-1].list));
			}
#line 9442 "parser_bison.c"
    break;

  case 267: /* set_block: set_block "elements" '=' set_block_expr  */
#line 1971 "parser_bison.y"
                        {
				(yyvsp[-3].set)->init = (yyvsp[0].expr);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 9451 "parser_bison.c"
    break;

  case 268: /* set_block: set_block "auto-merge"  */
#line 1976 "parser_bison.y"
                        {
				(yyvsp[-1].set)->automerge = true;
				(yyval.set) = (yyvsp[-1].set);
			}
#line 9460 "parser_bison.c"
    break;

  case 270: /* set_block: set_block comment_spec stmt_separator  */
#line 1982 "parser_bison.y"
                        {
				if (already_set((yyvsp[-2].set)->comment, &(yylsp[-1]), state)) {
					xfree((yyvsp[-1].string));
					YYERROR;
				}
				(yyvsp[-2].set)->comment = (yyvsp[-1].string);
				(yyval.set) = (yyvsp[-2].set);
			}
#line 9473 "parser_bison.c"
    break;

  case 273: /* set_flag_list: set_flag_list "comma" set_flag  */
#line 1997 "parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-2].val) | (yyvsp[0].val);
			}
#line 9481 "parser_bison.c"
    break;

  case 275: /* set_flag: "constant"  */
#line 2003 "parser_bison.y"
                                                { (yyval.val) = NFT_SET_CONSTANT; }
#line 9487 "parser_bison.c"
    break;

  case 276: /* set_flag: "interval"  */
#line 2004 "parser_bison.y"
                                                { (yyval.val) = NFT_SET_INTERVAL; }
#line 9493 "parser_bison.c"
    break;

  case 277: /* set_flag: "timeout"  */
#line 2005 "parser_bison.y"
                                                { (yyval.val) = NFT_SET_TIMEOUT; }
#line 9499 "parser_bison.c"
    break;

  case 278: /* set_flag: "dynamic"  */
#line 2006 "parser_bison.y"
                                                { (yyval.val) = NFT_SET_EVAL; }
#line 9505 "parser_bison.c"
    break;

  case 279: /* map_block_alloc: %empty  */
#line 2010 "parser_bison.y"
                        {
				(yyval.set) = set_alloc(NULL);
			}
#line 9513 "parser_bison.c"
    break;

  case 280: /* map_block_obj_type: "counter" close_scope_counter  */
#line 2015 "parser_bison.y"
                                                            { (yyval.val) = NFT_OBJECT_COUNTER; }
#line 9519 "parser_bison.c"
    break;

  case 281: /* map_block_obj_type: "quota" close_scope_quota  */
#line 2016 "parser_bison.y"
                                                          { (yyval.val) = NFT_OBJECT_QUOTA; }
#line 9525 "parser_bison.c"
    break;

  case 282: /* map_block_obj_type: "limit" close_scope_limit  */
#line 2017 "parser_bison.y"
                                                          { (yyval.val) = NFT_OBJECT_LIMIT; }
#line 9531 "parser_bison.c"
    break;

  case 283: /* map_block_obj_type: "secmark" close_scope_secmark  */
#line 2018 "parser_bison.y"
                                                            { (yyval.val) = NFT_OBJECT_SECMARK; }
#line 9537 "parser_bison.c"
    break;

  case 284: /* map_block_obj_type: "synproxy"  */
#line 2019 "parser_bison.y"
                                         { (yyval.val) = NFT_OBJECT_SYNPROXY; }
#line 9543 "parser_bison.c"
    break;

  case 285: /* map_block: %empty  */
#line 2022 "parser_bison.y"
                                                { (yyval.set) = (yyvsp[(-1) - (0)].set); }
#line 9549 "parser_bison.c"
    break;

  case 288: /* map_block: map_block "timeout" time_spec stmt_separator  */
#line 2026 "parser_bison.y"
                        {
				(yyvsp[-3].set)->timeout = (yyvsp[-1].val);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 9558 "parser_bison.c"
    break;

  case 289: /* map_block: map_block "type" data_type_expr "colon" data_type_expr stmt_separator close_scope_type  */
#line 2033 "parser_bison.y"
                        {
				(yyvsp[-6].set)->key = (yyvsp[-4].expr);
				(yyvsp[-6].set)->data = (yyvsp[-2].expr);

				(yyvsp[-6].set)->flags |= NFT_SET_MAP;
				(yyval.set) = (yyvsp[-6].set);
			}
#line 9570 "parser_bison.c"
    break;

  case 290: /* map_block: map_block "type" data_type_expr "colon" "interval" data_type_expr stmt_separator close_scope_type  */
#line 2043 "parser_bison.y"
                        {
				(yyvsp[-7].set)->key = (yyvsp[-5].expr);
				(yyvsp[-7].set)->data = (yyvsp[-2].expr);
				(yyvsp[-7].set)->data->flags |= EXPR_F_INTERVAL;

				(yyvsp[-7].set)->flags |= NFT_SET_MAP;
				(yyval.set) = (yyvsp[-7].set);
			}
#line 9583 "parser_bison.c"
    break;

  case 291: /* map_block: map_block "typeof" typeof_expr "colon" typeof_data_expr stmt_separator  */
#line 2054 "parser_bison.y"
                        {
				(yyvsp[-5].set)->key = (yyvsp[-3].expr);
				datatype_set((yyvsp[-5].set)->key, (yyvsp[-3].expr)->dtype);
				(yyvsp[-5].set)->data = (yyvsp[-1].expr);

				(yyvsp[-5].set)->flags |= NFT_SET_MAP;
				(yyval.set) = (yyvsp[-5].set);
			}
#line 9596 "parser_bison.c"
    break;

  case 292: /* map_block: map_block "typeof" typeof_expr "colon" "interval" typeof_expr stmt_separator  */
#line 2065 "parser_bison.y"
                        {
				(yyvsp[-6].set)->key = (yyvsp[-4].expr);
				datatype_set((yyvsp[-6].set)->key, (yyvsp[-4].expr)->dtype);
				(yyvsp[-6].set)->data = (yyvsp[-1].expr);
				(yyvsp[-6].set)->data->flags |= EXPR_F_INTERVAL;

				(yyvsp[-6].set)->flags |= NFT_SET_MAP;
				(yyval.set) = (yyvsp[-6].set);
			}
#line 9610 "parser_bison.c"
    break;

  case 293: /* map_block: map_block "type" data_type_expr "colon" map_block_obj_type stmt_separator close_scope_type  */
#line 2077 "parser_bison.y"
                        {
				(yyvsp[-6].set)->key = (yyvsp[-4].expr);
				(yyvsp[-6].set)->objtype = (yyvsp[-2].val);
				(yyvsp[-6].set)->flags  |= NFT_SET_OBJECT;
				(yyval.set) = (yyvsp[-6].set);
			}
#line 9621 "parser_bison.c"
    break;

  case 294: /* map_block: map_block "flags" set_flag_list stmt_separator close_scope_flags  */
#line 2084 "parser_bison.y"
                        {
				(yyvsp[-4].set)->flags |= (yyvsp[-2].val);
				(yyval.set) = (yyvsp[-4].set);
			}
#line 9630 "parser_bison.c"
    break;

  case 295: /* map_block: map_block stateful_stmt_list stmt_separator  */
#line 2089 "parser_bison.y"
                        {
				list_splice_tail((yyvsp[-1].list), &(yyvsp[-2].set)->stmt_list);
				(yyval.set) = (yyvsp[-2].set);
				free((yyvsp[-1].list));
			}
#line 9640 "parser_bison.c"
    break;

  case 296: /* map_block: map_block "elements" '=' set_block_expr  */
#line 2095 "parser_bison.y"
                        {
				(yyvsp[-3].set)->init = (yyvsp[0].expr);
				(yyval.set) = (yyvsp[-3].set);
			}
#line 9649 "parser_bison.c"
    break;

  case 297: /* map_block: map_block comment_spec stmt_separator  */
#line 2100 "parser_bison.y"
                        {
				if (already_set((yyvsp[-2].set)->comment, &(yylsp[-1]), state)) {
					xfree((yyvsp[-1].string));
					YYERROR;
				}
				(yyvsp[-2].set)->comment = (yyvsp[-1].string);
				(yyval.set) = (yyvsp[-2].set);
			}
#line 9662 "parser_bison.c"
    break;

  case 299: /* set_mechanism: "policy" set_policy_spec close_scope_policy  */
#line 2112 "parser_bison.y"
                        {
				(yyvsp[-3].set)->policy = (yyvsp[-1].val);
			}
#line 9670 "parser_bison.c"
    break;

  case 300: /* set_mechanism: "size" "number"  */
#line 2116 "parser_bison.y"
                        {
				(yyvsp[-2].set)->desc.size = (yyvsp[0].val);
			}
#line 9678 "parser_bison.c"
    break;

  case 301: /* set_policy_spec: "performance"  */
#line 2121 "parser_bison.y"
                                                { (yyval.val) = NFT_SET_POL_PERFORMANCE; }
#line 9684 "parser_bison.c"
    break;

  case 302: /* set_policy_spec: "memory"  */
#line 2122 "parser_bison.y"
                                                { (yyval.val) = NFT_SET_POL_MEMORY; }
#line 9690 "parser_bison.c"
    break;

  case 303: /* flowtable_block_alloc: %empty  */
#line 2126 "parser_bison.y"
                        {
				(yyval.flowtable) = flowtable_alloc(NULL);
			}
#line 9698 "parser_bison.c"
    break;

  case 304: /* flowtable_block: %empty  */
#line 2131 "parser_bison.y"
                                                { (yyval.flowtable) = (yyvsp[(-1) - (0)].flowtable); }
#line 9704 "parser_bison.c"
    break;

  case 307: /* flowtable_block: flowtable_block "hook" "string" prio_spec stmt_separator  */
#line 2135 "parser_bison.y"
                        {
				(yyval.flowtable)->hook.loc = (yylsp[-2]);
				(yyval.flowtable)->hook.name = chain_hookname_lookup((yyvsp[-2].string));
				if ((yyval.flowtable)->hook.name == NULL) {
					erec_queue(error(&(yylsp[-2]), "unknown chain hook"),
						   state->msgs);
					xfree((yyvsp[-2].string));
					YYERROR;
				}
				xfree((yyvsp[-2].string));

				(yyval.flowtable)->priority = (yyvsp[-1].prio_spec);
			}
#line 9722 "parser_bison.c"
    break;

  case 308: /* flowtable_block: flowtable_block "devices" '=' flowtable_expr stmt_separator  */
#line 2149 "parser_bison.y"
                        {
				(yyval.flowtable)->dev_expr = (yyvsp[-1].expr);
			}
#line 9730 "parser_bison.c"
    break;

  case 309: /* flowtable_block: flowtable_block "counter" close_scope_counter  */
#line 2153 "parser_bison.y"
                        {
				(yyval.flowtable)->flags |= NFT_FLOWTABLE_COUNTER;
			}
#line 9738 "parser_bison.c"
    break;

  case 310: /* flowtable_block: flowtable_block "flags" "offload" stmt_separator close_scope_flags  */
#line 2157 "parser_bison.y"
                        {
				(yyval.flowtable)->flags |= FLOWTABLE_F_HW_OFFLOAD;
			}
#line 9746 "parser_bison.c"
    break;

  case 311: /* flowtable_expr: '{' flowtable_list_expr '}'  */
#line 2163 "parser_bison.y"
                        {
				(yyvsp[-1].expr)->location = (yyloc);
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 9755 "parser_bison.c"
    break;

  case 312: /* flowtable_expr: variable_expr  */
#line 2168 "parser_bison.y"
                        {
				(yyvsp[0].expr)->location = (yyloc);
				(yyval.expr) = (yyvsp[0].expr);
			}
#line 9764 "parser_bison.c"
    break;

  case 313: /* flowtable_list_expr: flowtable_expr_member  */
#line 2175 "parser_bison.y"
                        {
				(yyval.expr) = compound_expr_alloc(&(yyloc), EXPR_LIST);
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 9773 "parser_bison.c"
    break;

  case 314: /* flowtable_list_expr: flowtable_list_expr "comma" flowtable_expr_member  */
#line 2180 "parser_bison.y"
                        {
				compound_expr_add((yyvsp[-2].expr), (yyvsp[0].expr));
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 9782 "parser_bison.c"
    break;

  case 316: /* flowtable_expr_member: "quoted string"  */
#line 2188 "parser_bison.y"
                        {
				(yyval.expr) = constant_expr_alloc(&(yyloc), &string_type,
							 BYTEORDER_HOST_ENDIAN,
							 strlen((yyvsp[0].string)) * BITS_PER_BYTE, (yyvsp[0].string));
				xfree((yyvsp[0].string));
			}
#line 9793 "parser_bison.c"
    break;

  case 317: /* flowtable_expr_member: "string"  */
#line 2195 "parser_bison.y"
                        {
				(yyval.expr) = constant_expr_alloc(&(yyloc), &string_type,
							 BYTEORDER_HOST_ENDIAN,
							 strlen((yyvsp[0].string)) * BITS_PER_BYTE, (yyvsp[0].string));
				xfree((yyvsp[0].string));
			}
#line 9804 "parser_bison.c"
    break;

  case 318: /* flowtable_expr_member: variable_expr  */
#line 2202 "parser_bison.y"
                        {
				datatype_set((yyvsp[0].expr)->sym->expr, &ifname_type);
				(yyval.expr) = (yyvsp[0].expr);
			}
#line 9813 "parser_bison.c"
    break;

  case 319: /* data_type_atom_expr: type_identifier  */
#line 2209 "parser_bison.y"
                        {
				const struct datatype *dtype = datatype_lookup_byname((yyvsp[0].string));
				if (dtype == NULL) {
					erec_queue(error(&(yylsp[0]), "unknown datatype %s", (yyvsp[0].string)),
						   state->msgs);
					xfree((yyvsp[0].string));
					YYERROR;
				}
				(yyval.expr) = constant_expr_alloc(&(yylsp[0]), dtype, dtype->byteorder,
							 dtype->size, NULL);
				xfree((yyvsp[0].string));
			}
#line 9830 "parser_bison.c"
    break;

  case 320: /* data_type_atom_expr: "time"  */
#line 2222 "parser_bison.y"
                        {
				(yyval.expr) = constant_expr_alloc(&(yylsp[0]), &time_type, time_type.byteorder,
							 time_type.size, NULL);
			}
#line 9839 "parser_bison.c"
    break;

  case 322: /* data_type_expr: data_type_expr "." data_type_atom_expr  */
#line 2230 "parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 9852 "parser_bison.c"
    break;

  case 323: /* obj_block_alloc: %empty  */
#line 2241 "parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(NULL);
			}
#line 9860 "parser_bison.c"
    break;

  case 324: /* counter_block: %empty  */
#line 2246 "parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 9866 "parser_bison.c"
    break;

  case 327: /* counter_block: counter_block counter_config  */
#line 2250 "parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 9874 "parser_bison.c"
    break;

  case 328: /* counter_block: counter_block comment_spec  */
#line 2254 "parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					xfree((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 9886 "parser_bison.c"
    break;

  case 329: /* quota_block: %empty  */
#line 2263 "parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 9892 "parser_bison.c"
    break;

  case 332: /* quota_block: quota_block quota_config  */
#line 2267 "parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 9900 "parser_bison.c"
    break;

  case 333: /* quota_block: quota_block comment_spec  */
#line 2271 "parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					xfree((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 9912 "parser_bison.c"
    break;

  case 334: /* ct_helper_block: %empty  */
#line 2280 "parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 9918 "parser_bison.c"
    break;

  case 337: /* ct_helper_block: ct_helper_block ct_helper_config  */
#line 2284 "parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 9926 "parser_bison.c"
    break;

  case 338: /* ct_helper_block: ct_helper_block comment_spec  */
#line 2288 "parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					xfree((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 9938 "parser_bison.c"
    break;

  case 339: /* ct_timeout_block: %empty  */
#line 2298 "parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[(-1) - (0)].obj);
				init_list_head(&(yyval.obj)->ct_timeout.timeout_list);
			}
#line 9947 "parser_bison.c"
    break;

  case 342: /* ct_timeout_block: ct_timeout_block ct_timeout_config  */
#line 2305 "parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 9955 "parser_bison.c"
    break;

  case 343: /* ct_timeout_block: ct_timeout_block comment_spec  */
#line 2309 "parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					xfree((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 9967 "parser_bison.c"
    break;

  case 344: /* ct_expect_block: %empty  */
#line 2318 "parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 9973 "parser_bison.c"
    break;

  case 347: /* ct_expect_block: ct_expect_block ct_expect_config  */
#line 2322 "parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 9981 "parser_bison.c"
    break;

  case 348: /* ct_expect_block: ct_expect_block comment_spec  */
#line 2326 "parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					xfree((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 9993 "parser_bison.c"
    break;

  case 349: /* limit_block: %empty  */
#line 2335 "parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 9999 "parser_bison.c"
    break;

  case 352: /* limit_block: limit_block limit_config  */
#line 2339 "parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10007 "parser_bison.c"
    break;

  case 353: /* limit_block: limit_block comment_spec  */
#line 2343 "parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					xfree((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10019 "parser_bison.c"
    break;

  case 354: /* secmark_block: %empty  */
#line 2352 "parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 10025 "parser_bison.c"
    break;

  case 357: /* secmark_block: secmark_block secmark_config  */
#line 2356 "parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10033 "parser_bison.c"
    break;

  case 358: /* secmark_block: secmark_block comment_spec  */
#line 2360 "parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					xfree((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10045 "parser_bison.c"
    break;

  case 359: /* synproxy_block: %empty  */
#line 2369 "parser_bison.y"
                                                { (yyval.obj) = (yyvsp[(-1) - (0)].obj); }
#line 10051 "parser_bison.c"
    break;

  case 362: /* synproxy_block: synproxy_block synproxy_config  */
#line 2373 "parser_bison.y"
                        {
				(yyval.obj) = (yyvsp[-1].obj);
			}
#line 10059 "parser_bison.c"
    break;

  case 363: /* synproxy_block: synproxy_block comment_spec  */
#line 2377 "parser_bison.y"
                        {
				if (already_set((yyvsp[-1].obj)->comment, &(yylsp[0]), state)) {
					xfree((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].obj)->comment = (yyvsp[0].string);
			}
#line 10071 "parser_bison.c"
    break;

  case 364: /* type_identifier: "string"  */
#line 2386 "parser_bison.y"
                                        { (yyval.string) = (yyvsp[0].string); }
#line 10077 "parser_bison.c"
    break;

  case 365: /* type_identifier: "mark"  */
#line 2387 "parser_bison.y"
                                        { (yyval.string) = xstrdup("mark"); }
#line 10083 "parser_bison.c"
    break;

  case 366: /* type_identifier: "dscp"  */
#line 2388 "parser_bison.y"
                                        { (yyval.string) = xstrdup("dscp"); }
#line 10089 "parser_bison.c"
    break;

  case 367: /* type_identifier: "ecn"  */
#line 2389 "parser_bison.y"
                                        { (yyval.string) = xstrdup("ecn"); }
#line 10095 "parser_bison.c"
    break;

  case 368: /* type_identifier: "classid"  */
#line 2390 "parser_bison.y"
                                        { (yyval.string) = xstrdup("classid"); }
#line 10101 "parser_bison.c"
    break;

  case 369: /* hook_spec: "type" close_scope_type "string" "hook" "string" dev_spec prio_spec  */
#line 2394 "parser_bison.y"
                        {
				const char *chain_type = chain_type_name_lookup((yyvsp[-4].string));

				if (chain_type == NULL) {
					erec_queue(error(&(yylsp[-4]), "unknown chain type"),
						   state->msgs);
					xfree((yyvsp[-4].string));
					YYERROR;
				}
				(yyvsp[-7].chain)->type.loc = (yylsp[-4]);
				(yyvsp[-7].chain)->type.str = xstrdup(chain_type);
				xfree((yyvsp[-4].string));

				(yyvsp[-7].chain)->loc = (yyloc);
				(yyvsp[-7].chain)->hook.loc = (yylsp[-2]);
				(yyvsp[-7].chain)->hook.name = chain_hookname_lookup((yyvsp[-2].string));
				if ((yyvsp[-7].chain)->hook.name == NULL) {
					erec_queue(error(&(yylsp[-2]), "unknown chain hook"),
						   state->msgs);
					xfree((yyvsp[-2].string));
					YYERROR;
				}
				xfree((yyvsp[-2].string));

				(yyvsp[-7].chain)->dev_expr	= (yyvsp[-1].expr);
				(yyvsp[-7].chain)->priority	= (yyvsp[0].prio_spec);
				(yyvsp[-7].chain)->flags	|= CHAIN_F_BASECHAIN;
			}
#line 10134 "parser_bison.c"
    break;

  case 370: /* prio_spec: "priority" extended_prio_spec  */
#line 2425 "parser_bison.y"
                        {
				(yyval.prio_spec) = (yyvsp[0].prio_spec);
				(yyval.prio_spec).loc = (yyloc);
			}
#line 10143 "parser_bison.c"
    break;

  case 371: /* extended_prio_name: "out"  */
#line 2432 "parser_bison.y"
                        {
				(yyval.string) = strdup("out");
			}
#line 10151 "parser_bison.c"
    break;

  case 373: /* extended_prio_spec: int_num  */
#line 2439 "parser_bison.y"
                        {
				struct prio_spec spec = {0};

				spec.expr = constant_expr_alloc(&(yyloc), &integer_type,
								BYTEORDER_HOST_ENDIAN,
								sizeof(int) *
								BITS_PER_BYTE, &(yyvsp[0].val32));
				(yyval.prio_spec) = spec;
			}
#line 10165 "parser_bison.c"
    break;

  case 374: /* extended_prio_spec: variable_expr  */
#line 2449 "parser_bison.y"
                        {
				struct prio_spec spec = {0};

				spec.expr = (yyvsp[0].expr);
				(yyval.prio_spec) = spec;
			}
#line 10176 "parser_bison.c"
    break;

  case 375: /* extended_prio_spec: extended_prio_name  */
#line 2456 "parser_bison.y"
                        {
				struct prio_spec spec = {0};

				spec.expr = constant_expr_alloc(&(yyloc), &string_type,
								BYTEORDER_HOST_ENDIAN,
								strlen((yyvsp[0].string)) * BITS_PER_BYTE,
								(yyvsp[0].string));
				xfree((yyvsp[0].string));
				(yyval.prio_spec) = spec;
			}
#line 10191 "parser_bison.c"
    break;

  case 376: /* extended_prio_spec: extended_prio_name "+" "number"  */
#line 2467 "parser_bison.y"
                        {
				struct prio_spec spec = {0};

				char str[NFT_NAME_MAXLEN];
				snprintf(str, sizeof(str), "%s + %" PRIu64, (yyvsp[-2].string), (yyvsp[0].val));
				spec.expr = constant_expr_alloc(&(yyloc), &string_type,
								BYTEORDER_HOST_ENDIAN,
								strlen(str) * BITS_PER_BYTE,
								str);
				xfree((yyvsp[-2].string));
				(yyval.prio_spec) = spec;
			}
#line 10208 "parser_bison.c"
    break;

  case 377: /* extended_prio_spec: extended_prio_name "-" "number"  */
#line 2480 "parser_bison.y"
                        {
				struct prio_spec spec = {0};
				char str[NFT_NAME_MAXLEN];

				snprintf(str, sizeof(str), "%s - %" PRIu64, (yyvsp[-2].string), (yyvsp[0].val));
				spec.expr = constant_expr_alloc(&(yyloc), &string_type,
								BYTEORDER_HOST_ENDIAN,
								strlen(str) * BITS_PER_BYTE,
								str);
				xfree((yyvsp[-2].string));
				(yyval.prio_spec) = spec;
			}
#line 10225 "parser_bison.c"
    break;

  case 378: /* int_num: "number"  */
#line 2494 "parser_bison.y"
                                                        { (yyval.val32) = (yyvsp[0].val); }
#line 10231 "parser_bison.c"
    break;

  case 379: /* int_num: "-" "number"  */
#line 2495 "parser_bison.y"
                                                        { (yyval.val32) = -(yyvsp[0].val); }
#line 10237 "parser_bison.c"
    break;

  case 380: /* dev_spec: "device" string  */
#line 2499 "parser_bison.y"
                        {
				struct expr *expr;

				expr = constant_expr_alloc(&(yyloc), &string_type,
							   BYTEORDER_HOST_ENDIAN,
							   strlen((yyvsp[0].string)) * BITS_PER_BYTE, (yyvsp[0].string));
				xfree((yyvsp[0].string));
				(yyval.expr) = compound_expr_alloc(&(yyloc), EXPR_LIST);
				compound_expr_add((yyval.expr), expr);

			}
#line 10253 "parser_bison.c"
    break;

  case 381: /* dev_spec: "device" variable_expr  */
#line 2511 "parser_bison.y"
                        {
				datatype_set((yyvsp[0].expr)->sym->expr, &ifname_type);
				(yyval.expr) = compound_expr_alloc(&(yyloc), EXPR_LIST);
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 10263 "parser_bison.c"
    break;

  case 382: /* dev_spec: "devices" '=' flowtable_expr  */
#line 2517 "parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[0].expr);
			}
#line 10271 "parser_bison.c"
    break;

  case 383: /* dev_spec: %empty  */
#line 2520 "parser_bison.y"
                                                        { (yyval.expr) = NULL; }
#line 10277 "parser_bison.c"
    break;

  case 384: /* flags_spec: "flags" "offload" close_scope_flags  */
#line 2524 "parser_bison.y"
                        {
				(yyvsp[-3].chain)->flags |= CHAIN_F_HW_OFFLOAD;
			}
#line 10285 "parser_bison.c"
    break;

  case 385: /* policy_spec: "policy" policy_expr close_scope_policy  */
#line 2530 "parser_bison.y"
                        {
				if ((yyvsp[-3].chain)->policy) {
					erec_queue(error(&(yyloc), "you cannot set chain policy twice"),
						   state->msgs);
					expr_free((yyvsp[-1].expr));
					YYERROR;
				}
				(yyvsp[-3].chain)->policy		= (yyvsp[-1].expr);
				(yyvsp[-3].chain)->policy->location	= (yyloc);
			}
#line 10300 "parser_bison.c"
    break;

  case 386: /* policy_expr: variable_expr  */
#line 2543 "parser_bison.y"
                        {
				datatype_set((yyvsp[0].expr)->sym->expr, &policy_type);
				(yyval.expr) = (yyvsp[0].expr);
			}
#line 10309 "parser_bison.c"
    break;

  case 387: /* policy_expr: chain_policy  */
#line 2548 "parser_bison.y"
                        {
				(yyval.expr) = constant_expr_alloc(&(yyloc), &integer_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(int) *
							 BITS_PER_BYTE, &(yyvsp[0].val32));
			}
#line 10320 "parser_bison.c"
    break;

  case 388: /* chain_policy: "accept"  */
#line 2556 "parser_bison.y"
                                                { (yyval.val32) = NF_ACCEPT; }
#line 10326 "parser_bison.c"
    break;

  case 389: /* chain_policy: "drop"  */
#line 2557 "parser_bison.y"
                                                { (yyval.val32) = NF_DROP;   }
#line 10332 "parser_bison.c"
    break;

  case 394: /* time_spec: "string"  */
#line 2569 "parser_bison.y"
                        {
				struct error_record *erec;
				uint64_t res;

				erec = time_parse(&(yylsp[0]), (yyvsp[0].string), &res);
				xfree((yyvsp[0].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				(yyval.val) = res;
			}
#line 10349 "parser_bison.c"
    break;

  case 395: /* family_spec: %empty  */
#line 2583 "parser_bison.y"
                                                        { (yyval.val) = NFPROTO_IPV4; }
#line 10355 "parser_bison.c"
    break;

  case 397: /* family_spec_explicit: "ip" close_scope_ip  */
#line 2587 "parser_bison.y"
                                                        { (yyval.val) = NFPROTO_IPV4; }
#line 10361 "parser_bison.c"
    break;

  case 398: /* family_spec_explicit: "ip6" close_scope_ip6  */
#line 2588 "parser_bison.y"
                                                        { (yyval.val) = NFPROTO_IPV6; }
#line 10367 "parser_bison.c"
    break;

  case 399: /* family_spec_explicit: "inet"  */
#line 2589 "parser_bison.y"
                                                        { (yyval.val) = NFPROTO_INET; }
#line 10373 "parser_bison.c"
    break;

  case 400: /* family_spec_explicit: "arp" close_scope_arp  */
#line 2590 "parser_bison.y"
                                                        { (yyval.val) = NFPROTO_ARP; }
#line 10379 "parser_bison.c"
    break;

  case 401: /* family_spec_explicit: "bridge"  */
#line 2591 "parser_bison.y"
                                                        { (yyval.val) = NFPROTO_BRIDGE; }
#line 10385 "parser_bison.c"
    break;

  case 402: /* family_spec_explicit: "netdev"  */
#line 2592 "parser_bison.y"
                                                        { (yyval.val) = NFPROTO_NETDEV; }
#line 10391 "parser_bison.c"
    break;

  case 403: /* table_spec: family_spec identifier  */
#line 2596 "parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).family	= (yyvsp[-1].val);
				(yyval.handle).table.location = (yylsp[0]);
				(yyval.handle).table.name	= (yyvsp[0].string);
			}
#line 10402 "parser_bison.c"
    break;

  case 404: /* tableid_spec: family_spec "handle" "number"  */
#line 2605 "parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).family 		= (yyvsp[-2].val);
				(yyval.handle).handle.id 		= (yyvsp[0].val);
				(yyval.handle).handle.location	= (yylsp[0]);
			}
#line 10413 "parser_bison.c"
    break;

  case 405: /* chain_spec: table_spec identifier  */
#line 2614 "parser_bison.y"
                        {
				(yyval.handle)		= (yyvsp[-1].handle);
				(yyval.handle).chain.name	= (yyvsp[0].string);
				(yyval.handle).chain.location = (yylsp[0]);
			}
#line 10423 "parser_bison.c"
    break;

  case 406: /* chainid_spec: table_spec "handle" "number"  */
#line 2622 "parser_bison.y"
                        {
				(yyval.handle) 			= (yyvsp[-2].handle);
				(yyval.handle).handle.location 	= (yylsp[0]);
				(yyval.handle).handle.id 		= (yyvsp[0].val);
			}
#line 10433 "parser_bison.c"
    break;

  case 407: /* chain_identifier: identifier  */
#line 2630 "parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).chain.name		= (yyvsp[0].string);
				(yyval.handle).chain.location	= (yylsp[0]);
			}
#line 10443 "parser_bison.c"
    break;

  case 408: /* set_spec: table_spec identifier  */
#line 2638 "parser_bison.y"
                        {
				(yyval.handle)		= (yyvsp[-1].handle);
				(yyval.handle).set.name	= (yyvsp[0].string);
				(yyval.handle).set.location	= (yylsp[0]);
			}
#line 10453 "parser_bison.c"
    break;

  case 409: /* setid_spec: table_spec "handle" "number"  */
#line 2646 "parser_bison.y"
                        {
				(yyval.handle) 			= (yyvsp[-2].handle);
				(yyval.handle).handle.location 	= (yylsp[0]);
				(yyval.handle).handle.id 		= (yyvsp[0].val);
			}
#line 10463 "parser_bison.c"
    break;

  case 410: /* set_identifier: identifier  */
#line 2654 "parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).set.name	= (yyvsp[0].string);
				(yyval.handle).set.location	= (yylsp[0]);
			}
#line 10473 "parser_bison.c"
    break;

  case 411: /* flowtable_spec: table_spec identifier  */
#line 2662 "parser_bison.y"
                        {
				(yyval.handle)			= (yyvsp[-1].handle);
				(yyval.handle).flowtable.name	= (yyvsp[0].string);
				(yyval.handle).flowtable.location	= (yylsp[0]);
			}
#line 10483 "parser_bison.c"
    break;

  case 412: /* flowtableid_spec: table_spec "handle" "number"  */
#line 2670 "parser_bison.y"
                        {
				(yyval.handle)			= (yyvsp[-2].handle);
				(yyval.handle).handle.location	= (yylsp[0]);
				(yyval.handle).handle.id		= (yyvsp[0].val);
			}
#line 10493 "parser_bison.c"
    break;

  case 413: /* flowtable_identifier: identifier  */
#line 2678 "parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).flowtable.name	= (yyvsp[0].string);
				(yyval.handle).flowtable.location	= (yylsp[0]);
			}
#line 10503 "parser_bison.c"
    break;

  case 414: /* obj_spec: table_spec identifier  */
#line 2686 "parser_bison.y"
                        {
				(yyval.handle)		= (yyvsp[-1].handle);
				(yyval.handle).obj.name	= (yyvsp[0].string);
				(yyval.handle).obj.location	= (yylsp[0]);
			}
#line 10513 "parser_bison.c"
    break;

  case 415: /* objid_spec: table_spec "handle" "number"  */
#line 2694 "parser_bison.y"
                        {
				(yyval.handle) 			= (yyvsp[-2].handle);
				(yyval.handle).handle.location	= (yylsp[0]);
				(yyval.handle).handle.id		= (yyvsp[0].val);
			}
#line 10523 "parser_bison.c"
    break;

  case 416: /* obj_identifier: identifier  */
#line 2702 "parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).obj.name		= (yyvsp[0].string);
				(yyval.handle).obj.location		= (yylsp[0]);
			}
#line 10533 "parser_bison.c"
    break;

  case 417: /* handle_spec: "handle" "number"  */
#line 2710 "parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).handle.location	= (yylsp[0]);
				(yyval.handle).handle.id		= (yyvsp[0].val);
			}
#line 10543 "parser_bison.c"
    break;

  case 418: /* position_spec: "position" "number"  */
#line 2718 "parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).position.location	= (yyloc);
				(yyval.handle).position.id		= (yyvsp[0].val);
			}
#line 10553 "parser_bison.c"
    break;

  case 419: /* index_spec: "index" "number"  */
#line 2726 "parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).index.location	= (yyloc);
				(yyval.handle).index.id		= (yyvsp[0].val) + 1;
			}
#line 10563 "parser_bison.c"
    break;

  case 420: /* rule_position: chain_spec  */
#line 2734 "parser_bison.y"
                        {
				(yyval.handle) = (yyvsp[0].handle);
			}
#line 10571 "parser_bison.c"
    break;

  case 421: /* rule_position: chain_spec position_spec  */
#line 2738 "parser_bison.y"
                        {
				handle_merge(&(yyvsp[-1].handle), &(yyvsp[0].handle));
				(yyval.handle) = (yyvsp[-1].handle);
			}
#line 10580 "parser_bison.c"
    break;

  case 422: /* rule_position: chain_spec handle_spec  */
#line 2743 "parser_bison.y"
                        {
				(yyvsp[0].handle).position.location = (yyvsp[0].handle).handle.location;
				(yyvsp[0].handle).position.id = (yyvsp[0].handle).handle.id;
				(yyvsp[0].handle).handle.id = 0;
				handle_merge(&(yyvsp[-1].handle), &(yyvsp[0].handle));
				(yyval.handle) = (yyvsp[-1].handle);
			}
#line 10592 "parser_bison.c"
    break;

  case 423: /* rule_position: chain_spec index_spec  */
#line 2751 "parser_bison.y"
                        {
				handle_merge(&(yyvsp[-1].handle), &(yyvsp[0].handle));
				(yyval.handle) = (yyvsp[-1].handle);
			}
#line 10601 "parser_bison.c"
    break;

  case 424: /* ruleid_spec: chain_spec handle_spec  */
#line 2758 "parser_bison.y"
                        {
				handle_merge(&(yyvsp[-1].handle), &(yyvsp[0].handle));
				(yyval.handle) = (yyvsp[-1].handle);
			}
#line 10610 "parser_bison.c"
    break;

  case 425: /* comment_spec: "comment" string  */
#line 2765 "parser_bison.y"
                        {
				if (strlen((yyvsp[0].string)) > NFTNL_UDATA_COMMENT_MAXLEN) {
					erec_queue(error(&(yylsp[0]), "comment too long, %d characters maximum allowed",
							 NFTNL_UDATA_COMMENT_MAXLEN),
						   state->msgs);
					xfree((yyvsp[0].string));
					YYERROR;
				}
				(yyval.string) = (yyvsp[0].string);
			}
#line 10625 "parser_bison.c"
    break;

  case 426: /* ruleset_spec: %empty  */
#line 2778 "parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).family	= NFPROTO_UNSPEC;
			}
#line 10634 "parser_bison.c"
    break;

  case 427: /* ruleset_spec: family_spec_explicit  */
#line 2783 "parser_bison.y"
                        {
				memset(&(yyval.handle), 0, sizeof((yyval.handle)));
				(yyval.handle).family	= (yyvsp[0].val);
			}
#line 10643 "parser_bison.c"
    break;

  case 428: /* rule: rule_alloc  */
#line 2790 "parser_bison.y"
                        {
				(yyval.rule)->comment = NULL;
			}
#line 10651 "parser_bison.c"
    break;

  case 429: /* rule: rule_alloc comment_spec  */
#line 2794 "parser_bison.y"
                        {
				(yyval.rule)->comment = (yyvsp[0].string);
			}
#line 10659 "parser_bison.c"
    break;

  case 430: /* rule_alloc: stmt_list  */
#line 2800 "parser_bison.y"
                        {
				struct stmt *i;

				(yyval.rule) = rule_alloc(&(yyloc), NULL);
				list_for_each_entry(i, (yyvsp[0].list), list)
					(yyval.rule)->num_stmts++;
				list_splice_tail((yyvsp[0].list), &(yyval.rule)->stmts);
				xfree((yyvsp[0].list));
			}
#line 10673 "parser_bison.c"
    break;

  case 431: /* stmt_list: stmt  */
#line 2812 "parser_bison.y"
                        {
				(yyval.list) = xmalloc(sizeof(*(yyval.list)));
				init_list_head((yyval.list));
				list_add_tail(&(yyvsp[0].stmt)->list, (yyval.list));
			}
#line 10683 "parser_bison.c"
    break;

  case 432: /* stmt_list: stmt_list stmt  */
#line 2818 "parser_bison.y"
                        {
				(yyval.list) = (yyvsp[-1].list);
				list_add_tail(&(yyvsp[0].stmt)->list, (yyvsp[-1].list));
			}
#line 10692 "parser_bison.c"
    break;

  case 433: /* stateful_stmt_list: stateful_stmt  */
#line 2825 "parser_bison.y"
                        {
				(yyval.list) = xmalloc(sizeof(*(yyval.list)));
				init_list_head((yyval.list));
				list_add_tail(&(yyvsp[0].stmt)->list, (yyval.list));
			}
#line 10702 "parser_bison.c"
    break;

  case 434: /* stateful_stmt_list: stateful_stmt_list stateful_stmt  */
#line 2831 "parser_bison.y"
                        {
				(yyval.list) = (yyvsp[-1].list);
				list_add_tail(&(yyvsp[0].stmt)->list, (yyvsp[-1].list));
			}
#line 10711 "parser_bison.c"
    break;

  case 460: /* chain_stmt_type: "jump"  */
#line 2866 "parser_bison.y"
                                        { (yyval.val) = NFT_JUMP; }
#line 10717 "parser_bison.c"
    break;

  case 461: /* chain_stmt_type: "goto"  */
#line 2867 "parser_bison.y"
                                        { (yyval.val) = NFT_GOTO; }
#line 10723 "parser_bison.c"
    break;

  case 462: /* chain_stmt: chain_stmt_type chain_block_alloc '{' subchain_block '}'  */
#line 2871 "parser_bison.y"
                        {
				(yyvsp[-3].chain)->location = (yylsp[-3]);
				close_scope(state);
				(yyvsp[-1].chain)->location = (yylsp[-1]);
				(yyval.stmt) = chain_stmt_alloc(&(yyloc), (yyvsp[-1].chain), (yyvsp[-4].val));
			}
#line 10734 "parser_bison.c"
    break;

  case 463: /* verdict_stmt: verdict_expr  */
#line 2880 "parser_bison.y"
                        {
				(yyval.stmt) = verdict_stmt_alloc(&(yyloc), (yyvsp[0].expr));
			}
#line 10742 "parser_bison.c"
    break;

  case 464: /* verdict_stmt: verdict_map_stmt  */
#line 2884 "parser_bison.y"
                        {
				(yyval.stmt) = verdict_stmt_alloc(&(yyloc), (yyvsp[0].expr));
			}
#line 10750 "parser_bison.c"
    break;

  case 465: /* verdict_map_stmt: concat_expr "vmap" verdict_map_expr  */
#line 2890 "parser_bison.y"
                        {
				(yyval.expr) = map_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 10758 "parser_bison.c"
    break;

  case 466: /* verdict_map_expr: '{' verdict_map_list_expr '}'  */
#line 2896 "parser_bison.y"
                        {
				(yyvsp[-1].expr)->location = (yyloc);
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 10767 "parser_bison.c"
    break;

  case 468: /* verdict_map_list_expr: verdict_map_list_member_expr  */
#line 2904 "parser_bison.y"
                        {
				(yyval.expr) = set_expr_alloc(&(yyloc), NULL);
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 10776 "parser_bison.c"
    break;

  case 469: /* verdict_map_list_expr: verdict_map_list_expr "comma" verdict_map_list_member_expr  */
#line 2909 "parser_bison.y"
                        {
				compound_expr_add((yyvsp[-2].expr), (yyvsp[0].expr));
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 10785 "parser_bison.c"
    break;

  case 471: /* verdict_map_list_member_expr: opt_newline set_elem_expr "colon" verdict_expr opt_newline  */
#line 2917 "parser_bison.y"
                        {
				(yyval.expr) = mapping_expr_alloc(&(yyloc), (yyvsp[-3].expr), (yyvsp[-1].expr));
			}
#line 10793 "parser_bison.c"
    break;

  case 472: /* connlimit_stmt: "ct" "count" "number" close_scope_ct  */
#line 2923 "parser_bison.y"
                        {
				(yyval.stmt) = connlimit_stmt_alloc(&(yyloc));
				(yyval.stmt)->connlimit.count	= (yyvsp[-1].val);
			}
#line 10802 "parser_bison.c"
    break;

  case 473: /* connlimit_stmt: "ct" "count" "over" "number" close_scope_ct  */
#line 2928 "parser_bison.y"
                        {
				(yyval.stmt) = connlimit_stmt_alloc(&(yyloc));
				(yyval.stmt)->connlimit.count = (yyvsp[-1].val);
				(yyval.stmt)->connlimit.flags = NFT_CONNLIMIT_F_INV;
			}
#line 10812 "parser_bison.c"
    break;

  case 476: /* counter_stmt_alloc: "counter"  */
#line 2939 "parser_bison.y"
                        {
				(yyval.stmt) = counter_stmt_alloc(&(yyloc));
			}
#line 10820 "parser_bison.c"
    break;

  case 477: /* counter_stmt_alloc: "counter" "name" stmt_expr  */
#line 2943 "parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_COUNTER;
				(yyval.stmt)->objref.expr = (yyvsp[0].expr);
			}
#line 10830 "parser_bison.c"
    break;

  case 478: /* counter_args: counter_arg  */
#line 2951 "parser_bison.y"
                        {
				(yyval.stmt)	= (yyvsp[-1].stmt);
			}
#line 10838 "parser_bison.c"
    break;

  case 480: /* counter_arg: "packets" "number"  */
#line 2958 "parser_bison.y"
                        {
				(yyvsp[-2].stmt)->counter.packets = (yyvsp[0].val);
			}
#line 10846 "parser_bison.c"
    break;

  case 481: /* counter_arg: "bytes" "number"  */
#line 2962 "parser_bison.y"
                        {
				(yyvsp[-2].stmt)->counter.bytes	 = (yyvsp[0].val);
			}
#line 10854 "parser_bison.c"
    break;

  case 484: /* log_stmt_alloc: "log"  */
#line 2972 "parser_bison.y"
                        {
				(yyval.stmt) = log_stmt_alloc(&(yyloc));
			}
#line 10862 "parser_bison.c"
    break;

  case 485: /* log_args: log_arg  */
#line 2978 "parser_bison.y"
                        {
				(yyval.stmt)	= (yyvsp[-1].stmt);
			}
#line 10870 "parser_bison.c"
    break;

  case 487: /* log_arg: "prefix" string  */
#line 2985 "parser_bison.y"
                        {
				struct scope *scope = current_scope(state);
				bool done = false, another_var = false;
				char *start, *end, scratch = '\0';
				struct expr *expr, *item;
				struct symbol *sym;
				enum {
					PARSE_TEXT,
					PARSE_VAR,
				} prefix_state;

				/* No variables in log prefix, skip. */
				if (!strchr((yyvsp[0].string), '$')) {
					expr = constant_expr_alloc(&(yyloc), &string_type,
								   BYTEORDER_HOST_ENDIAN,
								   (strlen((yyvsp[0].string)) + 1) * BITS_PER_BYTE, (yyvsp[0].string));
					xfree((yyvsp[0].string));
					(yyvsp[-2].stmt)->log.prefix = expr;
					(yyvsp[-2].stmt)->log.flags |= STMT_LOG_PREFIX;
					break;
				}

				/* Parse variables in log prefix string using a
				 * state machine parser with two states. This
				 * parser creates list of expressions composed
				 * of constant and variable expressions.
				 */
				expr = compound_expr_alloc(&(yyloc), EXPR_LIST);

				start = (char *)(yyvsp[0].string);

				if (*start != '$') {
					prefix_state = PARSE_TEXT;
				} else {
					prefix_state = PARSE_VAR;
					start++;
				}
				end = start;

				/* Not nice, but works. */
				while (!done) {
					switch (prefix_state) {
					case PARSE_TEXT:
						while (*end != '\0' && *end != '$')
							end++;

						if (*end == '\0')
							done = true;

						*end = '\0';
						item = constant_expr_alloc(&(yyloc), &string_type,
									   BYTEORDER_HOST_ENDIAN,
									   (strlen(start) + 1) * BITS_PER_BYTE,
									   start);
						compound_expr_add(expr, item);

						if (done)
							break;

						start = end + 1;
						end = start;

						/* fall through */
					case PARSE_VAR:
						while (isalnum(*end) || *end == '_')
							end++;

						if (*end == '\0')
							done = true;
						else if (*end == '$')
							another_var = true;
						else
							scratch = *end;

						*end = '\0';

						sym = symbol_get(scope, start);
						if (!sym) {
							sym = symbol_lookup_fuzzy(scope, start);
							if (sym) {
								erec_queue(error(&(yylsp[0]), "unknown identifier '%s'; "
										 "did you mean identifier ‘%s’?",
										 start, sym->identifier),
									   state->msgs);
							} else {
								erec_queue(error(&(yylsp[0]), "unknown identifier '%s'",
										 start),
									   state->msgs);
							}
							expr_free(expr);
							xfree((yyvsp[0].string));
							YYERROR;
						}
						item = variable_expr_alloc(&(yyloc), scope, sym);
						compound_expr_add(expr, item);

						if (done)
							break;

						/* Restore original byte after
						 * symbol lookup.
						 */
						if (scratch) {
							*end = scratch;
							scratch = '\0';
						}

						start = end;
						if (another_var) {
							another_var = false;
							start++;
							prefix_state = PARSE_VAR;
						} else {
							prefix_state = PARSE_TEXT;
						}
						end = start;
						break;
					}
				}

				xfree((yyvsp[0].string));
				(yyvsp[-2].stmt)->log.prefix	 = expr;
				(yyvsp[-2].stmt)->log.flags 	|= STMT_LOG_PREFIX;
			}
#line 10999 "parser_bison.c"
    break;

  case 488: /* log_arg: "group" "number"  */
#line 3110 "parser_bison.y"
                        {
				(yyvsp[-2].stmt)->log.group	 = (yyvsp[0].val);
				(yyvsp[-2].stmt)->log.flags 	|= STMT_LOG_GROUP;
			}
#line 11008 "parser_bison.c"
    break;

  case 489: /* log_arg: "snaplen" "number"  */
#line 3115 "parser_bison.y"
                        {
				(yyvsp[-2].stmt)->log.snaplen	 = (yyvsp[0].val);
				(yyvsp[-2].stmt)->log.flags 	|= STMT_LOG_SNAPLEN;
			}
#line 11017 "parser_bison.c"
    break;

  case 490: /* log_arg: "queue-threshold" "number"  */
#line 3120 "parser_bison.y"
                        {
				(yyvsp[-2].stmt)->log.qthreshold = (yyvsp[0].val);
				(yyvsp[-2].stmt)->log.flags 	|= STMT_LOG_QTHRESHOLD;
			}
#line 11026 "parser_bison.c"
    break;

  case 491: /* log_arg: "level" level_type  */
#line 3125 "parser_bison.y"
                        {
				(yyvsp[-2].stmt)->log.level	= (yyvsp[0].val);
				(yyvsp[-2].stmt)->log.flags 	|= STMT_LOG_LEVEL;
			}
#line 11035 "parser_bison.c"
    break;

  case 492: /* log_arg: "flags" log_flags close_scope_flags  */
#line 3130 "parser_bison.y"
                        {
				(yyvsp[-3].stmt)->log.logflags	|= (yyvsp[-1].val);
			}
#line 11043 "parser_bison.c"
    break;

  case 493: /* level_type: string  */
#line 3136 "parser_bison.y"
                        {
				if (!strcmp("emerg", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_EMERG;
				else if (!strcmp("alert", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_ALERT;
				else if (!strcmp("crit", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_CRIT;
				else if (!strcmp("err", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_ERR;
				else if (!strcmp("warn", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_WARNING;
				else if (!strcmp("notice", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_NOTICE;
				else if (!strcmp("info", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_INFO;
				else if (!strcmp("debug", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_DEBUG;
				else if (!strcmp("audit", (yyvsp[0].string)))
					(yyval.val) = NFT_LOGLEVEL_AUDIT;
				else {
					erec_queue(error(&(yylsp[0]), "invalid log level"),
						   state->msgs);
					xfree((yyvsp[0].string));
					YYERROR;
				}
				xfree((yyvsp[0].string));
			}
#line 11075 "parser_bison.c"
    break;

  case 494: /* log_flags: "tcp" log_flags_tcp close_scope_tcp  */
#line 3166 "parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-1].val);
			}
#line 11083 "parser_bison.c"
    break;

  case 495: /* log_flags: "ip" "options" close_scope_ip  */
#line 3170 "parser_bison.y"
                        {
				(yyval.val) = NF_LOG_IPOPT;
			}
#line 11091 "parser_bison.c"
    break;

  case 496: /* log_flags: "skuid"  */
#line 3174 "parser_bison.y"
                        {
				(yyval.val) = NF_LOG_UID;
			}
#line 11099 "parser_bison.c"
    break;

  case 497: /* log_flags: "ether" close_scope_eth  */
#line 3178 "parser_bison.y"
                        {
				(yyval.val) = NF_LOG_MACDECODE;
			}
#line 11107 "parser_bison.c"
    break;

  case 498: /* log_flags: "all"  */
#line 3182 "parser_bison.y"
                        {
				(yyval.val) = NF_LOG_MASK;
			}
#line 11115 "parser_bison.c"
    break;

  case 499: /* log_flags_tcp: log_flags_tcp "comma" log_flag_tcp  */
#line 3188 "parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-2].val) | (yyvsp[0].val);
			}
#line 11123 "parser_bison.c"
    break;

  case 501: /* log_flag_tcp: "seq"  */
#line 3195 "parser_bison.y"
                        {
				(yyval.val) = NF_LOG_TCPSEQ;
			}
#line 11131 "parser_bison.c"
    break;

  case 502: /* log_flag_tcp: "options"  */
#line 3199 "parser_bison.y"
                        {
				(yyval.val) = NF_LOG_TCPOPT;
			}
#line 11139 "parser_bison.c"
    break;

  case 503: /* limit_stmt: "limit" "rate" limit_mode limit_rate_pkts limit_burst_pkts close_scope_limit  */
#line 3205 "parser_bison.y"
                        {
				if ((yyvsp[-1].val) == 0) {
					erec_queue(error(&(yylsp[-1]), "limit burst must be > 0"),
						   state->msgs);
					YYERROR;
				}
				(yyval.stmt) = limit_stmt_alloc(&(yyloc));
				(yyval.stmt)->limit.rate	= (yyvsp[-2].limit_rate).rate;
				(yyval.stmt)->limit.unit	= (yyvsp[-2].limit_rate).unit;
				(yyval.stmt)->limit.burst	= (yyvsp[-1].val);
				(yyval.stmt)->limit.type	= NFT_LIMIT_PKTS;
				(yyval.stmt)->limit.flags = (yyvsp[-3].val);
			}
#line 11157 "parser_bison.c"
    break;

  case 504: /* limit_stmt: "limit" "rate" limit_mode limit_rate_bytes limit_burst_bytes close_scope_limit  */
#line 3219 "parser_bison.y"
                        {
				if ((yyvsp[-1].val) == 0) {
					erec_queue(error(&(yylsp[-1]), "limit burst must be > 0"),
						   state->msgs);
					YYERROR;
				}
				(yyval.stmt) = limit_stmt_alloc(&(yyloc));
				(yyval.stmt)->limit.rate	= (yyvsp[-2].limit_rate).rate;
				(yyval.stmt)->limit.unit	= (yyvsp[-2].limit_rate).unit;
				(yyval.stmt)->limit.burst	= (yyvsp[-1].val);
				(yyval.stmt)->limit.type	= NFT_LIMIT_PKT_BYTES;
				(yyval.stmt)->limit.flags = (yyvsp[-3].val);
			}
#line 11175 "parser_bison.c"
    break;

  case 505: /* limit_stmt: "limit" "name" stmt_expr close_scope_limit  */
#line 3233 "parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_LIMIT;
				(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
			}
#line 11185 "parser_bison.c"
    break;

  case 506: /* quota_mode: "over"  */
#line 3240 "parser_bison.y"
                                                { (yyval.val) = NFT_QUOTA_F_INV; }
#line 11191 "parser_bison.c"
    break;

  case 507: /* quota_mode: "until"  */
#line 3241 "parser_bison.y"
                                                { (yyval.val) = 0; }
#line 11197 "parser_bison.c"
    break;

  case 508: /* quota_mode: %empty  */
#line 3242 "parser_bison.y"
                                                { (yyval.val) = 0; }
#line 11203 "parser_bison.c"
    break;

  case 509: /* quota_unit: "bytes"  */
#line 3245 "parser_bison.y"
                                                { (yyval.string) = xstrdup("bytes"); }
#line 11209 "parser_bison.c"
    break;

  case 510: /* quota_unit: "string"  */
#line 3246 "parser_bison.y"
                                                { (yyval.string) = (yyvsp[0].string); }
#line 11215 "parser_bison.c"
    break;

  case 511: /* quota_used: %empty  */
#line 3249 "parser_bison.y"
                                                { (yyval.val) = 0; }
#line 11221 "parser_bison.c"
    break;

  case 512: /* quota_used: "used" "number" quota_unit  */
#line 3251 "parser_bison.y"
                        {
				struct error_record *erec;
				uint64_t rate;

				erec = data_unit_parse(&(yyloc), (yyvsp[0].string), &rate);
				xfree((yyvsp[0].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				(yyval.val) = (yyvsp[-1].val) * rate;
			}
#line 11238 "parser_bison.c"
    break;

  case 513: /* quota_stmt: "quota" quota_mode "number" quota_unit quota_used close_scope_quota  */
#line 3266 "parser_bison.y"
                        {
				struct error_record *erec;
				uint64_t rate;

				erec = data_unit_parse(&(yyloc), (yyvsp[-2].string), &rate);
				xfree((yyvsp[-2].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				(yyval.stmt) = quota_stmt_alloc(&(yyloc));
				(yyval.stmt)->quota.bytes	= (yyvsp[-3].val) * rate;
				(yyval.stmt)->quota.used = (yyvsp[-1].val);
				(yyval.stmt)->quota.flags	= (yyvsp[-4].val);
			}
#line 11258 "parser_bison.c"
    break;

  case 514: /* quota_stmt: "quota" "name" stmt_expr close_scope_quota  */
#line 3282 "parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_QUOTA;
				(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
			}
#line 11268 "parser_bison.c"
    break;

  case 515: /* limit_mode: "over"  */
#line 3289 "parser_bison.y"
                                                                { (yyval.val) = NFT_LIMIT_F_INV; }
#line 11274 "parser_bison.c"
    break;

  case 516: /* limit_mode: "until"  */
#line 3290 "parser_bison.y"
                                                                { (yyval.val) = 0; }
#line 11280 "parser_bison.c"
    break;

  case 517: /* limit_mode: %empty  */
#line 3291 "parser_bison.y"
                                                                { (yyval.val) = 0; }
#line 11286 "parser_bison.c"
    break;

  case 518: /* limit_burst_pkts: %empty  */
#line 3294 "parser_bison.y"
                                                                { (yyval.val) = 5; }
#line 11292 "parser_bison.c"
    break;

  case 519: /* limit_burst_pkts: "burst" "number" "packets"  */
#line 3295 "parser_bison.y"
                                                                { (yyval.val) = (yyvsp[-1].val); }
#line 11298 "parser_bison.c"
    break;

  case 520: /* limit_rate_pkts: "number" "/" time_unit  */
#line 3299 "parser_bison.y"
                        {
				(yyval.limit_rate).rate = (yyvsp[-2].val);
				(yyval.limit_rate).unit = (yyvsp[0].val);
			}
#line 11307 "parser_bison.c"
    break;

  case 521: /* limit_burst_bytes: %empty  */
#line 3305 "parser_bison.y"
                                                                { (yyval.val) = 5; }
#line 11313 "parser_bison.c"
    break;

  case 522: /* limit_burst_bytes: "burst" limit_bytes  */
#line 3306 "parser_bison.y"
                                                                { (yyval.val) = (yyvsp[0].val); }
#line 11319 "parser_bison.c"
    break;

  case 523: /* limit_rate_bytes: "number" "string"  */
#line 3310 "parser_bison.y"
                        {
				struct error_record *erec;
				uint64_t rate, unit;

				erec = rate_parse(&(yyloc), (yyvsp[0].string), &rate, &unit);
				xfree((yyvsp[0].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				(yyval.limit_rate).rate = rate * (yyvsp[-1].val);
				(yyval.limit_rate).unit = unit;
			}
#line 11337 "parser_bison.c"
    break;

  case 524: /* limit_rate_bytes: limit_bytes "/" time_unit  */
#line 3324 "parser_bison.y"
                        {
				(yyval.limit_rate).rate = (yyvsp[-2].val);
				(yyval.limit_rate).unit = (yyvsp[0].val);
			}
#line 11346 "parser_bison.c"
    break;

  case 525: /* limit_bytes: "number" "bytes"  */
#line 3330 "parser_bison.y"
                                                        { (yyval.val) = (yyvsp[-1].val); }
#line 11352 "parser_bison.c"
    break;

  case 526: /* limit_bytes: "number" "string"  */
#line 3332 "parser_bison.y"
                        {
				struct error_record *erec;
				uint64_t rate;

				erec = data_unit_parse(&(yyloc), (yyvsp[0].string), &rate);
				xfree((yyvsp[0].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}
				(yyval.val) = (yyvsp[-1].val) * rate;
			}
#line 11369 "parser_bison.c"
    break;

  case 527: /* time_unit: "second"  */
#line 3346 "parser_bison.y"
                                                { (yyval.val) = 1ULL; }
#line 11375 "parser_bison.c"
    break;

  case 528: /* time_unit: "minute"  */
#line 3347 "parser_bison.y"
                                                { (yyval.val) = 1ULL * 60; }
#line 11381 "parser_bison.c"
    break;

  case 529: /* time_unit: "hour"  */
#line 3348 "parser_bison.y"
                                                { (yyval.val) = 1ULL * 60 * 60; }
#line 11387 "parser_bison.c"
    break;

  case 530: /* time_unit: "day"  */
#line 3349 "parser_bison.y"
                                                { (yyval.val) = 1ULL * 60 * 60 * 24; }
#line 11393 "parser_bison.c"
    break;

  case 531: /* time_unit: "week"  */
#line 3350 "parser_bison.y"
                                                { (yyval.val) = 1ULL * 60 * 60 * 24 * 7; }
#line 11399 "parser_bison.c"
    break;

  case 533: /* reject_stmt_alloc: "reject"  */
#line 3357 "parser_bison.y"
                        {
				(yyval.stmt) = reject_stmt_alloc(&(yyloc));
			}
#line 11407 "parser_bison.c"
    break;

  case 534: /* reject_with_expr: "string"  */
#line 3363 "parser_bison.y"
                        {
				(yyval.expr) = symbol_expr_alloc(&(yyloc), SYMBOL_VALUE,
						       current_scope(state), (yyvsp[0].string));
				xfree((yyvsp[0].string));
			}
#line 11417 "parser_bison.c"
    break;

  case 535: /* reject_with_expr: integer_expr  */
#line 3368 "parser_bison.y"
                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 11423 "parser_bison.c"
    break;

  case 536: /* reject_opts: %empty  */
#line 3372 "parser_bison.y"
                        {
				(yyvsp[0].stmt)->reject.type = -1;
				(yyvsp[0].stmt)->reject.icmp_code = -1;
			}
#line 11432 "parser_bison.c"
    break;

  case 537: /* reject_opts: "with" "icmp" "type" reject_with_expr close_scope_type close_scope_icmp  */
#line 3377 "parser_bison.y"
                        {
				(yyvsp[-6].stmt)->reject.family = NFPROTO_IPV4;
				(yyvsp[-6].stmt)->reject.type = NFT_REJECT_ICMP_UNREACH;
				(yyvsp[-6].stmt)->reject.expr = (yyvsp[-2].expr);
				datatype_set((yyvsp[-6].stmt)->reject.expr, &icmp_code_type);
			}
#line 11443 "parser_bison.c"
    break;

  case 538: /* reject_opts: "with" "icmp" reject_with_expr  */
#line 3384 "parser_bison.y"
                        {
				(yyvsp[-3].stmt)->reject.family = NFPROTO_IPV4;
				(yyvsp[-3].stmt)->reject.type = NFT_REJECT_ICMP_UNREACH;
				(yyvsp[-3].stmt)->reject.expr = (yyvsp[0].expr);
				datatype_set((yyvsp[-3].stmt)->reject.expr, &icmp_code_type);
			}
#line 11454 "parser_bison.c"
    break;

  case 539: /* reject_opts: "with" "icmpv6" "type" reject_with_expr close_scope_type close_scope_icmp  */
#line 3391 "parser_bison.y"
                        {
				(yyvsp[-6].stmt)->reject.family = NFPROTO_IPV6;
				(yyvsp[-6].stmt)->reject.type = NFT_REJECT_ICMP_UNREACH;
				(yyvsp[-6].stmt)->reject.expr = (yyvsp[-2].expr);
				datatype_set((yyvsp[-6].stmt)->reject.expr, &icmpv6_code_type);
			}
#line 11465 "parser_bison.c"
    break;

  case 540: /* reject_opts: "with" "icmpv6" reject_with_expr  */
#line 3398 "parser_bison.y"
                        {
				(yyvsp[-3].stmt)->reject.family = NFPROTO_IPV6;
				(yyvsp[-3].stmt)->reject.type = NFT_REJECT_ICMP_UNREACH;
				(yyvsp[-3].stmt)->reject.expr = (yyvsp[0].expr);
				datatype_set((yyvsp[-3].stmt)->reject.expr, &icmpv6_code_type);
			}
#line 11476 "parser_bison.c"
    break;

  case 541: /* reject_opts: "with" "icmpx" "type" reject_with_expr close_scope_type  */
#line 3405 "parser_bison.y"
                        {
				(yyvsp[-5].stmt)->reject.type = NFT_REJECT_ICMPX_UNREACH;
				(yyvsp[-5].stmt)->reject.expr = (yyvsp[-1].expr);
				datatype_set((yyvsp[-5].stmt)->reject.expr, &icmpx_code_type);
			}
#line 11486 "parser_bison.c"
    break;

  case 542: /* reject_opts: "with" "icmpx" reject_with_expr  */
#line 3411 "parser_bison.y"
                        {
				(yyvsp[-3].stmt)->reject.type = NFT_REJECT_ICMPX_UNREACH;
				(yyvsp[-3].stmt)->reject.expr = (yyvsp[0].expr);
				datatype_set((yyvsp[-3].stmt)->reject.expr, &icmpx_code_type);
			}
#line 11496 "parser_bison.c"
    break;

  case 543: /* reject_opts: "with" "tcp" close_scope_tcp "reset" close_scope_reset  */
#line 3417 "parser_bison.y"
                        {
				(yyvsp[-5].stmt)->reject.type = NFT_REJECT_TCP_RST;
			}
#line 11504 "parser_bison.c"
    break;

  case 545: /* nat_stmt_alloc: "snat"  */
#line 3425 "parser_bison.y"
                                        { (yyval.stmt) = nat_stmt_alloc(&(yyloc), NFT_NAT_SNAT); }
#line 11510 "parser_bison.c"
    break;

  case 546: /* nat_stmt_alloc: "dnat"  */
#line 3426 "parser_bison.y"
                                        { (yyval.stmt) = nat_stmt_alloc(&(yyloc), NFT_NAT_DNAT); }
#line 11516 "parser_bison.c"
    break;

  case 547: /* tproxy_stmt: "tproxy" "to" stmt_expr  */
#line 3430 "parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = NFPROTO_UNSPEC;
				(yyval.stmt)->tproxy.addr = (yyvsp[0].expr);
			}
#line 11526 "parser_bison.c"
    break;

  case 548: /* tproxy_stmt: "tproxy" nf_key_proto "to" stmt_expr  */
#line 3436 "parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = (yyvsp[-2].val);
				(yyval.stmt)->tproxy.addr = (yyvsp[0].expr);
			}
#line 11536 "parser_bison.c"
    break;

  case 549: /* tproxy_stmt: "tproxy" "to" "colon" stmt_expr  */
#line 3442 "parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = NFPROTO_UNSPEC;
				(yyval.stmt)->tproxy.port = (yyvsp[0].expr);
			}
#line 11546 "parser_bison.c"
    break;

  case 550: /* tproxy_stmt: "tproxy" "to" stmt_expr "colon" stmt_expr  */
#line 3448 "parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = NFPROTO_UNSPEC;
				(yyval.stmt)->tproxy.addr = (yyvsp[-2].expr);
				(yyval.stmt)->tproxy.port = (yyvsp[0].expr);
			}
#line 11557 "parser_bison.c"
    break;

  case 551: /* tproxy_stmt: "tproxy" nf_key_proto "to" stmt_expr "colon" stmt_expr  */
#line 3455 "parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = (yyvsp[-4].val);
				(yyval.stmt)->tproxy.addr = (yyvsp[-2].expr);
				(yyval.stmt)->tproxy.port = (yyvsp[0].expr);
			}
#line 11568 "parser_bison.c"
    break;

  case 552: /* tproxy_stmt: "tproxy" nf_key_proto "to" "colon" stmt_expr  */
#line 3462 "parser_bison.y"
                        {
				(yyval.stmt) = tproxy_stmt_alloc(&(yyloc));
				(yyval.stmt)->tproxy.family = (yyvsp[-3].val);
				(yyval.stmt)->tproxy.port = (yyvsp[0].expr);
			}
#line 11578 "parser_bison.c"
    break;

  case 555: /* synproxy_stmt_alloc: "synproxy"  */
#line 3474 "parser_bison.y"
                        {
				(yyval.stmt) = synproxy_stmt_alloc(&(yyloc));
			}
#line 11586 "parser_bison.c"
    break;

  case 556: /* synproxy_stmt_alloc: "synproxy" "name" stmt_expr  */
#line 3478 "parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_SYNPROXY;
				(yyval.stmt)->objref.expr = (yyvsp[0].expr);
			}
#line 11596 "parser_bison.c"
    break;

  case 557: /* synproxy_args: synproxy_arg  */
#line 3486 "parser_bison.y"
                        {
				(yyval.stmt)	= (yyvsp[-1].stmt);
			}
#line 11604 "parser_bison.c"
    break;

  case 559: /* synproxy_arg: "mss" "number"  */
#line 3493 "parser_bison.y"
                        {
				(yyvsp[-2].stmt)->synproxy.mss = (yyvsp[0].val);
				(yyvsp[-2].stmt)->synproxy.flags |= NF_SYNPROXY_OPT_MSS;
			}
#line 11613 "parser_bison.c"
    break;

  case 560: /* synproxy_arg: "wscale" "number"  */
#line 3498 "parser_bison.y"
                        {
				(yyvsp[-2].stmt)->synproxy.wscale = (yyvsp[0].val);
				(yyvsp[-2].stmt)->synproxy.flags |= NF_SYNPROXY_OPT_WSCALE;
			}
#line 11622 "parser_bison.c"
    break;

  case 561: /* synproxy_arg: "timestamp"  */
#line 3503 "parser_bison.y"
                        {
				(yyvsp[-1].stmt)->synproxy.flags |= NF_SYNPROXY_OPT_TIMESTAMP;
			}
#line 11630 "parser_bison.c"
    break;

  case 562: /* synproxy_arg: "sack-permitted"  */
#line 3507 "parser_bison.y"
                        {
				(yyvsp[-1].stmt)->synproxy.flags |= NF_SYNPROXY_OPT_SACK_PERM;
			}
#line 11638 "parser_bison.c"
    break;

  case 563: /* synproxy_config: "mss" "number" "wscale" "number" synproxy_ts synproxy_sack  */
#line 3513 "parser_bison.y"
                        {
				struct synproxy *synproxy;
				uint32_t flags = 0;

				synproxy = &(yyvsp[-6].obj)->synproxy;
				synproxy->mss = (yyvsp[-4].val);
				flags |= NF_SYNPROXY_OPT_MSS;
				synproxy->wscale = (yyvsp[-2].val);
				flags |= NF_SYNPROXY_OPT_WSCALE;
				if ((yyvsp[-1].val))
					flags |= (yyvsp[-1].val);
				if ((yyvsp[0].val))
					flags |= (yyvsp[0].val);
				synproxy->flags = flags;
			}
#line 11658 "parser_bison.c"
    break;

  case 564: /* synproxy_config: "mss" "number" stmt_separator "wscale" "number" stmt_separator synproxy_ts synproxy_sack  */
#line 3529 "parser_bison.y"
                        {
				struct synproxy *synproxy;
				uint32_t flags = 0;

				synproxy = &(yyvsp[-8].obj)->synproxy;
				synproxy->mss = (yyvsp[-6].val);
				flags |= NF_SYNPROXY_OPT_MSS;
				synproxy->wscale = (yyvsp[-3].val);
				flags |= NF_SYNPROXY_OPT_WSCALE;
				if ((yyvsp[-1].val))
					flags |= (yyvsp[-1].val);
				if ((yyvsp[0].val))
					flags |= (yyvsp[0].val);
				synproxy->flags = flags;
			}
#line 11678 "parser_bison.c"
    break;

  case 565: /* synproxy_obj: %empty  */
#line 3547 "parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
				(yyval.obj)->type = NFT_OBJECT_SYNPROXY;
			}
#line 11687 "parser_bison.c"
    break;

  case 566: /* synproxy_ts: %empty  */
#line 3553 "parser_bison.y"
                                                { (yyval.val) = 0; }
#line 11693 "parser_bison.c"
    break;

  case 567: /* synproxy_ts: "timestamp"  */
#line 3555 "parser_bison.y"
                        {
				(yyval.val) = NF_SYNPROXY_OPT_TIMESTAMP;
			}
#line 11701 "parser_bison.c"
    break;

  case 568: /* synproxy_sack: %empty  */
#line 3560 "parser_bison.y"
                                                { (yyval.val) = 0; }
#line 11707 "parser_bison.c"
    break;

  case 569: /* synproxy_sack: "sack-permitted"  */
#line 3562 "parser_bison.y"
                        {
				(yyval.val) = NF_SYNPROXY_OPT_SACK_PERM;
			}
#line 11715 "parser_bison.c"
    break;

  case 570: /* primary_stmt_expr: symbol_expr  */
#line 3567 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 11721 "parser_bison.c"
    break;

  case 571: /* primary_stmt_expr: integer_expr  */
#line 3568 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 11727 "parser_bison.c"
    break;

  case 572: /* primary_stmt_expr: boolean_expr  */
#line 3569 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 11733 "parser_bison.c"
    break;

  case 573: /* primary_stmt_expr: meta_expr  */
#line 3570 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 11739 "parser_bison.c"
    break;

  case 574: /* primary_stmt_expr: rt_expr  */
#line 3571 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 11745 "parser_bison.c"
    break;

  case 575: /* primary_stmt_expr: ct_expr  */
#line 3572 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 11751 "parser_bison.c"
    break;

  case 576: /* primary_stmt_expr: numgen_expr  */
#line 3573 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 11757 "parser_bison.c"
    break;

  case 577: /* primary_stmt_expr: hash_expr  */
#line 3574 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 11763 "parser_bison.c"
    break;

  case 578: /* primary_stmt_expr: payload_expr  */
#line 3575 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 11769 "parser_bison.c"
    break;

  case 579: /* primary_stmt_expr: keyword_expr  */
#line 3576 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 11775 "parser_bison.c"
    break;

  case 580: /* primary_stmt_expr: socket_expr  */
#line 3577 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 11781 "parser_bison.c"
    break;

  case 581: /* primary_stmt_expr: osf_expr  */
#line 3578 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 11787 "parser_bison.c"
    break;

  case 582: /* primary_stmt_expr: '(' basic_stmt_expr ')'  */
#line 3579 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[-1].expr); }
#line 11793 "parser_bison.c"
    break;

  case 584: /* shift_stmt_expr: shift_stmt_expr "<<" primary_stmt_expr  */
#line 3584 "parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_LSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 11801 "parser_bison.c"
    break;

  case 585: /* shift_stmt_expr: shift_stmt_expr ">>" primary_stmt_expr  */
#line 3588 "parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_RSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 11809 "parser_bison.c"
    break;

  case 587: /* and_stmt_expr: and_stmt_expr "&" shift_stmt_expr  */
#line 3595 "parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_AND, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 11817 "parser_bison.c"
    break;

  case 589: /* exclusive_or_stmt_expr: exclusive_or_stmt_expr "^" and_stmt_expr  */
#line 3602 "parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_XOR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 11825 "parser_bison.c"
    break;

  case 591: /* inclusive_or_stmt_expr: inclusive_or_stmt_expr '|' exclusive_or_stmt_expr  */
#line 3609 "parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_OR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 11833 "parser_bison.c"
    break;

  case 594: /* concat_stmt_expr: concat_stmt_expr "." primary_stmt_expr  */
#line 3619 "parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 11846 "parser_bison.c"
    break;

  case 597: /* map_stmt_expr: concat_stmt_expr "map" map_stmt_expr_set  */
#line 3634 "parser_bison.y"
                        {
				(yyval.expr) = map_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 11854 "parser_bison.c"
    break;

  case 598: /* map_stmt_expr: concat_stmt_expr  */
#line 3637 "parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 11860 "parser_bison.c"
    break;

  case 599: /* prefix_stmt_expr: basic_stmt_expr "/" "number"  */
#line 3641 "parser_bison.y"
                        {
				(yyval.expr) = prefix_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].val));
			}
#line 11868 "parser_bison.c"
    break;

  case 600: /* range_stmt_expr: basic_stmt_expr "-" basic_stmt_expr  */
#line 3647 "parser_bison.y"
                        {
				(yyval.expr) = range_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 11876 "parser_bison.c"
    break;

  case 606: /* nat_stmt_args: stmt_expr  */
#line 3662 "parser_bison.y"
                        {
				(yyvsp[-1].stmt)->nat.addr = (yyvsp[0].expr);
			}
#line 11884 "parser_bison.c"
    break;

  case 607: /* nat_stmt_args: "to" stmt_expr  */
#line 3666 "parser_bison.y"
                        {
				(yyvsp[-2].stmt)->nat.addr = (yyvsp[0].expr);
			}
#line 11892 "parser_bison.c"
    break;

  case 608: /* nat_stmt_args: nf_key_proto "to" stmt_expr  */
#line 3670 "parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.family = (yyvsp[-2].val);
				(yyvsp[-3].stmt)->nat.addr = (yyvsp[0].expr);
			}
#line 11901 "parser_bison.c"
    break;

  case 609: /* nat_stmt_args: stmt_expr "colon" stmt_expr  */
#line 3675 "parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.addr = (yyvsp[-2].expr);
				(yyvsp[-3].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 11910 "parser_bison.c"
    break;

  case 610: /* nat_stmt_args: "to" stmt_expr "colon" stmt_expr  */
#line 3680 "parser_bison.y"
                        {
				(yyvsp[-4].stmt)->nat.addr = (yyvsp[-2].expr);
				(yyvsp[-4].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 11919 "parser_bison.c"
    break;

  case 611: /* nat_stmt_args: nf_key_proto "to" stmt_expr "colon" stmt_expr  */
#line 3685 "parser_bison.y"
                        {
				(yyvsp[-5].stmt)->nat.family = (yyvsp[-4].val);
				(yyvsp[-5].stmt)->nat.addr = (yyvsp[-2].expr);
				(yyvsp[-5].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 11929 "parser_bison.c"
    break;

  case 612: /* nat_stmt_args: "colon" stmt_expr  */
#line 3691 "parser_bison.y"
                        {
				(yyvsp[-2].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 11937 "parser_bison.c"
    break;

  case 613: /* nat_stmt_args: "to" "colon" stmt_expr  */
#line 3695 "parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 11945 "parser_bison.c"
    break;

  case 614: /* nat_stmt_args: nat_stmt_args nf_nat_flags  */
#line 3699 "parser_bison.y"
                        {
				(yyvsp[-2].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 11953 "parser_bison.c"
    break;

  case 615: /* nat_stmt_args: nf_key_proto "addr" "." "port" "to" stmt_expr  */
#line 3703 "parser_bison.y"
                        {
				(yyvsp[-6].stmt)->nat.family = (yyvsp[-5].val);
				(yyvsp[-6].stmt)->nat.addr = (yyvsp[0].expr);
				(yyvsp[-6].stmt)->nat.type_flags = STMT_NAT_F_CONCAT;
			}
#line 11963 "parser_bison.c"
    break;

  case 616: /* nat_stmt_args: nf_key_proto "interval" "to" stmt_expr  */
#line 3709 "parser_bison.y"
                        {
				(yyvsp[-4].stmt)->nat.family = (yyvsp[-3].val);
				(yyvsp[-4].stmt)->nat.addr = (yyvsp[0].expr);
			}
#line 11972 "parser_bison.c"
    break;

  case 617: /* nat_stmt_args: "interval" "to" stmt_expr  */
#line 3714 "parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.addr = (yyvsp[0].expr);
			}
#line 11980 "parser_bison.c"
    break;

  case 618: /* nat_stmt_args: nf_key_proto "prefix" "to" stmt_expr  */
#line 3718 "parser_bison.y"
                        {
				(yyvsp[-4].stmt)->nat.family = (yyvsp[-3].val);
				(yyvsp[-4].stmt)->nat.addr = (yyvsp[0].expr);
				(yyvsp[-4].stmt)->nat.type_flags =
						STMT_NAT_F_PREFIX;
				(yyvsp[-4].stmt)->nat.flags |= NF_NAT_RANGE_NETMAP;
			}
#line 11992 "parser_bison.c"
    break;

  case 619: /* nat_stmt_args: "prefix" "to" stmt_expr  */
#line 3726 "parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.addr = (yyvsp[0].expr);
				(yyvsp[-3].stmt)->nat.type_flags =
						STMT_NAT_F_PREFIX;
				(yyvsp[-3].stmt)->nat.flags |= NF_NAT_RANGE_NETMAP;
			}
#line 12003 "parser_bison.c"
    break;

  case 622: /* masq_stmt_alloc: "masquerade"  */
#line 3738 "parser_bison.y"
                                                { (yyval.stmt) = nat_stmt_alloc(&(yyloc), NFT_NAT_MASQ); }
#line 12009 "parser_bison.c"
    break;

  case 623: /* masq_stmt_args: "to" "colon" stmt_expr  */
#line 3742 "parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12017 "parser_bison.c"
    break;

  case 624: /* masq_stmt_args: "to" "colon" stmt_expr nf_nat_flags  */
#line 3746 "parser_bison.y"
                        {
				(yyvsp[-4].stmt)->nat.proto = (yyvsp[-1].expr);
				(yyvsp[-4].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 12026 "parser_bison.c"
    break;

  case 625: /* masq_stmt_args: nf_nat_flags  */
#line 3751 "parser_bison.y"
                        {
				(yyvsp[-1].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 12034 "parser_bison.c"
    break;

  case 628: /* redir_stmt_alloc: "redirect"  */
#line 3760 "parser_bison.y"
                                                { (yyval.stmt) = nat_stmt_alloc(&(yyloc), NFT_NAT_REDIR); }
#line 12040 "parser_bison.c"
    break;

  case 629: /* redir_stmt_arg: "to" stmt_expr  */
#line 3764 "parser_bison.y"
                        {
				(yyvsp[-2].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12048 "parser_bison.c"
    break;

  case 630: /* redir_stmt_arg: "to" "colon" stmt_expr  */
#line 3768 "parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.proto = (yyvsp[0].expr);
			}
#line 12056 "parser_bison.c"
    break;

  case 631: /* redir_stmt_arg: nf_nat_flags  */
#line 3772 "parser_bison.y"
                        {
				(yyvsp[-1].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 12064 "parser_bison.c"
    break;

  case 632: /* redir_stmt_arg: "to" stmt_expr nf_nat_flags  */
#line 3776 "parser_bison.y"
                        {
				(yyvsp[-3].stmt)->nat.proto = (yyvsp[-1].expr);
				(yyvsp[-3].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 12073 "parser_bison.c"
    break;

  case 633: /* redir_stmt_arg: "to" "colon" stmt_expr nf_nat_flags  */
#line 3781 "parser_bison.y"
                        {
				(yyvsp[-4].stmt)->nat.proto = (yyvsp[-1].expr);
				(yyvsp[-4].stmt)->nat.flags = (yyvsp[0].val);
			}
#line 12082 "parser_bison.c"
    break;

  case 634: /* dup_stmt: "dup" "to" stmt_expr  */
#line 3788 "parser_bison.y"
                        {
				(yyval.stmt) = dup_stmt_alloc(&(yyloc));
				(yyval.stmt)->dup.to = (yyvsp[0].expr);
			}
#line 12091 "parser_bison.c"
    break;

  case 635: /* dup_stmt: "dup" "to" stmt_expr "device" stmt_expr  */
#line 3793 "parser_bison.y"
                        {
				(yyval.stmt) = dup_stmt_alloc(&(yyloc));
				(yyval.stmt)->dup.to = (yyvsp[-2].expr);
				(yyval.stmt)->dup.dev = (yyvsp[0].expr);
			}
#line 12101 "parser_bison.c"
    break;

  case 636: /* fwd_stmt: "fwd" "to" stmt_expr  */
#line 3801 "parser_bison.y"
                        {
				(yyval.stmt) = fwd_stmt_alloc(&(yyloc));
				(yyval.stmt)->fwd.dev = (yyvsp[0].expr);
			}
#line 12110 "parser_bison.c"
    break;

  case 637: /* fwd_stmt: "fwd" nf_key_proto "to" stmt_expr "device" stmt_expr  */
#line 3806 "parser_bison.y"
                        {
				(yyval.stmt) = fwd_stmt_alloc(&(yyloc));
				(yyval.stmt)->fwd.family = (yyvsp[-4].val);
				(yyval.stmt)->fwd.addr = (yyvsp[-2].expr);
				(yyval.stmt)->fwd.dev = (yyvsp[0].expr);
			}
#line 12121 "parser_bison.c"
    break;

  case 639: /* nf_nat_flags: nf_nat_flags "comma" nf_nat_flag  */
#line 3816 "parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-2].val) | (yyvsp[0].val);
			}
#line 12129 "parser_bison.c"
    break;

  case 640: /* nf_nat_flag: "random"  */
#line 3821 "parser_bison.y"
                                                { (yyval.val) = NF_NAT_RANGE_PROTO_RANDOM; }
#line 12135 "parser_bison.c"
    break;

  case 641: /* nf_nat_flag: "fully-random"  */
#line 3822 "parser_bison.y"
                                                { (yyval.val) = NF_NAT_RANGE_PROTO_RANDOM_FULLY; }
#line 12141 "parser_bison.c"
    break;

  case 642: /* nf_nat_flag: "persistent"  */
#line 3823 "parser_bison.y"
                                                { (yyval.val) = NF_NAT_RANGE_PERSISTENT; }
#line 12147 "parser_bison.c"
    break;

  case 644: /* queue_stmt: "queue" "to" queue_stmt_expr close_scope_queue  */
#line 3828 "parser_bison.y"
                        {
				(yyval.stmt) = queue_stmt_alloc(&(yyloc), (yyvsp[-1].expr), 0);
			}
#line 12155 "parser_bison.c"
    break;

  case 645: /* queue_stmt: "queue" "flags" queue_stmt_flags close_scope_flags "to" queue_stmt_expr close_scope_queue  */
#line 3832 "parser_bison.y"
                        {
				(yyval.stmt) = queue_stmt_alloc(&(yyloc), (yyvsp[-1].expr), (yyvsp[-4].val));
			}
#line 12163 "parser_bison.c"
    break;

  case 646: /* queue_stmt: "queue" "flags" queue_stmt_flags close_scope_flags "num" queue_stmt_expr_simple close_scope_queue  */
#line 3836 "parser_bison.y"
                        {
				(yyval.stmt) = queue_stmt_alloc(&(yyloc), (yyvsp[-1].expr), (yyvsp[-4].val));
			}
#line 12171 "parser_bison.c"
    break;

  case 649: /* queue_stmt_alloc: "queue"  */
#line 3846 "parser_bison.y"
                        {
				(yyval.stmt) = queue_stmt_alloc(&(yyloc), NULL, 0);
			}
#line 12179 "parser_bison.c"
    break;

  case 650: /* queue_stmt_args: queue_stmt_arg  */
#line 3852 "parser_bison.y"
                        {
				(yyval.stmt)	= (yyvsp[-1].stmt);
			}
#line 12187 "parser_bison.c"
    break;

  case 652: /* queue_stmt_arg: "num" queue_stmt_expr_simple  */
#line 3859 "parser_bison.y"
                        {
				(yyvsp[-2].stmt)->queue.queue = (yyvsp[0].expr);
				(yyvsp[-2].stmt)->queue.queue->location = (yyloc);
			}
#line 12196 "parser_bison.c"
    break;

  case 653: /* queue_stmt_arg: queue_stmt_flags  */
#line 3864 "parser_bison.y"
                        {
				(yyvsp[-1].stmt)->queue.flags |= (yyvsp[0].val);
			}
#line 12204 "parser_bison.c"
    break;

  case 658: /* queue_stmt_expr_simple: queue_expr "-" queue_expr  */
#line 3876 "parser_bison.y"
                        {
				(yyval.expr) = range_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12212 "parser_bison.c"
    break;

  case 664: /* queue_stmt_flags: queue_stmt_flags "comma" queue_stmt_flag  */
#line 3889 "parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-2].val) | (yyvsp[0].val);
			}
#line 12220 "parser_bison.c"
    break;

  case 665: /* queue_stmt_flag: "bypass"  */
#line 3894 "parser_bison.y"
                                        { (yyval.val) = NFT_QUEUE_FLAG_BYPASS; }
#line 12226 "parser_bison.c"
    break;

  case 666: /* queue_stmt_flag: "fanout"  */
#line 3895 "parser_bison.y"
                                        { (yyval.val) = NFT_QUEUE_FLAG_CPU_FANOUT; }
#line 12232 "parser_bison.c"
    break;

  case 669: /* set_elem_expr_stmt_alloc: concat_expr  */
#line 3903 "parser_bison.y"
                        {
				(yyval.expr) = set_elem_expr_alloc(&(yylsp[0]), (yyvsp[0].expr));
			}
#line 12240 "parser_bison.c"
    break;

  case 670: /* set_stmt: "set" set_stmt_op set_elem_expr_stmt set_ref_expr  */
#line 3909 "parser_bison.y"
                        {
				(yyval.stmt) = set_stmt_alloc(&(yyloc));
				(yyval.stmt)->set.op  = (yyvsp[-2].val);
				(yyval.stmt)->set.key = (yyvsp[-1].expr);
				(yyval.stmt)->set.set = (yyvsp[0].expr);
			}
#line 12251 "parser_bison.c"
    break;

  case 671: /* set_stmt: set_stmt_op set_ref_expr '{' set_elem_expr_stmt '}'  */
#line 3916 "parser_bison.y"
                        {
				(yyval.stmt) = set_stmt_alloc(&(yyloc));
				(yyval.stmt)->set.op  = (yyvsp[-4].val);
				(yyval.stmt)->set.key = (yyvsp[-1].expr);
				(yyval.stmt)->set.set = (yyvsp[-3].expr);
			}
#line 12262 "parser_bison.c"
    break;

  case 672: /* set_stmt: set_stmt_op set_ref_expr '{' set_elem_expr_stmt stateful_stmt_list '}'  */
#line 3923 "parser_bison.y"
                        {
				(yyval.stmt) = set_stmt_alloc(&(yyloc));
				(yyval.stmt)->set.op  = (yyvsp[-5].val);
				(yyval.stmt)->set.key = (yyvsp[-2].expr);
				(yyval.stmt)->set.set = (yyvsp[-4].expr);
				list_splice_tail((yyvsp[-1].list), &(yyval.stmt)->set.stmt_list);
				free((yyvsp[-1].list));
			}
#line 12275 "parser_bison.c"
    break;

  case 673: /* set_stmt_op: "add"  */
#line 3933 "parser_bison.y"
                                        { (yyval.val) = NFT_DYNSET_OP_ADD; }
#line 12281 "parser_bison.c"
    break;

  case 674: /* set_stmt_op: "update"  */
#line 3934 "parser_bison.y"
                                        { (yyval.val) = NFT_DYNSET_OP_UPDATE; }
#line 12287 "parser_bison.c"
    break;

  case 675: /* set_stmt_op: "delete"  */
#line 3935 "parser_bison.y"
                                        { (yyval.val) = NFT_DYNSET_OP_DELETE; }
#line 12293 "parser_bison.c"
    break;

  case 676: /* map_stmt: set_stmt_op set_ref_expr '{' set_elem_expr_stmt "colon" set_elem_expr_stmt '}'  */
#line 3939 "parser_bison.y"
                        {
				(yyval.stmt) = map_stmt_alloc(&(yyloc));
				(yyval.stmt)->map.op  = (yyvsp[-6].val);
				(yyval.stmt)->map.key = (yyvsp[-3].expr);
				(yyval.stmt)->map.data = (yyvsp[-1].expr);
				(yyval.stmt)->map.set = (yyvsp[-5].expr);
			}
#line 12305 "parser_bison.c"
    break;

  case 677: /* map_stmt: set_stmt_op set_ref_expr '{' set_elem_expr_stmt stateful_stmt_list "colon" set_elem_expr_stmt '}'  */
#line 3947 "parser_bison.y"
                        {
				(yyval.stmt) = map_stmt_alloc(&(yyloc));
				(yyval.stmt)->map.op  = (yyvsp[-7].val);
				(yyval.stmt)->map.key = (yyvsp[-4].expr);
				(yyval.stmt)->map.data = (yyvsp[-1].expr);
				(yyval.stmt)->map.set = (yyvsp[-6].expr);
				list_splice_tail((yyvsp[-3].list), &(yyval.stmt)->map.stmt_list);
				free((yyvsp[-3].list));
			}
#line 12319 "parser_bison.c"
    break;

  case 678: /* meter_stmt: flow_stmt_legacy_alloc flow_stmt_opts '{' meter_key_expr stmt '}'  */
#line 3959 "parser_bison.y"
                        {
				(yyvsp[-5].stmt)->meter.key  = (yyvsp[-2].expr);
				(yyvsp[-5].stmt)->meter.stmt = (yyvsp[-1].stmt);
				(yyval.stmt)->location  = (yyloc);
				(yyval.stmt) = (yyvsp[-5].stmt);
			}
#line 12330 "parser_bison.c"
    break;

  case 679: /* meter_stmt: meter_stmt_alloc  */
#line 3965 "parser_bison.y"
                                                                { (yyval.stmt) = (yyvsp[0].stmt); }
#line 12336 "parser_bison.c"
    break;

  case 680: /* flow_stmt_legacy_alloc: "flow"  */
#line 3969 "parser_bison.y"
                        {
				(yyval.stmt) = meter_stmt_alloc(&(yyloc));
			}
#line 12344 "parser_bison.c"
    break;

  case 681: /* flow_stmt_opts: flow_stmt_opt  */
#line 3975 "parser_bison.y"
                        {
				(yyval.stmt)	= (yyvsp[-1].stmt);
			}
#line 12352 "parser_bison.c"
    break;

  case 683: /* flow_stmt_opt: "table" identifier  */
#line 3982 "parser_bison.y"
                        {
				(yyvsp[-2].stmt)->meter.name = (yyvsp[0].string);
			}
#line 12360 "parser_bison.c"
    break;

  case 684: /* meter_stmt_alloc: "meter" identifier '{' meter_key_expr stmt '}'  */
#line 3988 "parser_bison.y"
                        {
				(yyval.stmt) = meter_stmt_alloc(&(yyloc));
				(yyval.stmt)->meter.name = (yyvsp[-4].string);
				(yyval.stmt)->meter.size = 0;
				(yyval.stmt)->meter.key  = (yyvsp[-2].expr);
				(yyval.stmt)->meter.stmt = (yyvsp[-1].stmt);
				(yyval.stmt)->location  = (yyloc);
			}
#line 12373 "parser_bison.c"
    break;

  case 685: /* meter_stmt_alloc: "meter" identifier "size" "number" '{' meter_key_expr stmt '}'  */
#line 3997 "parser_bison.y"
                        {
				(yyval.stmt) = meter_stmt_alloc(&(yyloc));
				(yyval.stmt)->meter.name = (yyvsp[-6].string);
				(yyval.stmt)->meter.size = (yyvsp[-4].val);
				(yyval.stmt)->meter.key  = (yyvsp[-2].expr);
				(yyval.stmt)->meter.stmt = (yyvsp[-1].stmt);
				(yyval.stmt)->location  = (yyloc);
			}
#line 12386 "parser_bison.c"
    break;

  case 686: /* match_stmt: relational_expr  */
#line 4008 "parser_bison.y"
                        {
				(yyval.stmt) = expr_stmt_alloc(&(yyloc), (yyvsp[0].expr));
			}
#line 12394 "parser_bison.c"
    break;

  case 687: /* variable_expr: '$' identifier  */
#line 4014 "parser_bison.y"
                        {
				struct scope *scope = current_scope(state);
				struct symbol *sym;

				sym = symbol_get(scope, (yyvsp[0].string));
				if (!sym) {
					sym = symbol_lookup_fuzzy(scope, (yyvsp[0].string));
					if (sym) {
						erec_queue(error(&(yylsp[0]), "unknown identifier '%s'; "
								      "did you mean identifier ‘%s’?",
								      (yyvsp[0].string), sym->identifier),
							   state->msgs);
					} else {
						erec_queue(error(&(yylsp[0]), "unknown identifier '%s'", (yyvsp[0].string)),
							   state->msgs);
					}
					xfree((yyvsp[0].string));
					YYERROR;
				}

				(yyval.expr) = variable_expr_alloc(&(yyloc), scope, sym);
				xfree((yyvsp[0].string));
			}
#line 12422 "parser_bison.c"
    break;

  case 689: /* symbol_expr: string  */
#line 4041 "parser_bison.y"
                        {
				(yyval.expr) = symbol_expr_alloc(&(yyloc), SYMBOL_VALUE,
						       current_scope(state),
						       (yyvsp[0].string));
				xfree((yyvsp[0].string));
			}
#line 12433 "parser_bison.c"
    break;

  case 692: /* set_ref_symbol_expr: "@" identifier close_scope_at  */
#line 4054 "parser_bison.y"
                        {
				(yyval.expr) = symbol_expr_alloc(&(yyloc), SYMBOL_SET,
						       current_scope(state),
						       (yyvsp[-1].string));
				xfree((yyvsp[-1].string));
			}
#line 12444 "parser_bison.c"
    break;

  case 693: /* integer_expr: "number"  */
#line 4063 "parser_bison.y"
                        {
				char str[64];

				snprintf(str, sizeof(str), "%" PRIu64, (yyvsp[0].val));
				(yyval.expr) = symbol_expr_alloc(&(yyloc), SYMBOL_VALUE,
						       current_scope(state),
						       str);
			}
#line 12457 "parser_bison.c"
    break;

  case 694: /* primary_expr: symbol_expr  */
#line 4073 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12463 "parser_bison.c"
    break;

  case 695: /* primary_expr: integer_expr  */
#line 4074 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12469 "parser_bison.c"
    break;

  case 696: /* primary_expr: payload_expr  */
#line 4075 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12475 "parser_bison.c"
    break;

  case 697: /* primary_expr: exthdr_expr  */
#line 4076 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12481 "parser_bison.c"
    break;

  case 698: /* primary_expr: exthdr_exists_expr  */
#line 4077 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12487 "parser_bison.c"
    break;

  case 699: /* primary_expr: meta_expr  */
#line 4078 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12493 "parser_bison.c"
    break;

  case 700: /* primary_expr: socket_expr  */
#line 4079 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12499 "parser_bison.c"
    break;

  case 701: /* primary_expr: rt_expr  */
#line 4080 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12505 "parser_bison.c"
    break;

  case 702: /* primary_expr: ct_expr  */
#line 4081 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12511 "parser_bison.c"
    break;

  case 703: /* primary_expr: numgen_expr  */
#line 4082 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12517 "parser_bison.c"
    break;

  case 704: /* primary_expr: hash_expr  */
#line 4083 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12523 "parser_bison.c"
    break;

  case 705: /* primary_expr: fib_expr  */
#line 4084 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12529 "parser_bison.c"
    break;

  case 706: /* primary_expr: osf_expr  */
#line 4085 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12535 "parser_bison.c"
    break;

  case 707: /* primary_expr: xfrm_expr  */
#line 4086 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[0].expr); }
#line 12541 "parser_bison.c"
    break;

  case 708: /* primary_expr: '(' basic_expr ')'  */
#line 4087 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[-1].expr); }
#line 12547 "parser_bison.c"
    break;

  case 709: /* fib_expr: "fib" fib_tuple fib_result close_scope_fib  */
#line 4091 "parser_bison.y"
                        {
				if (((yyvsp[-2].val) & (NFTA_FIB_F_SADDR|NFTA_FIB_F_DADDR)) == 0) {
					erec_queue(error(&(yylsp[-2]), "fib: need either saddr or daddr"), state->msgs);
					YYERROR;
				}

				if (((yyvsp[-2].val) & (NFTA_FIB_F_SADDR|NFTA_FIB_F_DADDR)) ==
					  (NFTA_FIB_F_SADDR|NFTA_FIB_F_DADDR)) {
					erec_queue(error(&(yylsp[-2]), "fib: saddr and daddr are mutually exclusive"), state->msgs);
					YYERROR;
				}

				if (((yyvsp[-2].val) & (NFTA_FIB_F_IIF|NFTA_FIB_F_OIF)) ==
					  (NFTA_FIB_F_IIF|NFTA_FIB_F_OIF)) {
					erec_queue(error(&(yylsp[-2]), "fib: iif and oif are mutually exclusive"), state->msgs);
					YYERROR;
				}

				(yyval.expr) = fib_expr_alloc(&(yyloc), (yyvsp[-2].val), (yyvsp[-1].val));
			}
#line 12572 "parser_bison.c"
    break;

  case 710: /* fib_result: "oif"  */
#line 4113 "parser_bison.y"
                                        { (yyval.val) =NFT_FIB_RESULT_OIF; }
#line 12578 "parser_bison.c"
    break;

  case 711: /* fib_result: "oifname"  */
#line 4114 "parser_bison.y"
                                        { (yyval.val) =NFT_FIB_RESULT_OIFNAME; }
#line 12584 "parser_bison.c"
    break;

  case 712: /* fib_result: "type" close_scope_type  */
#line 4115 "parser_bison.y"
                                                                { (yyval.val) =NFT_FIB_RESULT_ADDRTYPE; }
#line 12590 "parser_bison.c"
    break;

  case 713: /* fib_flag: "saddr"  */
#line 4118 "parser_bison.y"
                                        { (yyval.val) = NFTA_FIB_F_SADDR; }
#line 12596 "parser_bison.c"
    break;

  case 714: /* fib_flag: "daddr"  */
#line 4119 "parser_bison.y"
                                        { (yyval.val) = NFTA_FIB_F_DADDR; }
#line 12602 "parser_bison.c"
    break;

  case 715: /* fib_flag: "mark"  */
#line 4120 "parser_bison.y"
                                        { (yyval.val) = NFTA_FIB_F_MARK; }
#line 12608 "parser_bison.c"
    break;

  case 716: /* fib_flag: "iif"  */
#line 4121 "parser_bison.y"
                                        { (yyval.val) = NFTA_FIB_F_IIF; }
#line 12614 "parser_bison.c"
    break;

  case 717: /* fib_flag: "oif"  */
#line 4122 "parser_bison.y"
                                        { (yyval.val) = NFTA_FIB_F_OIF; }
#line 12620 "parser_bison.c"
    break;

  case 718: /* fib_tuple: fib_flag "." fib_tuple  */
#line 4126 "parser_bison.y"
                        {
				(yyval.val) = (yyvsp[-2].val) | (yyvsp[0].val);
			}
#line 12628 "parser_bison.c"
    break;

  case 720: /* osf_expr: "osf" osf_ttl "version" close_scope_osf  */
#line 4133 "parser_bison.y"
                        {
				(yyval.expr) = osf_expr_alloc(&(yyloc), (yyvsp[-2].val), NFT_OSF_F_VERSION);
			}
#line 12636 "parser_bison.c"
    break;

  case 721: /* osf_expr: "osf" osf_ttl "name" close_scope_osf  */
#line 4137 "parser_bison.y"
                        {
				(yyval.expr) = osf_expr_alloc(&(yyloc), (yyvsp[-2].val), 0);
			}
#line 12644 "parser_bison.c"
    break;

  case 722: /* osf_ttl: %empty  */
#line 4143 "parser_bison.y"
                        {
				(yyval.val) = NF_OSF_TTL_TRUE;
			}
#line 12652 "parser_bison.c"
    break;

  case 723: /* osf_ttl: "ttl" "string"  */
#line 4147 "parser_bison.y"
                        {
				if (!strcmp((yyvsp[0].string), "loose"))
					(yyval.val) = NF_OSF_TTL_LESS;
				else if (!strcmp((yyvsp[0].string), "skip"))
					(yyval.val) = NF_OSF_TTL_NOCHECK;
				else {
					erec_queue(error(&(yylsp[0]), "invalid ttl option"),
						   state->msgs);
					xfree((yyvsp[0].string));
					YYERROR;
				}
				xfree((yyvsp[0].string));
			}
#line 12670 "parser_bison.c"
    break;

  case 725: /* shift_expr: shift_expr "<<" primary_rhs_expr  */
#line 4164 "parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_LSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12678 "parser_bison.c"
    break;

  case 726: /* shift_expr: shift_expr ">>" primary_rhs_expr  */
#line 4168 "parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_RSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12686 "parser_bison.c"
    break;

  case 728: /* and_expr: and_expr "&" shift_rhs_expr  */
#line 4175 "parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_AND, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12694 "parser_bison.c"
    break;

  case 730: /* exclusive_or_expr: exclusive_or_expr "^" and_rhs_expr  */
#line 4182 "parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_XOR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12702 "parser_bison.c"
    break;

  case 732: /* inclusive_or_expr: inclusive_or_expr '|' exclusive_or_rhs_expr  */
#line 4189 "parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_OR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12710 "parser_bison.c"
    break;

  case 735: /* concat_expr: concat_expr "." basic_expr  */
#line 4199 "parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 12723 "parser_bison.c"
    break;

  case 736: /* prefix_rhs_expr: basic_rhs_expr "/" "number"  */
#line 4210 "parser_bison.y"
                        {
				(yyval.expr) = prefix_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].val));
			}
#line 12731 "parser_bison.c"
    break;

  case 737: /* range_rhs_expr: basic_rhs_expr "-" basic_rhs_expr  */
#line 4216 "parser_bison.y"
                        {
				(yyval.expr) = range_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12739 "parser_bison.c"
    break;

  case 740: /* map_expr: concat_expr "map" rhs_expr  */
#line 4226 "parser_bison.y"
                        {
				(yyval.expr) = map_expr_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 12747 "parser_bison.c"
    break;

  case 744: /* set_expr: '{' set_list_expr '}'  */
#line 4237 "parser_bison.y"
                        {
				(yyvsp[-1].expr)->location = (yyloc);
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 12756 "parser_bison.c"
    break;

  case 745: /* set_list_expr: set_list_member_expr  */
#line 4244 "parser_bison.y"
                        {
				(yyval.expr) = set_expr_alloc(&(yyloc), NULL);
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 12765 "parser_bison.c"
    break;

  case 746: /* set_list_expr: set_list_expr "comma" set_list_member_expr  */
#line 4249 "parser_bison.y"
                        {
				compound_expr_add((yyvsp[-2].expr), (yyvsp[0].expr));
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 12774 "parser_bison.c"
    break;

  case 748: /* set_list_member_expr: opt_newline set_expr opt_newline  */
#line 4257 "parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 12782 "parser_bison.c"
    break;

  case 749: /* set_list_member_expr: opt_newline set_elem_expr opt_newline  */
#line 4261 "parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 12790 "parser_bison.c"
    break;

  case 750: /* set_list_member_expr: opt_newline set_elem_expr "colon" set_rhs_expr opt_newline  */
#line 4265 "parser_bison.y"
                        {
				(yyval.expr) = mapping_expr_alloc(&(yyloc), (yyvsp[-3].expr), (yyvsp[-1].expr));
			}
#line 12798 "parser_bison.c"
    break;

  case 752: /* meter_key_expr: meter_key_expr_alloc set_elem_options  */
#line 4272 "parser_bison.y"
                        {
				(yyval.expr)->location = (yyloc);
				(yyval.expr) = (yyvsp[-1].expr);
			}
#line 12807 "parser_bison.c"
    break;

  case 753: /* meter_key_expr_alloc: concat_expr  */
#line 4279 "parser_bison.y"
                        {
				(yyval.expr) = set_elem_expr_alloc(&(yylsp[0]), (yyvsp[0].expr));
			}
#line 12815 "parser_bison.c"
    break;

  case 756: /* set_elem_key_expr: set_lhs_expr  */
#line 4288 "parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 12821 "parser_bison.c"
    break;

  case 757: /* set_elem_key_expr: "*"  */
#line 4289 "parser_bison.y"
                                                        { (yyval.expr) = set_elem_catchall_expr_alloc(&(yylsp[0])); }
#line 12827 "parser_bison.c"
    break;

  case 758: /* set_elem_expr_alloc: set_elem_key_expr set_elem_stmt_list  */
#line 4293 "parser_bison.y"
                        {
				(yyval.expr) = set_elem_expr_alloc(&(yylsp[-1]), (yyvsp[-1].expr));
				list_splice_tail((yyvsp[0].list), &(yyval.expr)->stmt_list);
				xfree((yyvsp[0].list));
			}
#line 12837 "parser_bison.c"
    break;

  case 759: /* set_elem_expr_alloc: set_elem_key_expr  */
#line 4299 "parser_bison.y"
                        {
				(yyval.expr) = set_elem_expr_alloc(&(yylsp[0]), (yyvsp[0].expr));
			}
#line 12845 "parser_bison.c"
    break;

  case 760: /* set_elem_options: set_elem_option  */
#line 4305 "parser_bison.y"
                        {
				(yyval.expr)	= (yyvsp[-1].expr);
			}
#line 12853 "parser_bison.c"
    break;

  case 762: /* set_elem_option: "timeout" time_spec  */
#line 4312 "parser_bison.y"
                        {
				(yyvsp[-2].expr)->timeout = (yyvsp[0].val);
			}
#line 12861 "parser_bison.c"
    break;

  case 763: /* set_elem_option: "expires" time_spec  */
#line 4316 "parser_bison.y"
                        {
				(yyvsp[-2].expr)->expiration = (yyvsp[0].val);
			}
#line 12869 "parser_bison.c"
    break;

  case 764: /* set_elem_option: comment_spec  */
#line 4320 "parser_bison.y"
                        {
				if (already_set((yyvsp[-1].expr)->comment, &(yylsp[0]), state)) {
					xfree((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].expr)->comment = (yyvsp[0].string);
			}
#line 12881 "parser_bison.c"
    break;

  case 765: /* set_elem_expr_options: set_elem_expr_option  */
#line 4330 "parser_bison.y"
                        {
				(yyval.expr)	= (yyvsp[-1].expr);
			}
#line 12889 "parser_bison.c"
    break;

  case 767: /* set_elem_stmt_list: set_elem_stmt  */
#line 4337 "parser_bison.y"
                        {
				(yyval.list) = xmalloc(sizeof(*(yyval.list)));
				init_list_head((yyval.list));
				list_add_tail(&(yyvsp[0].stmt)->list, (yyval.list));
			}
#line 12899 "parser_bison.c"
    break;

  case 768: /* set_elem_stmt_list: set_elem_stmt_list set_elem_stmt  */
#line 4343 "parser_bison.y"
                        {
				(yyval.list) = (yyvsp[-1].list);
				list_add_tail(&(yyvsp[0].stmt)->list, (yyvsp[-1].list));
			}
#line 12908 "parser_bison.c"
    break;

  case 769: /* set_elem_stmt: "counter" close_scope_counter  */
#line 4350 "parser_bison.y"
                        {
				(yyval.stmt) = counter_stmt_alloc(&(yyloc));
			}
#line 12916 "parser_bison.c"
    break;

  case 770: /* set_elem_stmt: "counter" "packets" "number" "bytes" "number" close_scope_counter  */
#line 4354 "parser_bison.y"
                        {
				(yyval.stmt) = counter_stmt_alloc(&(yyloc));
				(yyval.stmt)->counter.packets = (yyvsp[-3].val);
				(yyval.stmt)->counter.bytes = (yyvsp[-1].val);
			}
#line 12926 "parser_bison.c"
    break;

  case 771: /* set_elem_stmt: "limit" "rate" limit_mode limit_rate_pkts limit_burst_pkts close_scope_limit  */
#line 4360 "parser_bison.y"
                        {
				if ((yyvsp[-1].val) == 0) {
					erec_queue(error(&(yylsp[-1]), "limit burst must be > 0"),
						   state->msgs);
					YYERROR;
				}
				(yyval.stmt) = limit_stmt_alloc(&(yyloc));
				(yyval.stmt)->limit.rate  = (yyvsp[-2].limit_rate).rate;
				(yyval.stmt)->limit.unit  = (yyvsp[-2].limit_rate).unit;
				(yyval.stmt)->limit.burst = (yyvsp[-1].val);
				(yyval.stmt)->limit.type  = NFT_LIMIT_PKTS;
				(yyval.stmt)->limit.flags = (yyvsp[-3].val);
			}
#line 12944 "parser_bison.c"
    break;

  case 772: /* set_elem_stmt: "limit" "rate" limit_mode limit_rate_bytes limit_burst_bytes close_scope_limit  */
#line 4374 "parser_bison.y"
                        {
				if ((yyvsp[-1].val) == 0) {
					erec_queue(error(&(yylsp[0]), "limit burst must be > 0"),
						   state->msgs);
					YYERROR;
				}
				(yyval.stmt) = limit_stmt_alloc(&(yyloc));
				(yyval.stmt)->limit.rate  = (yyvsp[-2].limit_rate).rate;
				(yyval.stmt)->limit.unit  = (yyvsp[-2].limit_rate).unit;
				(yyval.stmt)->limit.burst = (yyvsp[-1].val);
				(yyval.stmt)->limit.type  = NFT_LIMIT_PKT_BYTES;
				(yyval.stmt)->limit.flags = (yyvsp[-3].val);
			}
#line 12962 "parser_bison.c"
    break;

  case 773: /* set_elem_stmt: "ct" "count" "number" close_scope_ct  */
#line 4388 "parser_bison.y"
                        {
				(yyval.stmt) = connlimit_stmt_alloc(&(yyloc));
				(yyval.stmt)->connlimit.count	= (yyvsp[-1].val);
			}
#line 12971 "parser_bison.c"
    break;

  case 774: /* set_elem_stmt: "ct" "count" "over" "number" close_scope_ct  */
#line 4393 "parser_bison.y"
                        {
				(yyval.stmt) = connlimit_stmt_alloc(&(yyloc));
				(yyval.stmt)->connlimit.count = (yyvsp[-1].val);
				(yyval.stmt)->connlimit.flags = NFT_CONNLIMIT_F_INV;
			}
#line 12981 "parser_bison.c"
    break;

  case 775: /* set_elem_expr_option: "timeout" time_spec  */
#line 4401 "parser_bison.y"
                        {
				(yyvsp[-2].expr)->timeout = (yyvsp[0].val);
			}
#line 12989 "parser_bison.c"
    break;

  case 776: /* set_elem_expr_option: "expires" time_spec  */
#line 4405 "parser_bison.y"
                        {
				(yyvsp[-2].expr)->expiration = (yyvsp[0].val);
			}
#line 12997 "parser_bison.c"
    break;

  case 777: /* set_elem_expr_option: comment_spec  */
#line 4409 "parser_bison.y"
                        {
				if (already_set((yyvsp[-1].expr)->comment, &(yylsp[0]), state)) {
					xfree((yyvsp[0].string));
					YYERROR;
				}
				(yyvsp[-1].expr)->comment = (yyvsp[0].string);
			}
#line 13009 "parser_bison.c"
    break;

  case 783: /* initializer_expr: '{' '}'  */
#line 4427 "parser_bison.y"
                                                { (yyval.expr) = compound_expr_alloc(&(yyloc), EXPR_SET); }
#line 13015 "parser_bison.c"
    break;

  case 784: /* initializer_expr: "-" "number"  */
#line 4429 "parser_bison.y"
                        {
				int32_t num = -(yyvsp[0].val);

				(yyval.expr) = constant_expr_alloc(&(yyloc), &integer_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(num) * BITS_PER_BYTE,
							 &num);
			}
#line 13028 "parser_bison.c"
    break;

  case 785: /* counter_config: "packets" "number" "bytes" "number"  */
#line 4440 "parser_bison.y"
                        {
				struct counter *counter;

				counter = &(yyvsp[-4].obj)->counter;
				counter->packets = (yyvsp[-2].val);
				counter->bytes = (yyvsp[0].val);
			}
#line 13040 "parser_bison.c"
    break;

  case 786: /* counter_obj: %empty  */
#line 4450 "parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
				(yyval.obj)->type = NFT_OBJECT_COUNTER;
			}
#line 13049 "parser_bison.c"
    break;

  case 787: /* quota_config: quota_mode "number" quota_unit quota_used  */
#line 4457 "parser_bison.y"
                        {
				struct error_record *erec;
				struct quota *quota;
				uint64_t rate;

				erec = data_unit_parse(&(yyloc), (yyvsp[-1].string), &rate);
				xfree((yyvsp[-1].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}

				quota = &(yyvsp[-4].obj)->quota;
				quota->bytes	= (yyvsp[-2].val) * rate;
				quota->used	= (yyvsp[0].val);
				quota->flags	= (yyvsp[-3].val);
			}
#line 13071 "parser_bison.c"
    break;

  case 788: /* quota_obj: %empty  */
#line 4477 "parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
				(yyval.obj)->type = NFT_OBJECT_QUOTA;
			}
#line 13080 "parser_bison.c"
    break;

  case 789: /* secmark_config: string  */
#line 4484 "parser_bison.y"
                        {
				int ret;
				struct secmark *secmark;

				secmark = &(yyvsp[-1].obj)->secmark;
				ret = snprintf(secmark->ctx, sizeof(secmark->ctx), "%s", (yyvsp[0].string));
				if (ret <= 0 || ret >= (int)sizeof(secmark->ctx)) {
					erec_queue(error(&(yylsp[0]), "invalid context '%s', max length is %u\n", (yyvsp[0].string), (int)sizeof(secmark->ctx)), state->msgs);
					xfree((yyvsp[0].string));
					YYERROR;
				}
				xfree((yyvsp[0].string));
			}
#line 13098 "parser_bison.c"
    break;

  case 790: /* secmark_obj: %empty  */
#line 4500 "parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
				(yyval.obj)->type = NFT_OBJECT_SECMARK;
			}
#line 13107 "parser_bison.c"
    break;

  case 791: /* ct_obj_type: "helper"  */
#line 4506 "parser_bison.y"
                                                { (yyval.val) = NFT_OBJECT_CT_HELPER; }
#line 13113 "parser_bison.c"
    break;

  case 792: /* ct_obj_type: "timeout"  */
#line 4507 "parser_bison.y"
                                                { (yyval.val) = NFT_OBJECT_CT_TIMEOUT; }
#line 13119 "parser_bison.c"
    break;

  case 793: /* ct_obj_type: "expectation"  */
#line 4508 "parser_bison.y"
                                                { (yyval.val) = NFT_OBJECT_CT_EXPECT; }
#line 13125 "parser_bison.c"
    break;

  case 794: /* ct_cmd_type: "helpers"  */
#line 4511 "parser_bison.y"
                                                { (yyval.val) = CMD_OBJ_CT_HELPERS; }
#line 13131 "parser_bison.c"
    break;

  case 795: /* ct_cmd_type: "timeout"  */
#line 4512 "parser_bison.y"
                                                { (yyval.val) = CMD_OBJ_CT_TIMEOUT; }
#line 13137 "parser_bison.c"
    break;

  case 796: /* ct_cmd_type: "expectation"  */
#line 4513 "parser_bison.y"
                                                { (yyval.val) = CMD_OBJ_CT_EXPECT; }
#line 13143 "parser_bison.c"
    break;

  case 797: /* ct_l4protoname: "tcp" close_scope_tcp  */
#line 4516 "parser_bison.y"
                                                        { (yyval.val) = IPPROTO_TCP; }
#line 13149 "parser_bison.c"
    break;

  case 798: /* ct_l4protoname: "udp" close_scope_udp  */
#line 4517 "parser_bison.y"
                                                        { (yyval.val) = IPPROTO_UDP; }
#line 13155 "parser_bison.c"
    break;

  case 799: /* ct_helper_config: "type" "quoted string" "protocol" ct_l4protoname stmt_separator close_scope_type  */
#line 4521 "parser_bison.y"
                        {
				struct ct_helper *ct;
				int ret;

				ct = &(yyvsp[-6].obj)->ct_helper;

				ret = snprintf(ct->name, sizeof(ct->name), "%s", (yyvsp[-4].string));
				if (ret <= 0 || ret >= (int)sizeof(ct->name)) {
					erec_queue(error(&(yylsp[-4]), "invalid name '%s', max length is %u\n", (yyvsp[-4].string), (int)sizeof(ct->name)), state->msgs);
					YYERROR;
				}
				xfree((yyvsp[-4].string));

				ct->l4proto = (yyvsp[-2].val);
			}
#line 13175 "parser_bison.c"
    break;

  case 800: /* ct_helper_config: "l3proto" family_spec_explicit stmt_separator  */
#line 4537 "parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_helper.l3proto = (yyvsp[-1].val);
			}
#line 13183 "parser_bison.c"
    break;

  case 801: /* timeout_states: timeout_state  */
#line 4543 "parser_bison.y"
                        {
				(yyval.list) = xmalloc(sizeof(*(yyval.list)));
				init_list_head((yyval.list));
				list_add_tail((yyvsp[0].list), (yyval.list));
			}
#line 13193 "parser_bison.c"
    break;

  case 802: /* timeout_states: timeout_states "comma" timeout_state  */
#line 4549 "parser_bison.y"
                        {
				list_add_tail((yyvsp[0].list), (yyvsp[-2].list));
				(yyval.list) = (yyvsp[-2].list);
			}
#line 13202 "parser_bison.c"
    break;

  case 803: /* timeout_state: "string" "colon" "number"  */
#line 4557 "parser_bison.y"
                        {
				struct timeout_state *ts;

				ts = xzalloc(sizeof(*ts));
				ts->timeout_str = (yyvsp[-2].string);
				ts->timeout_value = (yyvsp[0].val);
				ts->location = (yylsp[-2]);
				init_list_head(&ts->head);
				(yyval.list) = &ts->head;
			}
#line 13217 "parser_bison.c"
    break;

  case 804: /* ct_timeout_config: "protocol" ct_l4protoname stmt_separator  */
#line 4570 "parser_bison.y"
                        {
				struct ct_timeout *ct;
				int l4proto = (yyvsp[-1].val);

				ct = &(yyvsp[-3].obj)->ct_timeout;
				ct->l4proto = l4proto;
			}
#line 13229 "parser_bison.c"
    break;

  case 805: /* ct_timeout_config: "policy" '=' '{' timeout_states '}' stmt_separator close_scope_policy  */
#line 4578 "parser_bison.y"
                        {
				struct ct_timeout *ct;

				ct = &(yyvsp[-7].obj)->ct_timeout;
				list_splice_tail((yyvsp[-3].list), &ct->timeout_list);
				xfree((yyvsp[-3].list));
			}
#line 13241 "parser_bison.c"
    break;

  case 806: /* ct_timeout_config: "l3proto" family_spec_explicit stmt_separator  */
#line 4586 "parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_timeout.l3proto = (yyvsp[-1].val);
			}
#line 13249 "parser_bison.c"
    break;

  case 807: /* ct_expect_config: "protocol" ct_l4protoname stmt_separator  */
#line 4592 "parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_expect.l4proto = (yyvsp[-1].val);
			}
#line 13257 "parser_bison.c"
    break;

  case 808: /* ct_expect_config: "dport" "number" stmt_separator  */
#line 4596 "parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_expect.dport = (yyvsp[-1].val);
			}
#line 13265 "parser_bison.c"
    break;

  case 809: /* ct_expect_config: "timeout" time_spec stmt_separator  */
#line 4600 "parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_expect.timeout = (yyvsp[-1].val);
			}
#line 13273 "parser_bison.c"
    break;

  case 810: /* ct_expect_config: "size" "number" stmt_separator  */
#line 4604 "parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_expect.size = (yyvsp[-1].val);
			}
#line 13281 "parser_bison.c"
    break;

  case 811: /* ct_expect_config: "l3proto" family_spec_explicit stmt_separator  */
#line 4608 "parser_bison.y"
                        {
				(yyvsp[-3].obj)->ct_expect.l3proto = (yyvsp[-1].val);
			}
#line 13289 "parser_bison.c"
    break;

  case 812: /* ct_obj_alloc: %empty  */
#line 4614 "parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
			}
#line 13297 "parser_bison.c"
    break;

  case 813: /* limit_config: "rate" limit_mode limit_rate_pkts limit_burst_pkts  */
#line 4620 "parser_bison.y"
                        {
				struct limit *limit;

				limit = &(yyvsp[-4].obj)->limit;
				limit->rate	= (yyvsp[-1].limit_rate).rate;
				limit->unit	= (yyvsp[-1].limit_rate).unit;
				limit->burst	= (yyvsp[0].val);
				limit->type	= NFT_LIMIT_PKTS;
				limit->flags	= (yyvsp[-2].val);
			}
#line 13312 "parser_bison.c"
    break;

  case 814: /* limit_config: "rate" limit_mode limit_rate_bytes limit_burst_bytes  */
#line 4631 "parser_bison.y"
                        {
				struct limit *limit;

				limit = &(yyvsp[-4].obj)->limit;
				limit->rate	= (yyvsp[-1].limit_rate).rate;
				limit->unit	= (yyvsp[-1].limit_rate).unit;
				limit->burst	= (yyvsp[0].val);
				limit->type	= NFT_LIMIT_PKT_BYTES;
				limit->flags	= (yyvsp[-2].val);
			}
#line 13327 "parser_bison.c"
    break;

  case 815: /* limit_obj: %empty  */
#line 4644 "parser_bison.y"
                        {
				(yyval.obj) = obj_alloc(&(yyloc));
				(yyval.obj)->type = NFT_OBJECT_LIMIT;
			}
#line 13336 "parser_bison.c"
    break;

  case 816: /* relational_expr: expr rhs_expr  */
#line 4651 "parser_bison.y"
                        {
				(yyval.expr) = relational_expr_alloc(&(yyloc), OP_IMPLICIT, (yyvsp[-1].expr), (yyvsp[0].expr));
			}
#line 13344 "parser_bison.c"
    break;

  case 817: /* relational_expr: expr list_rhs_expr  */
#line 4655 "parser_bison.y"
                        {
				(yyval.expr) = relational_expr_alloc(&(yyloc), OP_IMPLICIT, (yyvsp[-1].expr), (yyvsp[0].expr));
			}
#line 13352 "parser_bison.c"
    break;

  case 818: /* relational_expr: expr basic_rhs_expr "/" list_rhs_expr  */
#line 4659 "parser_bison.y"
                        {
				(yyval.expr) = flagcmp_expr_alloc(&(yyloc), OP_EQ, (yyvsp[-3].expr), (yyvsp[0].expr), (yyvsp[-2].expr));
			}
#line 13360 "parser_bison.c"
    break;

  case 819: /* relational_expr: expr list_rhs_expr "/" list_rhs_expr  */
#line 4663 "parser_bison.y"
                        {
				(yyval.expr) = flagcmp_expr_alloc(&(yyloc), OP_EQ, (yyvsp[-3].expr), (yyvsp[0].expr), (yyvsp[-2].expr));
			}
#line 13368 "parser_bison.c"
    break;

  case 820: /* relational_expr: expr relational_op basic_rhs_expr "/" list_rhs_expr  */
#line 4667 "parser_bison.y"
                        {
				(yyval.expr) = flagcmp_expr_alloc(&(yyloc), (yyvsp[-3].val), (yyvsp[-4].expr), (yyvsp[0].expr), (yyvsp[-2].expr));
			}
#line 13376 "parser_bison.c"
    break;

  case 821: /* relational_expr: expr relational_op list_rhs_expr "/" list_rhs_expr  */
#line 4671 "parser_bison.y"
                        {
				(yyval.expr) = flagcmp_expr_alloc(&(yyloc), (yyvsp[-3].val), (yyvsp[-4].expr), (yyvsp[0].expr), (yyvsp[-2].expr));
			}
#line 13384 "parser_bison.c"
    break;

  case 822: /* relational_expr: expr relational_op rhs_expr  */
#line 4675 "parser_bison.y"
                        {
				(yyval.expr) = relational_expr_alloc(&(yylsp[-1]), (yyvsp[-1].val), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13392 "parser_bison.c"
    break;

  case 823: /* relational_expr: expr relational_op list_rhs_expr  */
#line 4679 "parser_bison.y"
                        {
				(yyval.expr) = relational_expr_alloc(&(yylsp[-1]), (yyvsp[-1].val), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13400 "parser_bison.c"
    break;

  case 824: /* list_rhs_expr: basic_rhs_expr "comma" basic_rhs_expr  */
#line 4685 "parser_bison.y"
                        {
				(yyval.expr) = list_expr_alloc(&(yyloc));
				compound_expr_add((yyval.expr), (yyvsp[-2].expr));
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 13410 "parser_bison.c"
    break;

  case 825: /* list_rhs_expr: list_rhs_expr "comma" basic_rhs_expr  */
#line 4691 "parser_bison.y"
                        {
				(yyvsp[-2].expr)->location = (yyloc);
				compound_expr_add((yyvsp[-2].expr), (yyvsp[0].expr));
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 13420 "parser_bison.c"
    break;

  case 826: /* rhs_expr: concat_rhs_expr  */
#line 4698 "parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 13426 "parser_bison.c"
    break;

  case 827: /* rhs_expr: set_expr  */
#line 4699 "parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 13432 "parser_bison.c"
    break;

  case 828: /* rhs_expr: set_ref_symbol_expr  */
#line 4700 "parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 13438 "parser_bison.c"
    break;

  case 830: /* shift_rhs_expr: shift_rhs_expr "<<" primary_rhs_expr  */
#line 4705 "parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_LSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13446 "parser_bison.c"
    break;

  case 831: /* shift_rhs_expr: shift_rhs_expr ">>" primary_rhs_expr  */
#line 4709 "parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_RSHIFT, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13454 "parser_bison.c"
    break;

  case 833: /* and_rhs_expr: and_rhs_expr "&" shift_rhs_expr  */
#line 4716 "parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_AND, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13462 "parser_bison.c"
    break;

  case 835: /* exclusive_or_rhs_expr: exclusive_or_rhs_expr "^" and_rhs_expr  */
#line 4723 "parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_XOR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13470 "parser_bison.c"
    break;

  case 837: /* inclusive_or_rhs_expr: inclusive_or_rhs_expr '|' exclusive_or_rhs_expr  */
#line 4730 "parser_bison.y"
                        {
				(yyval.expr) = binop_expr_alloc(&(yyloc), OP_OR, (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 13478 "parser_bison.c"
    break;

  case 841: /* concat_rhs_expr: concat_rhs_expr "." multiton_rhs_expr  */
#line 4741 "parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 13491 "parser_bison.c"
    break;

  case 842: /* concat_rhs_expr: concat_rhs_expr "." basic_rhs_expr  */
#line 4750 "parser_bison.y"
                        {
				struct location rhs[] = {
					[1]	= (yylsp[-1]),
					[2]	= (yylsp[0]),
				};

				(yyval.expr) = handle_concat_expr(&(yyloc), (yyval.expr), (yyvsp[-2].expr), (yyvsp[0].expr), rhs);
			}
#line 13504 "parser_bison.c"
    break;

  case 843: /* boolean_keys: "exists"  */
#line 4760 "parser_bison.y"
                                                { (yyval.val8) = true; }
#line 13510 "parser_bison.c"
    break;

  case 844: /* boolean_keys: "missing"  */
#line 4761 "parser_bison.y"
                                                { (yyval.val8) = false; }
#line 13516 "parser_bison.c"
    break;

  case 845: /* boolean_expr: boolean_keys  */
#line 4765 "parser_bison.y"
                        {
				(yyval.expr) = constant_expr_alloc(&(yyloc), &boolean_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof((yyvsp[0].val8)) * BITS_PER_BYTE, &(yyvsp[0].val8));
			}
#line 13526 "parser_bison.c"
    break;

  case 846: /* keyword_expr: "ether" close_scope_eth  */
#line 4772 "parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "ether"); }
#line 13532 "parser_bison.c"
    break;

  case 847: /* keyword_expr: "ip" close_scope_ip  */
#line 4773 "parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "ip"); }
#line 13538 "parser_bison.c"
    break;

  case 848: /* keyword_expr: "ip6" close_scope_ip6  */
#line 4774 "parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "ip6"); }
#line 13544 "parser_bison.c"
    break;

  case 849: /* keyword_expr: "vlan" close_scope_vlan  */
#line 4775 "parser_bison.y"
                                                         { (yyval.expr) = symbol_value(&(yyloc), "vlan"); }
#line 13550 "parser_bison.c"
    break;

  case 850: /* keyword_expr: "arp" close_scope_arp  */
#line 4776 "parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "arp"); }
#line 13556 "parser_bison.c"
    break;

  case 851: /* keyword_expr: "dnat" close_scope_nat  */
#line 4777 "parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "dnat"); }
#line 13562 "parser_bison.c"
    break;

  case 852: /* keyword_expr: "snat" close_scope_nat  */
#line 4778 "parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "snat"); }
#line 13568 "parser_bison.c"
    break;

  case 853: /* keyword_expr: "ecn"  */
#line 4779 "parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "ecn"); }
#line 13574 "parser_bison.c"
    break;

  case 854: /* keyword_expr: "reset" close_scope_reset  */
#line 4780 "parser_bison.y"
                                                                { (yyval.expr) = symbol_value(&(yyloc), "reset"); }
#line 13580 "parser_bison.c"
    break;

  case 855: /* keyword_expr: "original"  */
#line 4781 "parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "original"); }
#line 13586 "parser_bison.c"
    break;

  case 856: /* keyword_expr: "reply"  */
#line 4782 "parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "reply"); }
#line 13592 "parser_bison.c"
    break;

  case 857: /* keyword_expr: "label"  */
#line 4783 "parser_bison.y"
                                                        { (yyval.expr) = symbol_value(&(yyloc), "label"); }
#line 13598 "parser_bison.c"
    break;

  case 858: /* primary_rhs_expr: symbol_expr  */
#line 4786 "parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 13604 "parser_bison.c"
    break;

  case 859: /* primary_rhs_expr: integer_expr  */
#line 4787 "parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 13610 "parser_bison.c"
    break;

  case 860: /* primary_rhs_expr: boolean_expr  */
#line 4788 "parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 13616 "parser_bison.c"
    break;

  case 861: /* primary_rhs_expr: keyword_expr  */
#line 4789 "parser_bison.y"
                                                        { (yyval.expr) = (yyvsp[0].expr); }
#line 13622 "parser_bison.c"
    break;

  case 862: /* primary_rhs_expr: "tcp" close_scope_tcp  */
#line 4791 "parser_bison.y"
                        {
				uint8_t data = IPPROTO_TCP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 13633 "parser_bison.c"
    break;

  case 863: /* primary_rhs_expr: "udp" close_scope_udp  */
#line 4798 "parser_bison.y"
                        {
				uint8_t data = IPPROTO_UDP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 13644 "parser_bison.c"
    break;

  case 864: /* primary_rhs_expr: "udplite" close_scope_udplite  */
#line 4805 "parser_bison.y"
                        {
				uint8_t data = IPPROTO_UDPLITE;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 13655 "parser_bison.c"
    break;

  case 865: /* primary_rhs_expr: "esp" close_scope_esp  */
#line 4812 "parser_bison.y"
                        {
				uint8_t data = IPPROTO_ESP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 13666 "parser_bison.c"
    break;

  case 866: /* primary_rhs_expr: "ah" close_scope_ah  */
#line 4819 "parser_bison.y"
                        {
				uint8_t data = IPPROTO_AH;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 13677 "parser_bison.c"
    break;

  case 867: /* primary_rhs_expr: "icmp" close_scope_icmp  */
#line 4826 "parser_bison.y"
                        {
				uint8_t data = IPPROTO_ICMP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 13688 "parser_bison.c"
    break;

  case 868: /* primary_rhs_expr: "igmp"  */
#line 4833 "parser_bison.y"
                        {
				uint8_t data = IPPROTO_IGMP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 13699 "parser_bison.c"
    break;

  case 869: /* primary_rhs_expr: "icmpv6" close_scope_icmp  */
#line 4840 "parser_bison.y"
                        {
				uint8_t data = IPPROTO_ICMPV6;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 13710 "parser_bison.c"
    break;

  case 870: /* primary_rhs_expr: "comp" close_scope_comp  */
#line 4847 "parser_bison.y"
                        {
				uint8_t data = IPPROTO_COMP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 13721 "parser_bison.c"
    break;

  case 871: /* primary_rhs_expr: "dccp" close_scope_dccp  */
#line 4854 "parser_bison.y"
                        {
				uint8_t data = IPPROTO_DCCP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 13732 "parser_bison.c"
    break;

  case 872: /* primary_rhs_expr: "sctp" close_scope_sctp  */
#line 4861 "parser_bison.y"
                        {
				uint8_t data = IPPROTO_SCTP;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &inet_protocol_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 13743 "parser_bison.c"
    break;

  case 873: /* primary_rhs_expr: "redirect" close_scope_nat  */
#line 4868 "parser_bison.y"
                        {
				uint8_t data = ICMP_REDIRECT;
				(yyval.expr) = constant_expr_alloc(&(yyloc), &icmp_type_type,
							 BYTEORDER_HOST_ENDIAN,
							 sizeof(data) * BITS_PER_BYTE, &data);
			}
#line 13754 "parser_bison.c"
    break;

  case 874: /* primary_rhs_expr: '(' basic_rhs_expr ')'  */
#line 4874 "parser_bison.y"
                                                                { (yyval.expr) = (yyvsp[-1].expr); }
#line 13760 "parser_bison.c"
    break;

  case 875: /* relational_op: "=="  */
#line 4877 "parser_bison.y"
                                                { (yyval.val) = OP_EQ; }
#line 13766 "parser_bison.c"
    break;

  case 876: /* relational_op: "!="  */
#line 4878 "parser_bison.y"
                                                { (yyval.val) = OP_NEQ; }
#line 13772 "parser_bison.c"
    break;

  case 877: /* relational_op: "<"  */
#line 4879 "parser_bison.y"
                                                { (yyval.val) = OP_LT; }
#line 13778 "parser_bison.c"
    break;

  case 878: /* relational_op: ">"  */
#line 4880 "parser_bison.y"
                                                { (yyval.val) = OP_GT; }
#line 13784 "parser_bison.c"
    break;

  case 879: /* relational_op: ">="  */
#line 4881 "parser_bison.y"
                                                { (yyval.val) = OP_GTE; }
#line 13790 "parser_bison.c"
    break;

  case 880: /* relational_op: "<="  */
#line 4882 "parser_bison.y"
                                                { (yyval.val) = OP_LTE; }
#line 13796 "parser_bison.c"
    break;

  case 881: /* relational_op: "!"  */
#line 4883 "parser_bison.y"
                                                { (yyval.val) = OP_NEG; }
#line 13802 "parser_bison.c"
    break;

  case 882: /* verdict_expr: "accept"  */
#line 4887 "parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NF_ACCEPT, NULL);
			}
#line 13810 "parser_bison.c"
    break;

  case 883: /* verdict_expr: "drop"  */
#line 4891 "parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NF_DROP, NULL);
			}
#line 13818 "parser_bison.c"
    break;

  case 884: /* verdict_expr: "continue"  */
#line 4895 "parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NFT_CONTINUE, NULL);
			}
#line 13826 "parser_bison.c"
    break;

  case 885: /* verdict_expr: "jump" chain_expr  */
#line 4899 "parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NFT_JUMP, (yyvsp[0].expr));
			}
#line 13834 "parser_bison.c"
    break;

  case 886: /* verdict_expr: "goto" chain_expr  */
#line 4903 "parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NFT_GOTO, (yyvsp[0].expr));
			}
#line 13842 "parser_bison.c"
    break;

  case 887: /* verdict_expr: "return"  */
#line 4907 "parser_bison.y"
                        {
				(yyval.expr) = verdict_expr_alloc(&(yyloc), NFT_RETURN, NULL);
			}
#line 13850 "parser_bison.c"
    break;

  case 889: /* chain_expr: identifier  */
#line 4914 "parser_bison.y"
                        {
				(yyval.expr) = constant_expr_alloc(&(yyloc), &string_type,
							 BYTEORDER_HOST_ENDIAN,
							 strlen((yyvsp[0].string)) * BITS_PER_BYTE,
							 (yyvsp[0].string));
				xfree((yyvsp[0].string));
			}
#line 13862 "parser_bison.c"
    break;

  case 890: /* meta_expr: "meta" meta_key close_scope_meta  */
#line 4924 "parser_bison.y"
                        {
				(yyval.expr) = meta_expr_alloc(&(yyloc), (yyvsp[-1].val));
			}
#line 13870 "parser_bison.c"
    break;

  case 891: /* meta_expr: meta_key_unqualified  */
#line 4928 "parser_bison.y"
                        {
				(yyval.expr) = meta_expr_alloc(&(yyloc), (yyvsp[0].val));
			}
#line 13878 "parser_bison.c"
    break;

  case 892: /* meta_expr: "meta" "string" close_scope_meta  */
#line 4932 "parser_bison.y"
                        {
				struct error_record *erec;
				unsigned int key;

				erec = meta_key_parse(&(yyloc), (yyvsp[-1].string), &key);
				xfree((yyvsp[-1].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}

				(yyval.expr) = meta_expr_alloc(&(yyloc), key);
			}
#line 13896 "parser_bison.c"
    break;

  case 895: /* meta_key_qualified: "length"  */
#line 4951 "parser_bison.y"
                                                { (yyval.val) = NFT_META_LEN; }
#line 13902 "parser_bison.c"
    break;

  case 896: /* meta_key_qualified: "protocol"  */
#line 4952 "parser_bison.y"
                                                { (yyval.val) = NFT_META_PROTOCOL; }
#line 13908 "parser_bison.c"
    break;

  case 897: /* meta_key_qualified: "priority"  */
#line 4953 "parser_bison.y"
                                                { (yyval.val) = NFT_META_PRIORITY; }
#line 13914 "parser_bison.c"
    break;

  case 898: /* meta_key_qualified: "random"  */
#line 4954 "parser_bison.y"
                                                { (yyval.val) = NFT_META_PRANDOM; }
#line 13920 "parser_bison.c"
    break;

  case 899: /* meta_key_qualified: "secmark" close_scope_secmark  */
#line 4955 "parser_bison.y"
                                                            { (yyval.val) = NFT_META_SECMARK; }
#line 13926 "parser_bison.c"
    break;

  case 900: /* meta_key_unqualified: "mark"  */
#line 4958 "parser_bison.y"
                                                { (yyval.val) = NFT_META_MARK; }
#line 13932 "parser_bison.c"
    break;

  case 901: /* meta_key_unqualified: "iif"  */
#line 4959 "parser_bison.y"
                                                { (yyval.val) = NFT_META_IIF; }
#line 13938 "parser_bison.c"
    break;

  case 902: /* meta_key_unqualified: "iifname"  */
#line 4960 "parser_bison.y"
                                                { (yyval.val) = NFT_META_IIFNAME; }
#line 13944 "parser_bison.c"
    break;

  case 903: /* meta_key_unqualified: "iiftype"  */
#line 4961 "parser_bison.y"
                                                { (yyval.val) = NFT_META_IIFTYPE; }
#line 13950 "parser_bison.c"
    break;

  case 904: /* meta_key_unqualified: "oif"  */
#line 4962 "parser_bison.y"
                                                { (yyval.val) = NFT_META_OIF; }
#line 13956 "parser_bison.c"
    break;

  case 905: /* meta_key_unqualified: "oifname"  */
#line 4963 "parser_bison.y"
                                                { (yyval.val) = NFT_META_OIFNAME; }
#line 13962 "parser_bison.c"
    break;

  case 906: /* meta_key_unqualified: "oiftype"  */
#line 4964 "parser_bison.y"
                                                { (yyval.val) = NFT_META_OIFTYPE; }
#line 13968 "parser_bison.c"
    break;

  case 907: /* meta_key_unqualified: "skuid"  */
#line 4965 "parser_bison.y"
                                                { (yyval.val) = NFT_META_SKUID; }
#line 13974 "parser_bison.c"
    break;

  case 908: /* meta_key_unqualified: "skgid"  */
#line 4966 "parser_bison.y"
                                                { (yyval.val) = NFT_META_SKGID; }
#line 13980 "parser_bison.c"
    break;

  case 909: /* meta_key_unqualified: "nftrace"  */
#line 4967 "parser_bison.y"
                                                { (yyval.val) = NFT_META_NFTRACE; }
#line 13986 "parser_bison.c"
    break;

  case 910: /* meta_key_unqualified: "rtclassid"  */
#line 4968 "parser_bison.y"
                                                { (yyval.val) = NFT_META_RTCLASSID; }
#line 13992 "parser_bison.c"
    break;

  case 911: /* meta_key_unqualified: "ibriport"  */
#line 4969 "parser_bison.y"
                                                { (yyval.val) = NFT_META_BRI_IIFNAME; }
#line 13998 "parser_bison.c"
    break;

  case 912: /* meta_key_unqualified: "obriport"  */
#line 4970 "parser_bison.y"
                                                { (yyval.val) = NFT_META_BRI_OIFNAME; }
#line 14004 "parser_bison.c"
    break;

  case 913: /* meta_key_unqualified: "ibrname"  */
#line 4971 "parser_bison.y"
                                                { (yyval.val) = NFT_META_BRI_IIFNAME; }
#line 14010 "parser_bison.c"
    break;

  case 914: /* meta_key_unqualified: "obrname"  */
#line 4972 "parser_bison.y"
                                                { (yyval.val) = NFT_META_BRI_OIFNAME; }
#line 14016 "parser_bison.c"
    break;

  case 915: /* meta_key_unqualified: "pkttype"  */
#line 4973 "parser_bison.y"
                                                { (yyval.val) = NFT_META_PKTTYPE; }
#line 14022 "parser_bison.c"
    break;

  case 916: /* meta_key_unqualified: "cpu"  */
#line 4974 "parser_bison.y"
                                                { (yyval.val) = NFT_META_CPU; }
#line 14028 "parser_bison.c"
    break;

  case 917: /* meta_key_unqualified: "iifgroup"  */
#line 4975 "parser_bison.y"
                                                { (yyval.val) = NFT_META_IIFGROUP; }
#line 14034 "parser_bison.c"
    break;

  case 918: /* meta_key_unqualified: "oifgroup"  */
#line 4976 "parser_bison.y"
                                                { (yyval.val) = NFT_META_OIFGROUP; }
#line 14040 "parser_bison.c"
    break;

  case 919: /* meta_key_unqualified: "cgroup"  */
#line 4977 "parser_bison.y"
                                                { (yyval.val) = NFT_META_CGROUP; }
#line 14046 "parser_bison.c"
    break;

  case 920: /* meta_key_unqualified: "ipsec" close_scope_ipsec  */
#line 4978 "parser_bison.y"
                                                          { (yyval.val) = NFT_META_SECPATH; }
#line 14052 "parser_bison.c"
    break;

  case 921: /* meta_key_unqualified: "time"  */
#line 4979 "parser_bison.y"
                                                { (yyval.val) = NFT_META_TIME_NS; }
#line 14058 "parser_bison.c"
    break;

  case 922: /* meta_key_unqualified: "day"  */
#line 4980 "parser_bison.y"
                                                { (yyval.val) = NFT_META_TIME_DAY; }
#line 14064 "parser_bison.c"
    break;

  case 923: /* meta_key_unqualified: "hour"  */
#line 4981 "parser_bison.y"
                                                { (yyval.val) = NFT_META_TIME_HOUR; }
#line 14070 "parser_bison.c"
    break;

  case 924: /* meta_stmt: "meta" meta_key "set" stmt_expr close_scope_meta  */
#line 4985 "parser_bison.y"
                        {
				switch ((yyvsp[-3].val)) {
				case NFT_META_SECMARK:
					switch ((yyvsp[-1].expr)->etype) {
					case EXPR_CT:
						(yyval.stmt) = meta_stmt_alloc(&(yyloc), (yyvsp[-3].val), (yyvsp[-1].expr));
						break;
					default:
						(yyval.stmt) = objref_stmt_alloc(&(yyloc));
						(yyval.stmt)->objref.type = NFT_OBJECT_SECMARK;
						(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
						break;
					}
					break;
				default:
					(yyval.stmt) = meta_stmt_alloc(&(yyloc), (yyvsp[-3].val), (yyvsp[-1].expr));
					break;
				}
			}
#line 14094 "parser_bison.c"
    break;

  case 925: /* meta_stmt: meta_key_unqualified "set" stmt_expr  */
#line 5005 "parser_bison.y"
                        {
				(yyval.stmt) = meta_stmt_alloc(&(yyloc), (yyvsp[-2].val), (yyvsp[0].expr));
			}
#line 14102 "parser_bison.c"
    break;

  case 926: /* meta_stmt: "meta" "string" "set" stmt_expr close_scope_meta  */
#line 5009 "parser_bison.y"
                        {
				struct error_record *erec;
				unsigned int key;

				erec = meta_key_parse(&(yyloc), (yyvsp[-3].string), &key);
				xfree((yyvsp[-3].string));
				if (erec != NULL) {
					erec_queue(erec, state->msgs);
					YYERROR;
				}

				(yyval.stmt) = meta_stmt_alloc(&(yyloc), key, (yyvsp[-1].expr));
			}
#line 14120 "parser_bison.c"
    break;

  case 927: /* meta_stmt: "notrack"  */
#line 5023 "parser_bison.y"
                        {
				(yyval.stmt) = notrack_stmt_alloc(&(yyloc));
			}
#line 14128 "parser_bison.c"
    break;

  case 928: /* meta_stmt: "flow" "offload" "@" string close_scope_at  */
#line 5027 "parser_bison.y"
                        {
				(yyval.stmt) = flow_offload_stmt_alloc(&(yyloc), (yyvsp[-1].string));
			}
#line 14136 "parser_bison.c"
    break;

  case 929: /* meta_stmt: "flow" "add" "@" string close_scope_at  */
#line 5031 "parser_bison.y"
                        {
				(yyval.stmt) = flow_offload_stmt_alloc(&(yyloc), (yyvsp[-1].string));
			}
#line 14144 "parser_bison.c"
    break;

  case 930: /* socket_expr: "socket" socket_key close_scope_socket  */
#line 5037 "parser_bison.y"
                        {
				(yyval.expr) = socket_expr_alloc(&(yyloc), (yyvsp[-1].val), 0);
			}
#line 14152 "parser_bison.c"
    break;

  case 931: /* socket_expr: "socket" "cgroupv2" "level" "number" close_scope_socket  */
#line 5041 "parser_bison.y"
                        {
				(yyval.expr) = socket_expr_alloc(&(yyloc), NFT_SOCKET_CGROUPV2, (yyvsp[-1].val));
			}
#line 14160 "parser_bison.c"
    break;

  case 932: /* socket_key: "transparent"  */
#line 5046 "parser_bison.y"
                                                { (yyval.val) = NFT_SOCKET_TRANSPARENT; }
#line 14166 "parser_bison.c"
    break;

  case 933: /* socket_key: "mark"  */
#line 5047 "parser_bison.y"
                                                { (yyval.val) = NFT_SOCKET_MARK; }
#line 14172 "parser_bison.c"
    break;

  case 934: /* socket_key: "wildcard"  */
#line 5048 "parser_bison.y"
                                                { (yyval.val) = NFT_SOCKET_WILDCARD; }
#line 14178 "parser_bison.c"
    break;

  case 935: /* offset_opt: %empty  */
#line 5051 "parser_bison.y"
                                                { (yyval.val) = 0; }
#line 14184 "parser_bison.c"
    break;

  case 936: /* offset_opt: "offset" "number"  */
#line 5052 "parser_bison.y"
                                                { (yyval.val) = (yyvsp[0].val); }
#line 14190 "parser_bison.c"
    break;

  case 937: /* numgen_type: "inc"  */
#line 5055 "parser_bison.y"
                                                { (yyval.val) = NFT_NG_INCREMENTAL; }
#line 14196 "parser_bison.c"
    break;

  case 938: /* numgen_type: "random"  */
#line 5056 "parser_bison.y"
                                                { (yyval.val) = NFT_NG_RANDOM; }
#line 14202 "parser_bison.c"
    break;

  case 939: /* numgen_expr: "numgen" numgen_type "mod" "number" offset_opt close_scope_numgen  */
#line 5060 "parser_bison.y"
                        {
				(yyval.expr) = numgen_expr_alloc(&(yyloc), (yyvsp[-4].val), (yyvsp[-2].val), (yyvsp[-1].val));
			}
#line 14210 "parser_bison.c"
    break;

  case 940: /* xfrm_spnum: "spnum" "number"  */
#line 5065 "parser_bison.y"
                                            { (yyval.val) = (yyvsp[0].val); }
#line 14216 "parser_bison.c"
    break;

  case 941: /* xfrm_spnum: %empty  */
#line 5066 "parser_bison.y"
                                            { (yyval.val) = 0; }
#line 14222 "parser_bison.c"
    break;

  case 942: /* xfrm_dir: "in"  */
#line 5069 "parser_bison.y"
                                        { (yyval.val) = XFRM_POLICY_IN; }
#line 14228 "parser_bison.c"
    break;

  case 943: /* xfrm_dir: "out"  */
#line 5070 "parser_bison.y"
                                        { (yyval.val) = XFRM_POLICY_OUT; }
#line 14234 "parser_bison.c"
    break;

  case 944: /* xfrm_state_key: "spi"  */
#line 5073 "parser_bison.y"
                                    { (yyval.val) = NFT_XFRM_KEY_SPI; }
#line 14240 "parser_bison.c"
    break;

  case 945: /* xfrm_state_key: "reqid"  */
#line 5074 "parser_bison.y"
                                      { (yyval.val) = NFT_XFRM_KEY_REQID; }
#line 14246 "parser_bison.c"
    break;

  case 946: /* xfrm_state_proto_key: "daddr"  */
#line 5077 "parser_bison.y"
                                                { (yyval.val) = NFT_XFRM_KEY_DADDR_IP4; }
#line 14252 "parser_bison.c"
    break;

  case 947: /* xfrm_state_proto_key: "saddr"  */
#line 5078 "parser_bison.y"
                                                { (yyval.val) = NFT_XFRM_KEY_SADDR_IP4; }
#line 14258 "parser_bison.c"
    break;

  case 948: /* xfrm_expr: "ipsec" xfrm_dir xfrm_spnum xfrm_state_key close_scope_ipsec  */
#line 5082 "parser_bison.y"
                        {
				if ((yyvsp[-2].val) > 255) {
					erec_queue(error(&(yylsp[-2]), "value too large"), state->msgs);
					YYERROR;
				}
				(yyval.expr) = xfrm_expr_alloc(&(yyloc), (yyvsp[-3].val), (yyvsp[-2].val), (yyvsp[-1].val));
			}
#line 14270 "parser_bison.c"
    break;

  case 949: /* xfrm_expr: "ipsec" xfrm_dir xfrm_spnum nf_key_proto xfrm_state_proto_key close_scope_ipsec  */
#line 5090 "parser_bison.y"
                        {
				enum nft_xfrm_keys xfrmk = (yyvsp[-1].val);

				switch ((yyvsp[-2].val)) {
				case NFPROTO_IPV4:
					break;
				case NFPROTO_IPV6:
					if ((yyvsp[-1].val) == NFT_XFRM_KEY_SADDR_IP4)
						xfrmk = NFT_XFRM_KEY_SADDR_IP6;
					else if ((yyvsp[-1].val) == NFT_XFRM_KEY_DADDR_IP4)
						xfrmk = NFT_XFRM_KEY_DADDR_IP6;
					break;
				default:
					YYERROR;
					break;
				}

				if ((yyvsp[-3].val) > 255) {
					erec_queue(error(&(yylsp[-3]), "value too large"), state->msgs);
					YYERROR;
				}

				(yyval.expr) = xfrm_expr_alloc(&(yyloc), (yyvsp[-4].val), (yyvsp[-3].val), xfrmk);
			}
#line 14299 "parser_bison.c"
    break;

  case 950: /* hash_expr: "jhash" expr "mod" "number" "seed" "number" offset_opt close_scope_hash  */
#line 5117 "parser_bison.y"
                        {
				(yyval.expr) = hash_expr_alloc(&(yyloc), (yyvsp[-4].val), true, (yyvsp[-2].val), (yyvsp[-1].val), NFT_HASH_JENKINS);
				(yyval.expr)->hash.expr = (yyvsp[-6].expr);
			}
#line 14308 "parser_bison.c"
    break;

  case 951: /* hash_expr: "jhash" expr "mod" "number" offset_opt close_scope_hash  */
#line 5122 "parser_bison.y"
                        {
				(yyval.expr) = hash_expr_alloc(&(yyloc), (yyvsp[-2].val), false, 0, (yyvsp[-1].val), NFT_HASH_JENKINS);
				(yyval.expr)->hash.expr = (yyvsp[-4].expr);
			}
#line 14317 "parser_bison.c"
    break;

  case 952: /* hash_expr: "symhash" "mod" "number" offset_opt close_scope_hash  */
#line 5127 "parser_bison.y"
                        {
				(yyval.expr) = hash_expr_alloc(&(yyloc), (yyvsp[-2].val), false, 0, (yyvsp[-1].val), NFT_HASH_SYM);
			}
#line 14325 "parser_bison.c"
    break;

  case 953: /* nf_key_proto: "ip" close_scope_ip  */
#line 5132 "parser_bison.y"
                                                       { (yyval.val) = NFPROTO_IPV4; }
#line 14331 "parser_bison.c"
    break;

  case 954: /* nf_key_proto: "ip6" close_scope_ip6  */
#line 5133 "parser_bison.y"
                                                        { (yyval.val) = NFPROTO_IPV6; }
#line 14337 "parser_bison.c"
    break;

  case 955: /* rt_expr: "rt" rt_key close_scope_rt  */
#line 5137 "parser_bison.y"
                        {
				(yyval.expr) = rt_expr_alloc(&(yyloc), (yyvsp[-1].val), true);
			}
#line 14345 "parser_bison.c"
    break;

  case 956: /* rt_expr: "rt" nf_key_proto rt_key close_scope_rt  */
#line 5141 "parser_bison.y"
                        {
				enum nft_rt_keys rtk = (yyvsp[-1].val);

				switch ((yyvsp[-2].val)) {
				case NFPROTO_IPV4:
					break;
				case NFPROTO_IPV6:
					if ((yyvsp[-1].val) == NFT_RT_NEXTHOP4)
						rtk = NFT_RT_NEXTHOP6;
					break;
				default:
					YYERROR;
					break;
				}

				(yyval.expr) = rt_expr_alloc(&(yyloc), rtk, false);
			}
#line 14367 "parser_bison.c"
    break;

  case 957: /* rt_key: "classid"  */
#line 5160 "parser_bison.y"
                                                { (yyval.val) = NFT_RT_CLASSID; }
#line 14373 "parser_bison.c"
    break;

  case 958: /* rt_key: "nexthop"  */
#line 5161 "parser_bison.y"
                                                { (yyval.val) = NFT_RT_NEXTHOP4; }
#line 14379 "parser_bison.c"
    break;

  case 959: /* rt_key: "mtu"  */
#line 5162 "parser_bison.y"
                                                { (yyval.val) = NFT_RT_TCPMSS; }
#line 14385 "parser_bison.c"
    break;

  case 960: /* rt_key: "ipsec" close_scope_ipsec  */
#line 5163 "parser_bison.y"
                                                          { (yyval.val) = NFT_RT_XFRM; }
#line 14391 "parser_bison.c"
    break;

  case 961: /* ct_expr: "ct" ct_key close_scope_ct  */
#line 5167 "parser_bison.y"
                        {
				(yyval.expr) = ct_expr_alloc(&(yyloc), (yyvsp[-1].val), -1);
			}
#line 14399 "parser_bison.c"
    break;

  case 962: /* ct_expr: "ct" ct_dir ct_key_dir close_scope_ct  */
#line 5171 "parser_bison.y"
                        {
				(yyval.expr) = ct_expr_alloc(&(yyloc), (yyvsp[-1].val), (yyvsp[-2].val));
			}
#line 14407 "parser_bison.c"
    break;

  case 963: /* ct_expr: "ct" ct_dir ct_key_proto_field close_scope_ct  */
#line 5175 "parser_bison.y"
                        {
				(yyval.expr) = ct_expr_alloc(&(yyloc), (yyvsp[-1].val), (yyvsp[-2].val));
			}
#line 14415 "parser_bison.c"
    break;

  case 964: /* ct_dir: "original"  */
#line 5180 "parser_bison.y"
                                                { (yyval.val) = IP_CT_DIR_ORIGINAL; }
#line 14421 "parser_bison.c"
    break;

  case 965: /* ct_dir: "reply"  */
#line 5181 "parser_bison.y"
                                                { (yyval.val) = IP_CT_DIR_REPLY; }
#line 14427 "parser_bison.c"
    break;

  case 966: /* ct_key: "l3proto"  */
#line 5184 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_L3PROTOCOL; }
#line 14433 "parser_bison.c"
    break;

  case 967: /* ct_key: "protocol"  */
#line 5185 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTOCOL; }
#line 14439 "parser_bison.c"
    break;

  case 968: /* ct_key: "mark"  */
#line 5186 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_MARK; }
#line 14445 "parser_bison.c"
    break;

  case 969: /* ct_key: "state"  */
#line 5187 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_STATE; }
#line 14451 "parser_bison.c"
    break;

  case 970: /* ct_key: "direction"  */
#line 5188 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_DIRECTION; }
#line 14457 "parser_bison.c"
    break;

  case 971: /* ct_key: "status"  */
#line 5189 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_STATUS; }
#line 14463 "parser_bison.c"
    break;

  case 972: /* ct_key: "expiration"  */
#line 5190 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_EXPIRATION; }
#line 14469 "parser_bison.c"
    break;

  case 973: /* ct_key: "helper"  */
#line 5191 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_HELPER; }
#line 14475 "parser_bison.c"
    break;

  case 974: /* ct_key: "saddr"  */
#line 5192 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_SRC; }
#line 14481 "parser_bison.c"
    break;

  case 975: /* ct_key: "daddr"  */
#line 5193 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_DST; }
#line 14487 "parser_bison.c"
    break;

  case 976: /* ct_key: "proto-src"  */
#line 5194 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTO_SRC; }
#line 14493 "parser_bison.c"
    break;

  case 977: /* ct_key: "proto-dst"  */
#line 5195 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTO_DST; }
#line 14499 "parser_bison.c"
    break;

  case 978: /* ct_key: "label"  */
#line 5196 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_LABELS; }
#line 14505 "parser_bison.c"
    break;

  case 979: /* ct_key: "event"  */
#line 5197 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_EVENTMASK; }
#line 14511 "parser_bison.c"
    break;

  case 980: /* ct_key: "secmark" close_scope_secmark  */
#line 5198 "parser_bison.y"
                                                            { (yyval.val) = NFT_CT_SECMARK; }
#line 14517 "parser_bison.c"
    break;

  case 981: /* ct_key: "id"  */
#line 5199 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_ID; }
#line 14523 "parser_bison.c"
    break;

  case 983: /* ct_key_dir: "saddr"  */
#line 5203 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_SRC; }
#line 14529 "parser_bison.c"
    break;

  case 984: /* ct_key_dir: "daddr"  */
#line 5204 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_DST; }
#line 14535 "parser_bison.c"
    break;

  case 985: /* ct_key_dir: "l3proto"  */
#line 5205 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_L3PROTOCOL; }
#line 14541 "parser_bison.c"
    break;

  case 986: /* ct_key_dir: "protocol"  */
#line 5206 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTOCOL; }
#line 14547 "parser_bison.c"
    break;

  case 987: /* ct_key_dir: "proto-src"  */
#line 5207 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTO_SRC; }
#line 14553 "parser_bison.c"
    break;

  case 988: /* ct_key_dir: "proto-dst"  */
#line 5208 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_PROTO_DST; }
#line 14559 "parser_bison.c"
    break;

  case 990: /* ct_key_proto_field: "ip" "saddr" close_scope_ip  */
#line 5212 "parser_bison.y"
                                                               { (yyval.val) = NFT_CT_SRC_IP; }
#line 14565 "parser_bison.c"
    break;

  case 991: /* ct_key_proto_field: "ip" "daddr" close_scope_ip  */
#line 5213 "parser_bison.y"
                                                               { (yyval.val) = NFT_CT_DST_IP; }
#line 14571 "parser_bison.c"
    break;

  case 992: /* ct_key_proto_field: "ip6" "saddr" close_scope_ip6  */
#line 5214 "parser_bison.y"
                                                                { (yyval.val) = NFT_CT_SRC_IP6; }
#line 14577 "parser_bison.c"
    break;

  case 993: /* ct_key_proto_field: "ip6" "daddr" close_scope_ip6  */
#line 5215 "parser_bison.y"
                                                                { (yyval.val) = NFT_CT_DST_IP6; }
#line 14583 "parser_bison.c"
    break;

  case 994: /* ct_key_dir_optional: "bytes"  */
#line 5218 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_BYTES; }
#line 14589 "parser_bison.c"
    break;

  case 995: /* ct_key_dir_optional: "packets"  */
#line 5219 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_PKTS; }
#line 14595 "parser_bison.c"
    break;

  case 996: /* ct_key_dir_optional: "avgpkt"  */
#line 5220 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_AVGPKT; }
#line 14601 "parser_bison.c"
    break;

  case 997: /* ct_key_dir_optional: "zone"  */
#line 5221 "parser_bison.y"
                                                { (yyval.val) = NFT_CT_ZONE; }
#line 14607 "parser_bison.c"
    break;

  case 1000: /* list_stmt_expr: symbol_stmt_expr "comma" symbol_stmt_expr  */
#line 5229 "parser_bison.y"
                        {
				(yyval.expr) = list_expr_alloc(&(yyloc));
				compound_expr_add((yyval.expr), (yyvsp[-2].expr));
				compound_expr_add((yyval.expr), (yyvsp[0].expr));
			}
#line 14617 "parser_bison.c"
    break;

  case 1001: /* list_stmt_expr: list_stmt_expr "comma" symbol_stmt_expr  */
#line 5235 "parser_bison.y"
                        {
				(yyvsp[-2].expr)->location = (yyloc);
				compound_expr_add((yyvsp[-2].expr), (yyvsp[0].expr));
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 14627 "parser_bison.c"
    break;

  case 1002: /* ct_stmt: "ct" ct_key "set" stmt_expr close_scope_ct  */
#line 5243 "parser_bison.y"
                        {
				switch ((yyvsp[-3].val)) {
				case NFT_CT_HELPER:
					(yyval.stmt) = objref_stmt_alloc(&(yyloc));
					(yyval.stmt)->objref.type = NFT_OBJECT_CT_HELPER;
					(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
					break;
				default:
					(yyval.stmt) = ct_stmt_alloc(&(yyloc), (yyvsp[-3].val), -1, (yyvsp[-1].expr));
					break;
				}
			}
#line 14644 "parser_bison.c"
    break;

  case 1003: /* ct_stmt: "ct" "timeout" "set" stmt_expr close_scope_ct  */
#line 5256 "parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_CT_TIMEOUT;
				(yyval.stmt)->objref.expr = (yyvsp[-1].expr);

			}
#line 14655 "parser_bison.c"
    break;

  case 1004: /* ct_stmt: "ct" "expectation" "set" stmt_expr close_scope_ct  */
#line 5263 "parser_bison.y"
                        {
				(yyval.stmt) = objref_stmt_alloc(&(yyloc));
				(yyval.stmt)->objref.type = NFT_OBJECT_CT_EXPECT;
				(yyval.stmt)->objref.expr = (yyvsp[-1].expr);
			}
#line 14665 "parser_bison.c"
    break;

  case 1005: /* ct_stmt: "ct" ct_dir ct_key_dir_optional "set" stmt_expr close_scope_ct  */
#line 5269 "parser_bison.y"
                        {
				(yyval.stmt) = ct_stmt_alloc(&(yyloc), (yyvsp[-3].val), (yyvsp[-4].val), (yyvsp[-1].expr));
			}
#line 14673 "parser_bison.c"
    break;

  case 1006: /* payload_stmt: payload_expr "set" stmt_expr  */
#line 5275 "parser_bison.y"
                        {
				if ((yyvsp[-2].expr)->etype == EXPR_EXTHDR)
					(yyval.stmt) = exthdr_stmt_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
				else
					(yyval.stmt) = payload_stmt_alloc(&(yyloc), (yyvsp[-2].expr), (yyvsp[0].expr));
			}
#line 14684 "parser_bison.c"
    break;

  case 1025: /* payload_raw_expr: "@" payload_base_spec "comma" "number" "comma" "number" close_scope_at  */
#line 5304 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), NULL, 0);
				payload_init_raw((yyval.expr), (yyvsp[-5].val), (yyvsp[-3].val), (yyvsp[-1].val));
				(yyval.expr)->byteorder		= BYTEORDER_BIG_ENDIAN;
				(yyval.expr)->payload.is_raw	= true;
			}
#line 14695 "parser_bison.c"
    break;

  case 1026: /* payload_base_spec: "ll"  */
#line 5312 "parser_bison.y"
                                                { (yyval.val) = PROTO_BASE_LL_HDR; }
#line 14701 "parser_bison.c"
    break;

  case 1027: /* payload_base_spec: "nh"  */
#line 5313 "parser_bison.y"
                                                { (yyval.val) = PROTO_BASE_NETWORK_HDR; }
#line 14707 "parser_bison.c"
    break;

  case 1028: /* payload_base_spec: "th" close_scope_th  */
#line 5314 "parser_bison.y"
                                                                { (yyval.val) = PROTO_BASE_TRANSPORT_HDR; }
#line 14713 "parser_bison.c"
    break;

  case 1029: /* payload_base_spec: "string"  */
#line 5316 "parser_bison.y"
                        {
				if (!strcmp((yyvsp[0].string), "ih")) {
					(yyval.val) = PROTO_BASE_INNER_HDR;
				} else {
					erec_queue(error(&(yylsp[0]), "unknown raw payload base"), state->msgs);
					xfree((yyvsp[0].string));
					YYERROR;
				}
				xfree((yyvsp[0].string));
			}
#line 14728 "parser_bison.c"
    break;

  case 1030: /* eth_hdr_expr: "ether" eth_hdr_field close_scope_eth  */
#line 5329 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_eth, (yyvsp[-1].val));
			}
#line 14736 "parser_bison.c"
    break;

  case 1031: /* eth_hdr_field: "saddr"  */
#line 5334 "parser_bison.y"
                                                { (yyval.val) = ETHHDR_SADDR; }
#line 14742 "parser_bison.c"
    break;

  case 1032: /* eth_hdr_field: "daddr"  */
#line 5335 "parser_bison.y"
                                                { (yyval.val) = ETHHDR_DADDR; }
#line 14748 "parser_bison.c"
    break;

  case 1033: /* eth_hdr_field: "type" close_scope_type  */
#line 5336 "parser_bison.y"
                                                                        { (yyval.val) = ETHHDR_TYPE; }
#line 14754 "parser_bison.c"
    break;

  case 1034: /* vlan_hdr_expr: "vlan" vlan_hdr_field close_scope_vlan  */
#line 5340 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_vlan, (yyvsp[-1].val));
			}
#line 14762 "parser_bison.c"
    break;

  case 1035: /* vlan_hdr_field: "id"  */
#line 5345 "parser_bison.y"
                                                { (yyval.val) = VLANHDR_VID; }
#line 14768 "parser_bison.c"
    break;

  case 1036: /* vlan_hdr_field: "cfi"  */
#line 5346 "parser_bison.y"
                                                { (yyval.val) = VLANHDR_CFI; }
#line 14774 "parser_bison.c"
    break;

  case 1037: /* vlan_hdr_field: "dei"  */
#line 5347 "parser_bison.y"
                                                { (yyval.val) = VLANHDR_DEI; }
#line 14780 "parser_bison.c"
    break;

  case 1038: /* vlan_hdr_field: "pcp"  */
#line 5348 "parser_bison.y"
                                                { (yyval.val) = VLANHDR_PCP; }
#line 14786 "parser_bison.c"
    break;

  case 1039: /* vlan_hdr_field: "type" close_scope_type  */
#line 5349 "parser_bison.y"
                                                                        { (yyval.val) = VLANHDR_TYPE; }
#line 14792 "parser_bison.c"
    break;

  case 1040: /* arp_hdr_expr: "arp" arp_hdr_field close_scope_arp  */
#line 5353 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_arp, (yyvsp[-1].val));
			}
#line 14800 "parser_bison.c"
    break;

  case 1041: /* arp_hdr_field: "htype"  */
#line 5358 "parser_bison.y"
                                                { (yyval.val) = ARPHDR_HRD; }
#line 14806 "parser_bison.c"
    break;

  case 1042: /* arp_hdr_field: "ptype"  */
#line 5359 "parser_bison.y"
                                                { (yyval.val) = ARPHDR_PRO; }
#line 14812 "parser_bison.c"
    break;

  case 1043: /* arp_hdr_field: "hlen"  */
#line 5360 "parser_bison.y"
                                                { (yyval.val) = ARPHDR_HLN; }
#line 14818 "parser_bison.c"
    break;

  case 1044: /* arp_hdr_field: "plen"  */
#line 5361 "parser_bison.y"
                                                { (yyval.val) = ARPHDR_PLN; }
#line 14824 "parser_bison.c"
    break;

  case 1045: /* arp_hdr_field: "operation"  */
#line 5362 "parser_bison.y"
                                                { (yyval.val) = ARPHDR_OP; }
#line 14830 "parser_bison.c"
    break;

  case 1046: /* arp_hdr_field: "saddr" "ether" close_scope_eth  */
#line 5363 "parser_bison.y"
                                                                { (yyval.val) = ARPHDR_SADDR_ETHER; }
#line 14836 "parser_bison.c"
    break;

  case 1047: /* arp_hdr_field: "daddr" "ether" close_scope_eth  */
#line 5364 "parser_bison.y"
                                                                { (yyval.val) = ARPHDR_DADDR_ETHER; }
#line 14842 "parser_bison.c"
    break;

  case 1048: /* arp_hdr_field: "saddr" "ip" close_scope_ip  */
#line 5365 "parser_bison.y"
                                                                { (yyval.val) = ARPHDR_SADDR_IP; }
#line 14848 "parser_bison.c"
    break;

  case 1049: /* arp_hdr_field: "daddr" "ip" close_scope_ip  */
#line 5366 "parser_bison.y"
                                                                { (yyval.val) = ARPHDR_DADDR_IP; }
#line 14854 "parser_bison.c"
    break;

  case 1050: /* ip_hdr_expr: "ip" ip_hdr_field close_scope_ip  */
#line 5370 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_ip, (yyvsp[-1].val));
			}
#line 14862 "parser_bison.c"
    break;

  case 1051: /* ip_hdr_expr: "ip" "option" ip_option_type ip_option_field close_scope_ip  */
#line 5374 "parser_bison.y"
                        {
				(yyval.expr) = ipopt_expr_alloc(&(yyloc), (yyvsp[-2].val), (yyvsp[-1].val));
				if (!(yyval.expr)) {
					erec_queue(error(&(yylsp[-4]), "unknown ip option type/field"), state->msgs);
					YYERROR;
				}
			}
#line 14874 "parser_bison.c"
    break;

  case 1052: /* ip_hdr_expr: "ip" "option" ip_option_type close_scope_ip  */
#line 5382 "parser_bison.y"
                        {
				(yyval.expr) = ipopt_expr_alloc(&(yyloc), (yyvsp[-1].val), IPOPT_FIELD_TYPE);
				(yyval.expr)->exthdr.flags = NFT_EXTHDR_F_PRESENT;
			}
#line 14883 "parser_bison.c"
    break;

  case 1053: /* ip_hdr_field: "version"  */
#line 5388 "parser_bison.y"
                                                { (yyval.val) = IPHDR_VERSION; }
#line 14889 "parser_bison.c"
    break;

  case 1054: /* ip_hdr_field: "hdrlength"  */
#line 5389 "parser_bison.y"
                                                { (yyval.val) = IPHDR_HDRLENGTH; }
#line 14895 "parser_bison.c"
    break;

  case 1055: /* ip_hdr_field: "dscp"  */
#line 5390 "parser_bison.y"
                                                { (yyval.val) = IPHDR_DSCP; }
#line 14901 "parser_bison.c"
    break;

  case 1056: /* ip_hdr_field: "ecn"  */
#line 5391 "parser_bison.y"
                                                { (yyval.val) = IPHDR_ECN; }
#line 14907 "parser_bison.c"
    break;

  case 1057: /* ip_hdr_field: "length"  */
#line 5392 "parser_bison.y"
                                                { (yyval.val) = IPHDR_LENGTH; }
#line 14913 "parser_bison.c"
    break;

  case 1058: /* ip_hdr_field: "id"  */
#line 5393 "parser_bison.y"
                                                { (yyval.val) = IPHDR_ID; }
#line 14919 "parser_bison.c"
    break;

  case 1059: /* ip_hdr_field: "frag-off"  */
#line 5394 "parser_bison.y"
                                                { (yyval.val) = IPHDR_FRAG_OFF; }
#line 14925 "parser_bison.c"
    break;

  case 1060: /* ip_hdr_field: "ttl"  */
#line 5395 "parser_bison.y"
                                                { (yyval.val) = IPHDR_TTL; }
#line 14931 "parser_bison.c"
    break;

  case 1061: /* ip_hdr_field: "protocol"  */
#line 5396 "parser_bison.y"
                                                { (yyval.val) = IPHDR_PROTOCOL; }
#line 14937 "parser_bison.c"
    break;

  case 1062: /* ip_hdr_field: "checksum"  */
#line 5397 "parser_bison.y"
                                                { (yyval.val) = IPHDR_CHECKSUM; }
#line 14943 "parser_bison.c"
    break;

  case 1063: /* ip_hdr_field: "saddr"  */
#line 5398 "parser_bison.y"
                                                { (yyval.val) = IPHDR_SADDR; }
#line 14949 "parser_bison.c"
    break;

  case 1064: /* ip_hdr_field: "daddr"  */
#line 5399 "parser_bison.y"
                                                { (yyval.val) = IPHDR_DADDR; }
#line 14955 "parser_bison.c"
    break;

  case 1065: /* ip_option_type: "lsrr"  */
#line 5402 "parser_bison.y"
                                                { (yyval.val) = IPOPT_LSRR; }
#line 14961 "parser_bison.c"
    break;

  case 1066: /* ip_option_type: "rr"  */
#line 5403 "parser_bison.y"
                                                { (yyval.val) = IPOPT_RR; }
#line 14967 "parser_bison.c"
    break;

  case 1067: /* ip_option_type: "ssrr"  */
#line 5404 "parser_bison.y"
                                                { (yyval.val) = IPOPT_SSRR; }
#line 14973 "parser_bison.c"
    break;

  case 1068: /* ip_option_type: "ra"  */
#line 5405 "parser_bison.y"
                                                { (yyval.val) = IPOPT_RA; }
#line 14979 "parser_bison.c"
    break;

  case 1069: /* ip_option_field: "type" close_scope_type  */
#line 5408 "parser_bison.y"
                                                                        { (yyval.val) = IPOPT_FIELD_TYPE; }
#line 14985 "parser_bison.c"
    break;

  case 1070: /* ip_option_field: "length"  */
#line 5409 "parser_bison.y"
                                                { (yyval.val) = IPOPT_FIELD_LENGTH; }
#line 14991 "parser_bison.c"
    break;

  case 1071: /* ip_option_field: "value"  */
#line 5410 "parser_bison.y"
                                                { (yyval.val) = IPOPT_FIELD_VALUE; }
#line 14997 "parser_bison.c"
    break;

  case 1072: /* ip_option_field: "ptr"  */
#line 5411 "parser_bison.y"
                                                { (yyval.val) = IPOPT_FIELD_PTR; }
#line 15003 "parser_bison.c"
    break;

  case 1073: /* ip_option_field: "addr"  */
#line 5412 "parser_bison.y"
                                                { (yyval.val) = IPOPT_FIELD_ADDR_0; }
#line 15009 "parser_bison.c"
    break;

  case 1074: /* icmp_hdr_expr: "icmp" icmp_hdr_field close_scope_icmp  */
#line 5416 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_icmp, (yyvsp[-1].val));
			}
#line 15017 "parser_bison.c"
    break;

  case 1075: /* icmp_hdr_field: "type" close_scope_type  */
#line 5421 "parser_bison.y"
                                                                        { (yyval.val) = ICMPHDR_TYPE; }
#line 15023 "parser_bison.c"
    break;

  case 1076: /* icmp_hdr_field: "code"  */
#line 5422 "parser_bison.y"
                                                { (yyval.val) = ICMPHDR_CODE; }
#line 15029 "parser_bison.c"
    break;

  case 1077: /* icmp_hdr_field: "checksum"  */
#line 5423 "parser_bison.y"
                                                { (yyval.val) = ICMPHDR_CHECKSUM; }
#line 15035 "parser_bison.c"
    break;

  case 1078: /* icmp_hdr_field: "id"  */
#line 5424 "parser_bison.y"
                                                { (yyval.val) = ICMPHDR_ID; }
#line 15041 "parser_bison.c"
    break;

  case 1079: /* icmp_hdr_field: "seq"  */
#line 5425 "parser_bison.y"
                                                { (yyval.val) = ICMPHDR_SEQ; }
#line 15047 "parser_bison.c"
    break;

  case 1080: /* icmp_hdr_field: "gateway"  */
#line 5426 "parser_bison.y"
                                                { (yyval.val) = ICMPHDR_GATEWAY; }
#line 15053 "parser_bison.c"
    break;

  case 1081: /* icmp_hdr_field: "mtu"  */
#line 5427 "parser_bison.y"
                                                { (yyval.val) = ICMPHDR_MTU; }
#line 15059 "parser_bison.c"
    break;

  case 1082: /* igmp_hdr_expr: "igmp" igmp_hdr_field close_scope_igmp  */
#line 5431 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_igmp, (yyvsp[-1].val));
			}
#line 15067 "parser_bison.c"
    break;

  case 1083: /* igmp_hdr_field: "type" close_scope_type  */
#line 5436 "parser_bison.y"
                                                                        { (yyval.val) = IGMPHDR_TYPE; }
#line 15073 "parser_bison.c"
    break;

  case 1084: /* igmp_hdr_field: "checksum"  */
#line 5437 "parser_bison.y"
                                                { (yyval.val) = IGMPHDR_CHECKSUM; }
#line 15079 "parser_bison.c"
    break;

  case 1085: /* igmp_hdr_field: "mrt"  */
#line 5438 "parser_bison.y"
                                                { (yyval.val) = IGMPHDR_MRT; }
#line 15085 "parser_bison.c"
    break;

  case 1086: /* igmp_hdr_field: "group"  */
#line 5439 "parser_bison.y"
                                                { (yyval.val) = IGMPHDR_GROUP; }
#line 15091 "parser_bison.c"
    break;

  case 1087: /* ip6_hdr_expr: "ip6" ip6_hdr_field close_scope_ip6  */
#line 5443 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_ip6, (yyvsp[-1].val));
			}
#line 15099 "parser_bison.c"
    break;

  case 1088: /* ip6_hdr_field: "version"  */
#line 5448 "parser_bison.y"
                                                { (yyval.val) = IP6HDR_VERSION; }
#line 15105 "parser_bison.c"
    break;

  case 1089: /* ip6_hdr_field: "dscp"  */
#line 5449 "parser_bison.y"
                                                { (yyval.val) = IP6HDR_DSCP; }
#line 15111 "parser_bison.c"
    break;

  case 1090: /* ip6_hdr_field: "ecn"  */
#line 5450 "parser_bison.y"
                                                { (yyval.val) = IP6HDR_ECN; }
#line 15117 "parser_bison.c"
    break;

  case 1091: /* ip6_hdr_field: "flowlabel"  */
#line 5451 "parser_bison.y"
                                                { (yyval.val) = IP6HDR_FLOWLABEL; }
#line 15123 "parser_bison.c"
    break;

  case 1092: /* ip6_hdr_field: "length"  */
#line 5452 "parser_bison.y"
                                                { (yyval.val) = IP6HDR_LENGTH; }
#line 15129 "parser_bison.c"
    break;

  case 1093: /* ip6_hdr_field: "nexthdr"  */
#line 5453 "parser_bison.y"
                                                { (yyval.val) = IP6HDR_NEXTHDR; }
#line 15135 "parser_bison.c"
    break;

  case 1094: /* ip6_hdr_field: "hoplimit"  */
#line 5454 "parser_bison.y"
                                                { (yyval.val) = IP6HDR_HOPLIMIT; }
#line 15141 "parser_bison.c"
    break;

  case 1095: /* ip6_hdr_field: "saddr"  */
#line 5455 "parser_bison.y"
                                                { (yyval.val) = IP6HDR_SADDR; }
#line 15147 "parser_bison.c"
    break;

  case 1096: /* ip6_hdr_field: "daddr"  */
#line 5456 "parser_bison.y"
                                                { (yyval.val) = IP6HDR_DADDR; }
#line 15153 "parser_bison.c"
    break;

  case 1097: /* icmp6_hdr_expr: "icmpv6" icmp6_hdr_field close_scope_icmp  */
#line 5459 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_icmp6, (yyvsp[-1].val));
			}
#line 15161 "parser_bison.c"
    break;

  case 1098: /* icmp6_hdr_field: "type" close_scope_type  */
#line 5464 "parser_bison.y"
                                                                        { (yyval.val) = ICMP6HDR_TYPE; }
#line 15167 "parser_bison.c"
    break;

  case 1099: /* icmp6_hdr_field: "code"  */
#line 5465 "parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_CODE; }
#line 15173 "parser_bison.c"
    break;

  case 1100: /* icmp6_hdr_field: "checksum"  */
#line 5466 "parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_CHECKSUM; }
#line 15179 "parser_bison.c"
    break;

  case 1101: /* icmp6_hdr_field: "param-problem"  */
#line 5467 "parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_PPTR; }
#line 15185 "parser_bison.c"
    break;

  case 1102: /* icmp6_hdr_field: "mtu"  */
#line 5468 "parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_MTU; }
#line 15191 "parser_bison.c"
    break;

  case 1103: /* icmp6_hdr_field: "id"  */
#line 5469 "parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_ID; }
#line 15197 "parser_bison.c"
    break;

  case 1104: /* icmp6_hdr_field: "seq"  */
#line 5470 "parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_SEQ; }
#line 15203 "parser_bison.c"
    break;

  case 1105: /* icmp6_hdr_field: "max-delay"  */
#line 5471 "parser_bison.y"
                                                { (yyval.val) = ICMP6HDR_MAXDELAY; }
#line 15209 "parser_bison.c"
    break;

  case 1106: /* auth_hdr_expr: "ah" auth_hdr_field close_scope_ah  */
#line 5475 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_ah, (yyvsp[-1].val));
			}
#line 15217 "parser_bison.c"
    break;

  case 1107: /* auth_hdr_field: "nexthdr"  */
#line 5480 "parser_bison.y"
                                                { (yyval.val) = AHHDR_NEXTHDR; }
#line 15223 "parser_bison.c"
    break;

  case 1108: /* auth_hdr_field: "hdrlength"  */
#line 5481 "parser_bison.y"
                                                { (yyval.val) = AHHDR_HDRLENGTH; }
#line 15229 "parser_bison.c"
    break;

  case 1109: /* auth_hdr_field: "reserved"  */
#line 5482 "parser_bison.y"
                                                { (yyval.val) = AHHDR_RESERVED; }
#line 15235 "parser_bison.c"
    break;

  case 1110: /* auth_hdr_field: "spi"  */
#line 5483 "parser_bison.y"
                                                { (yyval.val) = AHHDR_SPI; }
#line 15241 "parser_bison.c"
    break;

  case 1111: /* auth_hdr_field: "seq"  */
#line 5484 "parser_bison.y"
                                                { (yyval.val) = AHHDR_SEQUENCE; }
#line 15247 "parser_bison.c"
    break;

  case 1112: /* esp_hdr_expr: "esp" esp_hdr_field close_scope_esp  */
#line 5488 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_esp, (yyvsp[-1].val));
			}
#line 15255 "parser_bison.c"
    break;

  case 1113: /* esp_hdr_field: "spi"  */
#line 5493 "parser_bison.y"
                                                { (yyval.val) = ESPHDR_SPI; }
#line 15261 "parser_bison.c"
    break;

  case 1114: /* esp_hdr_field: "seq"  */
#line 5494 "parser_bison.y"
                                                { (yyval.val) = ESPHDR_SEQUENCE; }
#line 15267 "parser_bison.c"
    break;

  case 1115: /* comp_hdr_expr: "comp" comp_hdr_field close_scope_comp  */
#line 5498 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_comp, (yyvsp[-1].val));
			}
#line 15275 "parser_bison.c"
    break;

  case 1116: /* comp_hdr_field: "nexthdr"  */
#line 5503 "parser_bison.y"
                                                { (yyval.val) = COMPHDR_NEXTHDR; }
#line 15281 "parser_bison.c"
    break;

  case 1117: /* comp_hdr_field: "flags" close_scope_flags  */
#line 5504 "parser_bison.y"
                                                                { (yyval.val) = COMPHDR_FLAGS; }
#line 15287 "parser_bison.c"
    break;

  case 1118: /* comp_hdr_field: "cpi"  */
#line 5505 "parser_bison.y"
                                                { (yyval.val) = COMPHDR_CPI; }
#line 15293 "parser_bison.c"
    break;

  case 1119: /* udp_hdr_expr: "udp" udp_hdr_field close_scope_udp  */
#line 5509 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_udp, (yyvsp[-1].val));
			}
#line 15301 "parser_bison.c"
    break;

  case 1120: /* udp_hdr_field: "sport"  */
#line 5514 "parser_bison.y"
                                                { (yyval.val) = UDPHDR_SPORT; }
#line 15307 "parser_bison.c"
    break;

  case 1121: /* udp_hdr_field: "dport"  */
#line 5515 "parser_bison.y"
                                                { (yyval.val) = UDPHDR_DPORT; }
#line 15313 "parser_bison.c"
    break;

  case 1122: /* udp_hdr_field: "length"  */
#line 5516 "parser_bison.y"
                                                { (yyval.val) = UDPHDR_LENGTH; }
#line 15319 "parser_bison.c"
    break;

  case 1123: /* udp_hdr_field: "checksum"  */
#line 5517 "parser_bison.y"
                                                { (yyval.val) = UDPHDR_CHECKSUM; }
#line 15325 "parser_bison.c"
    break;

  case 1124: /* udplite_hdr_expr: "udplite" udplite_hdr_field close_scope_udplite  */
#line 5521 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_udplite, (yyvsp[-1].val));
			}
#line 15333 "parser_bison.c"
    break;

  case 1125: /* udplite_hdr_field: "sport"  */
#line 5526 "parser_bison.y"
                                                { (yyval.val) = UDPHDR_SPORT; }
#line 15339 "parser_bison.c"
    break;

  case 1126: /* udplite_hdr_field: "dport"  */
#line 5527 "parser_bison.y"
                                                { (yyval.val) = UDPHDR_DPORT; }
#line 15345 "parser_bison.c"
    break;

  case 1127: /* udplite_hdr_field: "csumcov"  */
#line 5528 "parser_bison.y"
                                                { (yyval.val) = UDPHDR_LENGTH; }
#line 15351 "parser_bison.c"
    break;

  case 1128: /* udplite_hdr_field: "checksum"  */
#line 5529 "parser_bison.y"
                                                { (yyval.val) = UDPHDR_CHECKSUM; }
#line 15357 "parser_bison.c"
    break;

  case 1129: /* tcp_hdr_expr: "tcp" tcp_hdr_field  */
#line 5533 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_tcp, (yyvsp[0].val));
			}
#line 15365 "parser_bison.c"
    break;

  case 1130: /* tcp_hdr_expr: "tcp" "option" tcp_hdr_option_type  */
#line 5537 "parser_bison.y"
                        {
				(yyval.expr) = tcpopt_expr_alloc(&(yyloc), (yyvsp[0].val), TCPOPT_COMMON_KIND);
				(yyval.expr)->exthdr.flags = NFT_EXTHDR_F_PRESENT;
			}
#line 15374 "parser_bison.c"
    break;

  case 1131: /* tcp_hdr_expr: "tcp" "option" tcp_hdr_option_kind_and_field  */
#line 5542 "parser_bison.y"
                        {
				(yyval.expr) = tcpopt_expr_alloc(&(yyloc), (yyvsp[0].tcp_kind_field).kind, (yyvsp[0].tcp_kind_field).field);
			}
#line 15382 "parser_bison.c"
    break;

  case 1132: /* tcp_hdr_expr: "tcp" "option" "@" close_scope_at tcp_hdr_option_type "comma" "number" "comma" "number"  */
#line 5546 "parser_bison.y"
                        {
				(yyval.expr) = tcpopt_expr_alloc(&(yyloc), (yyvsp[-4].val), 0);
				tcpopt_init_raw((yyval.expr), (yyvsp[-4].val), (yyvsp[-2].val), (yyvsp[0].val), 0);
			}
#line 15391 "parser_bison.c"
    break;

  case 1133: /* optstrip_stmt: "reset" "tcp" "option" tcp_hdr_option_type close_scope_tcp  */
#line 5553 "parser_bison.y"
                        {
				(yyval.stmt) = optstrip_stmt_alloc(&(yyloc), tcpopt_expr_alloc(&(yyloc),
										(yyvsp[-1].val), TCPOPT_COMMON_KIND));
			}
#line 15400 "parser_bison.c"
    break;

  case 1134: /* tcp_hdr_field: "sport"  */
#line 5559 "parser_bison.y"
                                                { (yyval.val) = TCPHDR_SPORT; }
#line 15406 "parser_bison.c"
    break;

  case 1135: /* tcp_hdr_field: "dport"  */
#line 5560 "parser_bison.y"
                                                { (yyval.val) = TCPHDR_DPORT; }
#line 15412 "parser_bison.c"
    break;

  case 1136: /* tcp_hdr_field: "seq"  */
#line 5561 "parser_bison.y"
                                                { (yyval.val) = TCPHDR_SEQ; }
#line 15418 "parser_bison.c"
    break;

  case 1137: /* tcp_hdr_field: "ackseq"  */
#line 5562 "parser_bison.y"
                                                { (yyval.val) = TCPHDR_ACKSEQ; }
#line 15424 "parser_bison.c"
    break;

  case 1138: /* tcp_hdr_field: "doff"  */
#line 5563 "parser_bison.y"
                                                { (yyval.val) = TCPHDR_DOFF; }
#line 15430 "parser_bison.c"
    break;

  case 1139: /* tcp_hdr_field: "reserved"  */
#line 5564 "parser_bison.y"
                                                { (yyval.val) = TCPHDR_RESERVED; }
#line 15436 "parser_bison.c"
    break;

  case 1140: /* tcp_hdr_field: "flags" close_scope_flags  */
#line 5565 "parser_bison.y"
                                                                { (yyval.val) = TCPHDR_FLAGS; }
#line 15442 "parser_bison.c"
    break;

  case 1141: /* tcp_hdr_field: "window"  */
#line 5566 "parser_bison.y"
                                                { (yyval.val) = TCPHDR_WINDOW; }
#line 15448 "parser_bison.c"
    break;

  case 1142: /* tcp_hdr_field: "checksum"  */
#line 5567 "parser_bison.y"
                                                { (yyval.val) = TCPHDR_CHECKSUM; }
#line 15454 "parser_bison.c"
    break;

  case 1143: /* tcp_hdr_field: "urgptr"  */
#line 5568 "parser_bison.y"
                                                { (yyval.val) = TCPHDR_URGPTR; }
#line 15460 "parser_bison.c"
    break;

  case 1144: /* tcp_hdr_option_kind_and_field: "mss" tcpopt_field_maxseg  */
#line 5572 "parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = TCPOPT_KIND_MAXSEG, .field = (yyvsp[0].val) };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 15469 "parser_bison.c"
    break;

  case 1145: /* tcp_hdr_option_kind_and_field: tcp_hdr_option_sack tcpopt_field_sack  */
#line 5577 "parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = (yyvsp[-1].val), .field = (yyvsp[0].val) };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 15478 "parser_bison.c"
    break;

  case 1146: /* tcp_hdr_option_kind_and_field: "window" tcpopt_field_window  */
#line 5582 "parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = TCPOPT_KIND_WINDOW, .field = (yyvsp[0].val) };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 15487 "parser_bison.c"
    break;

  case 1147: /* tcp_hdr_option_kind_and_field: "timestamp" tcpopt_field_tsopt  */
#line 5587 "parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = TCPOPT_KIND_TIMESTAMP, .field = (yyvsp[0].val) };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 15496 "parser_bison.c"
    break;

  case 1148: /* tcp_hdr_option_kind_and_field: tcp_hdr_option_type "length"  */
#line 5592 "parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = (yyvsp[-1].val), .field = TCPOPT_COMMON_LENGTH };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 15505 "parser_bison.c"
    break;

  case 1149: /* tcp_hdr_option_kind_and_field: "mptcp" tcpopt_field_mptcp  */
#line 5597 "parser_bison.y"
                                {
					struct tcp_kind_field kind_field = { .kind = TCPOPT_KIND_MPTCP, .field = (yyvsp[0].val) };
					(yyval.tcp_kind_field) = kind_field;
				}
#line 15514 "parser_bison.c"
    break;

  case 1150: /* tcp_hdr_option_sack: "sack"  */
#line 5603 "parser_bison.y"
                                                { (yyval.val) = TCPOPT_KIND_SACK; }
#line 15520 "parser_bison.c"
    break;

  case 1151: /* tcp_hdr_option_sack: "sack0"  */
#line 5604 "parser_bison.y"
                                                { (yyval.val) = TCPOPT_KIND_SACK; }
#line 15526 "parser_bison.c"
    break;

  case 1152: /* tcp_hdr_option_sack: "sack1"  */
#line 5605 "parser_bison.y"
                                                { (yyval.val) = TCPOPT_KIND_SACK1; }
#line 15532 "parser_bison.c"
    break;

  case 1153: /* tcp_hdr_option_sack: "sack2"  */
#line 5606 "parser_bison.y"
                                                { (yyval.val) = TCPOPT_KIND_SACK2; }
#line 15538 "parser_bison.c"
    break;

  case 1154: /* tcp_hdr_option_sack: "sack3"  */
#line 5607 "parser_bison.y"
                                                { (yyval.val) = TCPOPT_KIND_SACK3; }
#line 15544 "parser_bison.c"
    break;

  case 1155: /* tcp_hdr_option_type: "echo"  */
#line 5610 "parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_ECHO; }
#line 15550 "parser_bison.c"
    break;

  case 1156: /* tcp_hdr_option_type: "eol"  */
#line 5611 "parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_EOL; }
#line 15556 "parser_bison.c"
    break;

  case 1157: /* tcp_hdr_option_type: "fastopen"  */
#line 5612 "parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_FASTOPEN; }
#line 15562 "parser_bison.c"
    break;

  case 1158: /* tcp_hdr_option_type: "md5sig"  */
#line 5613 "parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_MD5SIG; }
#line 15568 "parser_bison.c"
    break;

  case 1159: /* tcp_hdr_option_type: "mptcp"  */
#line 5614 "parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_MPTCP; }
#line 15574 "parser_bison.c"
    break;

  case 1160: /* tcp_hdr_option_type: "mss"  */
#line 5615 "parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_MAXSEG; }
#line 15580 "parser_bison.c"
    break;

  case 1161: /* tcp_hdr_option_type: "nop"  */
#line 5616 "parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_NOP; }
#line 15586 "parser_bison.c"
    break;

  case 1162: /* tcp_hdr_option_type: "sack-permitted"  */
#line 5617 "parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_SACK_PERMITTED; }
#line 15592 "parser_bison.c"
    break;

  case 1163: /* tcp_hdr_option_type: "timestamp"  */
#line 5618 "parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_TIMESTAMP; }
#line 15598 "parser_bison.c"
    break;

  case 1164: /* tcp_hdr_option_type: "window"  */
#line 5619 "parser_bison.y"
                                                        { (yyval.val) = TCPOPT_KIND_WINDOW; }
#line 15604 "parser_bison.c"
    break;

  case 1165: /* tcp_hdr_option_type: tcp_hdr_option_sack  */
#line 5620 "parser_bison.y"
                                                        { (yyval.val) = (yyvsp[0].val); }
#line 15610 "parser_bison.c"
    break;

  case 1166: /* tcp_hdr_option_type: "number"  */
#line 5621 "parser_bison.y"
                                                        {
				if ((yyvsp[0].val) > 255) {
					erec_queue(error(&(yylsp[0]), "value too large"), state->msgs);
					YYERROR;
				}
				(yyval.val) = (yyvsp[0].val);
			}
#line 15622 "parser_bison.c"
    break;

  case 1167: /* tcpopt_field_sack: "left"  */
#line 5630 "parser_bison.y"
                                                { (yyval.val) = TCPOPT_SACK_LEFT; }
#line 15628 "parser_bison.c"
    break;

  case 1168: /* tcpopt_field_sack: "right"  */
#line 5631 "parser_bison.y"
                                                { (yyval.val) = TCPOPT_SACK_RIGHT; }
#line 15634 "parser_bison.c"
    break;

  case 1169: /* tcpopt_field_window: "count"  */
#line 5634 "parser_bison.y"
                                                { (yyval.val) = TCPOPT_WINDOW_COUNT; }
#line 15640 "parser_bison.c"
    break;

  case 1170: /* tcpopt_field_tsopt: "tsval"  */
#line 5637 "parser_bison.y"
                                                { (yyval.val) = TCPOPT_TS_TSVAL; }
#line 15646 "parser_bison.c"
    break;

  case 1171: /* tcpopt_field_tsopt: "tsecr"  */
#line 5638 "parser_bison.y"
                                                { (yyval.val) = TCPOPT_TS_TSECR; }
#line 15652 "parser_bison.c"
    break;

  case 1172: /* tcpopt_field_maxseg: "size"  */
#line 5641 "parser_bison.y"
                                                { (yyval.val) = TCPOPT_MAXSEG_SIZE; }
#line 15658 "parser_bison.c"
    break;

  case 1173: /* tcpopt_field_mptcp: "subtype"  */
#line 5644 "parser_bison.y"
                                                { (yyval.val) = TCPOPT_MPTCP_SUBTYPE; }
#line 15664 "parser_bison.c"
    break;

  case 1174: /* dccp_hdr_expr: "dccp" dccp_hdr_field close_scope_dccp  */
#line 5648 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_dccp, (yyvsp[-1].val));
			}
#line 15672 "parser_bison.c"
    break;

  case 1175: /* dccp_hdr_field: "sport"  */
#line 5653 "parser_bison.y"
                                                { (yyval.val) = DCCPHDR_SPORT; }
#line 15678 "parser_bison.c"
    break;

  case 1176: /* dccp_hdr_field: "dport"  */
#line 5654 "parser_bison.y"
                                                { (yyval.val) = DCCPHDR_DPORT; }
#line 15684 "parser_bison.c"
    break;

  case 1177: /* dccp_hdr_field: "type" close_scope_type  */
#line 5655 "parser_bison.y"
                                                                        { (yyval.val) = DCCPHDR_TYPE; }
#line 15690 "parser_bison.c"
    break;

  case 1178: /* sctp_chunk_type: "data"  */
#line 5658 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_DATA; }
#line 15696 "parser_bison.c"
    break;

  case 1179: /* sctp_chunk_type: "init"  */
#line 5659 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_INIT; }
#line 15702 "parser_bison.c"
    break;

  case 1180: /* sctp_chunk_type: "init-ack"  */
#line 5660 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_INIT_ACK; }
#line 15708 "parser_bison.c"
    break;

  case 1181: /* sctp_chunk_type: "sack"  */
#line 5661 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_SACK; }
#line 15714 "parser_bison.c"
    break;

  case 1182: /* sctp_chunk_type: "heartbeat"  */
#line 5662 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_HEARTBEAT; }
#line 15720 "parser_bison.c"
    break;

  case 1183: /* sctp_chunk_type: "heartbeat-ack"  */
#line 5663 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_HEARTBEAT_ACK; }
#line 15726 "parser_bison.c"
    break;

  case 1184: /* sctp_chunk_type: "abort"  */
#line 5664 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_ABORT; }
#line 15732 "parser_bison.c"
    break;

  case 1185: /* sctp_chunk_type: "shutdown"  */
#line 5665 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_SHUTDOWN; }
#line 15738 "parser_bison.c"
    break;

  case 1186: /* sctp_chunk_type: "shutdown-ack"  */
#line 5666 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_SHUTDOWN_ACK; }
#line 15744 "parser_bison.c"
    break;

  case 1187: /* sctp_chunk_type: "error"  */
#line 5667 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_ERROR; }
#line 15750 "parser_bison.c"
    break;

  case 1188: /* sctp_chunk_type: "cookie-echo"  */
#line 5668 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_COOKIE_ECHO; }
#line 15756 "parser_bison.c"
    break;

  case 1189: /* sctp_chunk_type: "cookie-ack"  */
#line 5669 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_COOKIE_ACK; }
#line 15762 "parser_bison.c"
    break;

  case 1190: /* sctp_chunk_type: "ecne"  */
#line 5670 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_ECNE; }
#line 15768 "parser_bison.c"
    break;

  case 1191: /* sctp_chunk_type: "cwr"  */
#line 5671 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_CWR; }
#line 15774 "parser_bison.c"
    break;

  case 1192: /* sctp_chunk_type: "shutdown-complete"  */
#line 5672 "parser_bison.y"
                                                  { (yyval.val) = SCTP_CHUNK_TYPE_SHUTDOWN_COMPLETE; }
#line 15780 "parser_bison.c"
    break;

  case 1193: /* sctp_chunk_type: "asconf-ack"  */
#line 5673 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_ASCONF_ACK; }
#line 15786 "parser_bison.c"
    break;

  case 1194: /* sctp_chunk_type: "forward-tsn"  */
#line 5674 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_FORWARD_TSN; }
#line 15792 "parser_bison.c"
    break;

  case 1195: /* sctp_chunk_type: "asconf"  */
#line 5675 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_TYPE_ASCONF; }
#line 15798 "parser_bison.c"
    break;

  case 1196: /* sctp_chunk_common_field: "type" close_scope_type  */
#line 5678 "parser_bison.y"
                                                                { (yyval.val) = SCTP_CHUNK_COMMON_TYPE; }
#line 15804 "parser_bison.c"
    break;

  case 1197: /* sctp_chunk_common_field: "flags" close_scope_flags  */
#line 5679 "parser_bison.y"
                                                                { (yyval.val) = SCTP_CHUNK_COMMON_FLAGS; }
#line 15810 "parser_bison.c"
    break;

  case 1198: /* sctp_chunk_common_field: "length"  */
#line 5680 "parser_bison.y"
                                        { (yyval.val) = SCTP_CHUNK_COMMON_LENGTH; }
#line 15816 "parser_bison.c"
    break;

  case 1199: /* sctp_chunk_data_field: "tsn"  */
#line 5683 "parser_bison.y"
                                        { (yyval.val) = SCTP_CHUNK_DATA_TSN; }
#line 15822 "parser_bison.c"
    break;

  case 1200: /* sctp_chunk_data_field: "stream"  */
#line 5684 "parser_bison.y"
                                        { (yyval.val) = SCTP_CHUNK_DATA_STREAM; }
#line 15828 "parser_bison.c"
    break;

  case 1201: /* sctp_chunk_data_field: "ssn"  */
#line 5685 "parser_bison.y"
                                        { (yyval.val) = SCTP_CHUNK_DATA_SSN; }
#line 15834 "parser_bison.c"
    break;

  case 1202: /* sctp_chunk_data_field: "ppid"  */
#line 5686 "parser_bison.y"
                                        { (yyval.val) = SCTP_CHUNK_DATA_PPID; }
#line 15840 "parser_bison.c"
    break;

  case 1203: /* sctp_chunk_init_field: "init-tag"  */
#line 5689 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_INIT_TAG; }
#line 15846 "parser_bison.c"
    break;

  case 1204: /* sctp_chunk_init_field: "a-rwnd"  */
#line 5690 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_INIT_RWND; }
#line 15852 "parser_bison.c"
    break;

  case 1205: /* sctp_chunk_init_field: "num-outbound-streams"  */
#line 5691 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_INIT_OSTREAMS; }
#line 15858 "parser_bison.c"
    break;

  case 1206: /* sctp_chunk_init_field: "num-inbound-streams"  */
#line 5692 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_INIT_ISTREAMS; }
#line 15864 "parser_bison.c"
    break;

  case 1207: /* sctp_chunk_init_field: "initial-tsn"  */
#line 5693 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_INIT_TSN; }
#line 15870 "parser_bison.c"
    break;

  case 1208: /* sctp_chunk_sack_field: "cum-tsn-ack"  */
#line 5696 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_SACK_CTSN_ACK; }
#line 15876 "parser_bison.c"
    break;

  case 1209: /* sctp_chunk_sack_field: "a-rwnd"  */
#line 5697 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_SACK_RWND; }
#line 15882 "parser_bison.c"
    break;

  case 1210: /* sctp_chunk_sack_field: "num-gap-ack-blocks"  */
#line 5698 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_SACK_GACK_BLOCKS; }
#line 15888 "parser_bison.c"
    break;

  case 1211: /* sctp_chunk_sack_field: "num-dup-tsns"  */
#line 5699 "parser_bison.y"
                                                { (yyval.val) = SCTP_CHUNK_SACK_DUP_TSNS; }
#line 15894 "parser_bison.c"
    break;

  case 1212: /* sctp_chunk_alloc: sctp_chunk_type  */
#line 5703 "parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), (yyvsp[0].val), SCTP_CHUNK_COMMON_TYPE);
				(yyval.expr)->exthdr.flags = NFT_EXTHDR_F_PRESENT;
			}
#line 15903 "parser_bison.c"
    break;

  case 1213: /* sctp_chunk_alloc: sctp_chunk_type sctp_chunk_common_field  */
#line 5708 "parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), (yyvsp[-1].val), (yyvsp[0].val));
			}
#line 15911 "parser_bison.c"
    break;

  case 1214: /* sctp_chunk_alloc: "data" sctp_chunk_data_field  */
#line 5712 "parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_DATA, (yyvsp[0].val));
			}
#line 15919 "parser_bison.c"
    break;

  case 1215: /* sctp_chunk_alloc: "init" sctp_chunk_init_field  */
#line 5716 "parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_INIT, (yyvsp[0].val));
			}
#line 15927 "parser_bison.c"
    break;

  case 1216: /* sctp_chunk_alloc: "init-ack" sctp_chunk_init_field  */
#line 5720 "parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_INIT_ACK, (yyvsp[0].val));
			}
#line 15935 "parser_bison.c"
    break;

  case 1217: /* sctp_chunk_alloc: "sack" sctp_chunk_sack_field  */
#line 5724 "parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_SACK, (yyvsp[0].val));
			}
#line 15943 "parser_bison.c"
    break;

  case 1218: /* sctp_chunk_alloc: "shutdown" "cum-tsn-ack"  */
#line 5728 "parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_SHUTDOWN,
							   SCTP_CHUNK_SHUTDOWN_CTSN_ACK);
			}
#line 15952 "parser_bison.c"
    break;

  case 1219: /* sctp_chunk_alloc: "ecne" "lowest-tsn"  */
#line 5733 "parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_ECNE,
							   SCTP_CHUNK_ECNE_CWR_MIN_TSN);
			}
#line 15961 "parser_bison.c"
    break;

  case 1220: /* sctp_chunk_alloc: "cwr" "lowest-tsn"  */
#line 5738 "parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_CWR,
							   SCTP_CHUNK_ECNE_CWR_MIN_TSN);
			}
#line 15970 "parser_bison.c"
    break;

  case 1221: /* sctp_chunk_alloc: "asconf-ack" "seqno"  */
#line 5743 "parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_ASCONF_ACK,
							   SCTP_CHUNK_ASCONF_SEQNO);
			}
#line 15979 "parser_bison.c"
    break;

  case 1222: /* sctp_chunk_alloc: "forward-tsn" "new-cum-tsn"  */
#line 5748 "parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_FORWARD_TSN,
							   SCTP_CHUNK_FORWARD_TSN_NCTSN);
			}
#line 15988 "parser_bison.c"
    break;

  case 1223: /* sctp_chunk_alloc: "asconf" "seqno"  */
#line 5753 "parser_bison.y"
                        {
				(yyval.expr) = sctp_chunk_expr_alloc(&(yyloc), SCTP_CHUNK_TYPE_ASCONF,
							   SCTP_CHUNK_ASCONF_SEQNO);
			}
#line 15997 "parser_bison.c"
    break;

  case 1224: /* sctp_hdr_expr: "sctp" sctp_hdr_field close_scope_sctp  */
#line 5760 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_sctp, (yyvsp[-1].val));
			}
#line 16005 "parser_bison.c"
    break;

  case 1225: /* sctp_hdr_expr: "sctp" "chunk" sctp_chunk_alloc close_scope_sctp_chunk close_scope_sctp  */
#line 5764 "parser_bison.y"
                        {
				(yyval.expr) = (yyvsp[-2].expr);
			}
#line 16013 "parser_bison.c"
    break;

  case 1226: /* sctp_hdr_field: "sport"  */
#line 5769 "parser_bison.y"
                                                { (yyval.val) = SCTPHDR_SPORT; }
#line 16019 "parser_bison.c"
    break;

  case 1227: /* sctp_hdr_field: "dport"  */
#line 5770 "parser_bison.y"
                                                { (yyval.val) = SCTPHDR_DPORT; }
#line 16025 "parser_bison.c"
    break;

  case 1228: /* sctp_hdr_field: "vtag"  */
#line 5771 "parser_bison.y"
                                                { (yyval.val) = SCTPHDR_VTAG; }
#line 16031 "parser_bison.c"
    break;

  case 1229: /* sctp_hdr_field: "checksum"  */
#line 5772 "parser_bison.y"
                                                { (yyval.val) = SCTPHDR_CHECKSUM; }
#line 16037 "parser_bison.c"
    break;

  case 1230: /* th_hdr_expr: "th" th_hdr_field close_scope_th  */
#line 5776 "parser_bison.y"
                        {
				(yyval.expr) = payload_expr_alloc(&(yyloc), &proto_th, (yyvsp[-1].val));
				if ((yyval.expr))
					(yyval.expr)->payload.is_raw = true;
			}
#line 16047 "parser_bison.c"
    break;

  case 1231: /* th_hdr_field: "sport"  */
#line 5783 "parser_bison.y"
                                                { (yyval.val) = THDR_SPORT; }
#line 16053 "parser_bison.c"
    break;

  case 1232: /* th_hdr_field: "dport"  */
#line 5784 "parser_bison.y"
                                                { (yyval.val) = THDR_DPORT; }
#line 16059 "parser_bison.c"
    break;

  case 1241: /* hbh_hdr_expr: "hbh" hbh_hdr_field close_scope_hbh  */
#line 5798 "parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_hbh, (yyvsp[-1].val));
			}
#line 16067 "parser_bison.c"
    break;

  case 1242: /* hbh_hdr_field: "nexthdr"  */
#line 5803 "parser_bison.y"
                                                { (yyval.val) = HBHHDR_NEXTHDR; }
#line 16073 "parser_bison.c"
    break;

  case 1243: /* hbh_hdr_field: "hdrlength"  */
#line 5804 "parser_bison.y"
                                                { (yyval.val) = HBHHDR_HDRLENGTH; }
#line 16079 "parser_bison.c"
    break;

  case 1244: /* rt_hdr_expr: "rt" rt_hdr_field close_scope_rt  */
#line 5808 "parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_rt, (yyvsp[-1].val));
			}
#line 16087 "parser_bison.c"
    break;

  case 1245: /* rt_hdr_field: "nexthdr"  */
#line 5813 "parser_bison.y"
                                                { (yyval.val) = RTHDR_NEXTHDR; }
#line 16093 "parser_bison.c"
    break;

  case 1246: /* rt_hdr_field: "hdrlength"  */
#line 5814 "parser_bison.y"
                                                { (yyval.val) = RTHDR_HDRLENGTH; }
#line 16099 "parser_bison.c"
    break;

  case 1247: /* rt_hdr_field: "type" close_scope_type  */
#line 5815 "parser_bison.y"
                                                                        { (yyval.val) = RTHDR_TYPE; }
#line 16105 "parser_bison.c"
    break;

  case 1248: /* rt_hdr_field: "seg-left"  */
#line 5816 "parser_bison.y"
                                                { (yyval.val) = RTHDR_SEG_LEFT; }
#line 16111 "parser_bison.c"
    break;

  case 1249: /* rt0_hdr_expr: "rt0" rt0_hdr_field close_scope_rt  */
#line 5820 "parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_rt0, (yyvsp[-1].val));
			}
#line 16119 "parser_bison.c"
    break;

  case 1250: /* rt0_hdr_field: "addr" '[' "number" ']'  */
#line 5826 "parser_bison.y"
                        {
				(yyval.val) = RT0HDR_ADDR_1 + (yyvsp[-1].val) - 1;
			}
#line 16127 "parser_bison.c"
    break;

  case 1251: /* rt2_hdr_expr: "rt2" rt2_hdr_field close_scope_rt  */
#line 5832 "parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_rt2, (yyvsp[-1].val));
			}
#line 16135 "parser_bison.c"
    break;

  case 1252: /* rt2_hdr_field: "addr"  */
#line 5837 "parser_bison.y"
                                                { (yyval.val) = RT2HDR_ADDR; }
#line 16141 "parser_bison.c"
    break;

  case 1253: /* rt4_hdr_expr: "srh" rt4_hdr_field close_scope_rt  */
#line 5841 "parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_rt4, (yyvsp[-1].val));
			}
#line 16149 "parser_bison.c"
    break;

  case 1254: /* rt4_hdr_field: "last-entry"  */
#line 5846 "parser_bison.y"
                                                { (yyval.val) = RT4HDR_LASTENT; }
#line 16155 "parser_bison.c"
    break;

  case 1255: /* rt4_hdr_field: "flags" close_scope_flags  */
#line 5847 "parser_bison.y"
                                                                { (yyval.val) = RT4HDR_FLAGS; }
#line 16161 "parser_bison.c"
    break;

  case 1256: /* rt4_hdr_field: "tag"  */
#line 5848 "parser_bison.y"
                                                { (yyval.val) = RT4HDR_TAG; }
#line 16167 "parser_bison.c"
    break;

  case 1257: /* rt4_hdr_field: "sid" '[' "number" ']'  */
#line 5850 "parser_bison.y"
                        {
				(yyval.val) = RT4HDR_SID_1 + (yyvsp[-1].val) - 1;
			}
#line 16175 "parser_bison.c"
    break;

  case 1258: /* frag_hdr_expr: "frag" frag_hdr_field close_scope_frag  */
#line 5856 "parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_frag, (yyvsp[-1].val));
			}
#line 16183 "parser_bison.c"
    break;

  case 1259: /* frag_hdr_field: "nexthdr"  */
#line 5861 "parser_bison.y"
                                                { (yyval.val) = FRAGHDR_NEXTHDR; }
#line 16189 "parser_bison.c"
    break;

  case 1260: /* frag_hdr_field: "reserved"  */
#line 5862 "parser_bison.y"
                                                { (yyval.val) = FRAGHDR_RESERVED; }
#line 16195 "parser_bison.c"
    break;

  case 1261: /* frag_hdr_field: "frag-off"  */
#line 5863 "parser_bison.y"
                                                { (yyval.val) = FRAGHDR_FRAG_OFF; }
#line 16201 "parser_bison.c"
    break;

  case 1262: /* frag_hdr_field: "reserved2"  */
#line 5864 "parser_bison.y"
                                                { (yyval.val) = FRAGHDR_RESERVED2; }
#line 16207 "parser_bison.c"
    break;

  case 1263: /* frag_hdr_field: "more-fragments"  */
#line 5865 "parser_bison.y"
                                                { (yyval.val) = FRAGHDR_MFRAGS; }
#line 16213 "parser_bison.c"
    break;

  case 1264: /* frag_hdr_field: "id"  */
#line 5866 "parser_bison.y"
                                                { (yyval.val) = FRAGHDR_ID; }
#line 16219 "parser_bison.c"
    break;

  case 1265: /* dst_hdr_expr: "dst" dst_hdr_field close_scope_dst  */
#line 5870 "parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_dst, (yyvsp[-1].val));
			}
#line 16227 "parser_bison.c"
    break;

  case 1266: /* dst_hdr_field: "nexthdr"  */
#line 5875 "parser_bison.y"
                                                { (yyval.val) = DSTHDR_NEXTHDR; }
#line 16233 "parser_bison.c"
    break;

  case 1267: /* dst_hdr_field: "hdrlength"  */
#line 5876 "parser_bison.y"
                                                { (yyval.val) = DSTHDR_HDRLENGTH; }
#line 16239 "parser_bison.c"
    break;

  case 1268: /* mh_hdr_expr: "mh" mh_hdr_field close_scope_mh  */
#line 5880 "parser_bison.y"
                        {
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), &exthdr_mh, (yyvsp[-1].val));
			}
#line 16247 "parser_bison.c"
    break;

  case 1269: /* mh_hdr_field: "nexthdr"  */
#line 5885 "parser_bison.y"
                                                { (yyval.val) = MHHDR_NEXTHDR; }
#line 16253 "parser_bison.c"
    break;

  case 1270: /* mh_hdr_field: "hdrlength"  */
#line 5886 "parser_bison.y"
                                                { (yyval.val) = MHHDR_HDRLENGTH; }
#line 16259 "parser_bison.c"
    break;

  case 1271: /* mh_hdr_field: "type" close_scope_type  */
#line 5887 "parser_bison.y"
                                                                        { (yyval.val) = MHHDR_TYPE; }
#line 16265 "parser_bison.c"
    break;

  case 1272: /* mh_hdr_field: "reserved"  */
#line 5888 "parser_bison.y"
                                                { (yyval.val) = MHHDR_RESERVED; }
#line 16271 "parser_bison.c"
    break;

  case 1273: /* mh_hdr_field: "checksum"  */
#line 5889 "parser_bison.y"
                                                { (yyval.val) = MHHDR_CHECKSUM; }
#line 16277 "parser_bison.c"
    break;

  case 1274: /* exthdr_exists_expr: "exthdr" exthdr_key  */
#line 5893 "parser_bison.y"
                        {
				const struct exthdr_desc *desc;

				desc = exthdr_find_proto((yyvsp[0].val));

				/* Assume that NEXTHDR template is always
				 * the fist one in list of templates.
				 */
				(yyval.expr) = exthdr_expr_alloc(&(yyloc), desc, 1);
				(yyval.expr)->exthdr.flags = NFT_EXTHDR_F_PRESENT;
			}
#line 16293 "parser_bison.c"
    break;

  case 1275: /* exthdr_key: "hbh" close_scope_hbh  */
#line 5906 "parser_bison.y"
                                                        { (yyval.val) = IPPROTO_HOPOPTS; }
#line 16299 "parser_bison.c"
    break;

  case 1276: /* exthdr_key: "rt" close_scope_rt  */
#line 5907 "parser_bison.y"
                                                        { (yyval.val) = IPPROTO_ROUTING; }
#line 16305 "parser_bison.c"
    break;

  case 1277: /* exthdr_key: "frag" close_scope_frag  */
#line 5908 "parser_bison.y"
                                                                { (yyval.val) = IPPROTO_FRAGMENT; }
#line 16311 "parser_bison.c"
    break;

  case 1278: /* exthdr_key: "dst" close_scope_dst  */
#line 5909 "parser_bison.y"
                                                        { (yyval.val) = IPPROTO_DSTOPTS; }
#line 16317 "parser_bison.c"
    break;

  case 1279: /* exthdr_key: "mh" close_scope_mh  */
#line 5910 "parser_bison.y"
                                                        { (yyval.val) = IPPROTO_MH; }
#line 16323 "parser_bison.c"
    break;


#line 16327 "parser_bison.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (&yylloc, nft, scanner, state, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          goto yyexhaustedlab;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= TOKEN_EOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == TOKEN_EOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, nft, scanner, state);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, nft, scanner, state);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if 1
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, nft, scanner, state, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, nft, scanner, state);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, nft, scanner, state);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 5913 "parser_bison.y"

