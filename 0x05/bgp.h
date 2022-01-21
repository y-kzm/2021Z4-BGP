#ifndef BGP_H
#include <stdbool.h>

/* BGP State */
enum STATE {
    IDLE_STATE,         // 0
    CONNECT_STATE,      // 1
    OPENSENT_STATE,     // 2
    ACTIVE_STATE,       // 3
    OPENCONFIRM_STATE,  // 4
    ESTABLISHED_STATE   // 5
};

/* BGP Message Types. */
#define OPEN_MSG          1
#define UPDATE_MSG        2
#define NOTIFICATION_MSG  3
#define KEEPALIVE_MSG     4

/* BGP Path Attribute Type. */
#define ORIGIN          1
#define AS_PATH         2
#define NEXT_HOP        3
#define MULTI_EXIT_DISC 4

/* Path Attribute Origin. */
#define ORIGIN_IGP          0
#define OROGON_EGP          1
#define ORIGIN_INCOMPLETE   2

/* Path Attribute AS_Path Segment Types. */
#define AS_SET              1
#define AS_SEQUENCE         2
#define AS_CONFED_SEQUENCE  3
#define AS_CONFED_SET       4

/* BGP Message Length. */
#define MARKER_LEN              16
#define BGP_HDR_LEN             19
#define BGP_OPEN_LEN            29
#define BGP_OPEN_OPT_LEN        46
#define BGP_OPEN_OPT_TOTAL_LEN  75

/* Utilt. */
struct peer {
    enum STATE state;
};

struct network {
    uint8_t prefix_len;
    uint8_t prefix[4];
} __attribute__((__packed__));


/*
    >>>>    Msg Format.    <<<<
*/
/* BGP Header Format. */
struct bgp_hdr {
    uint8_t marker[16];  
    uint16_t len;
    uint8_t type;
} __attribute__((__packed__));

/* BGP Open Msg Format. */
struct bgp_open {
    struct bgp_hdr  hdr; 
    uint8_t version;
    uint16_t myas;
    uint16_t holdtime;
    struct in_addr id;
    uint8_t opt_len;
} __attribute__((__packed__));

/* BGP Open Msg Opt Format. */
struct bgp_open_opt {
    struct bgp_hdr  hdr;
    uint8_t version;
    uint16_t myas;
    uint16_t holdtime;
    struct in_addr id;
    uint8_t opt_len;
    uint8_t opt[46];
} __attribute__((__packed__));

/* BGP Update Msg Format. */
struct withdrawn_routes {
    uint16_t len;
    struct network routes[256];
};
struct total_path_attrib {
    uint16_t total_len;
    uint8_t path_attrib[64];
};
struct bgp_update {
    struct bgp_hdr  hdr;
    uint8_t contents[4096];
        /*  ~ Memo. ~
            > uint16_t  withdrawn_routes_len;
            > uint8_t   withdrawn_routes[variable]
            > uint16_t  total_path_len
            > uint8_t   path_attrib[variable]
            > uint8_t   nlri[varriable] 
        */
} __attribute__((__packed__));




/* 
    >>>>    Path Atribute.    <<<< 
*/
    /*  ~ Memo. ~
        flagsをビットフィールドを用いた構造体で定義していた.
        ビットフィールドは"上から読まれる"か、"下から読まれるか"という環境依存が大きい.
    */
/* NLRI. */
struct nlri {
    struct network networks;
} __attribute__((__packed__));

/* Path Attrib. */
// パス属性の場合分けに使用.
struct pa_code {
    uint8_t flags;
    uint8_t code;
} __attribute__((__packed__));

struct pa_origin {
    uint8_t flags;
    uint8_t code;
    uint8_t len;
    uint8_t origin;
} __attribute__((__packed__));

struct as_path_segment {
    uint8_t sgmnt_type;
    uint8_t sgmnt_len;
    uint16_t sgmnt_value[64];   // AS2
    // sgmnt_value > 可変長対応可能に！
    // ASN: 65536 ~ 4294967295 > 23456
} __attribute__((__packed__));
struct pa_as_path {
    uint8_t flags;
    uint8_t code;
    uint16_t len;
    struct as_path_segment sgmnt;
} __attribute__((__packed__));

struct pa_next_hop {
    uint8_t flags;
    uint8_t code;
    uint8_t len;
    struct in_addr nexthop; 
} __attribute__((__packed__));

struct pa_multi_exit_disc {
    uint8_t flags;
    uint8_t code;
    uint8_t len; 
    uint32_t med;
} __attribute__((__packed__));




/* 
    >>>>    Table elements.    <<<< 
*/
// > https://www.infraexpert.com/study/bgpz12.html 
struct bgp_table_entry {
    struct in_addr addr;        // Network
    uint8_t mask;               // prefix_len
    struct in_addr nexthop;     // NextHop
    uint32_t metric;            // Med
    uint8_t path_sgmnt_len;     // Path Sgmnt Len
    uint16_t path_sgmnt[64];    // Value > [0]: 1 [1]: 2 ...
    bool check;                 // F: Still. T: Already set the route.
    struct bgp_table_entry *next;
} __attribute__((__packed__));

struct List {
    struct bgp_table_entry *head;
    struct bgp_table_entry *tail;
};


#endif



