#ifndef BGP_H

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

/*BGP Peer. */
struct peer {
    enum STATE state;
};





/* BGP Header Format. */
struct bgp_hdr {
    uint8_t marker[16];  
    uint16_t len;
    uint8_t type;
} __attribute__((__packed__));
#define MARKER_LEN  16
#define BGP_HDR_LEN 19

/* BGP Open Msg Format. */
struct bgp_open {
    struct bgp_hdr  hdr; 
    uint8_t version;
    uint16_t myas;
    uint16_t holdtime;
    struct in_addr id;
    uint8_t opt_len;
} __attribute__((__packed__));
#define BGP_OPEN_LEN 29

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
#define BGP_OPEN_OPT_LEN 46
#define BGP_OPEN_OPT_TOTAL_LEN 75

/* BGP Update Msg Format. */
// typedef uint8_t msg[64];
struct bgp_update {
    struct bgp_hdr  hdr;
    uint16_t withdrawn_routes_len;
    // truct nlri withdrawn_routes[32];   // sizeどうするの?
    uint16_t total_pa_len;
    // uint8_t pa[64];     // size
    // struct nlri nlri[32];   // size
    uint8_t contents[64];
} __attribute__((__packed__));





/* Path Atribute internal structure. */
// ビットフィールドは、上から読まれるか、下から読めれるかという環境依存が大きい
// > ビット演算を使う方が良い
/*
struct flags {
    uint8_t opt: 1;
    uint8_t trans: 1;
    uint8_t part: 1;
    uint8_t ext_len: 1;
    uint8_t unused: 4;
};
*/

struct as_path_segment {
    uint8_t sgmnt_type;
    uint8_t sgmnt_len;
    uint32_t sgmnt_value;
} __attribute__((__packed__));

/* NLRI. */
struct nlri {
  uint8_t prefix_len;
  uint8_t prefix[4];
} __attribute__((__packed__));





/* Path Atribute. */
struct pa_origin {
    uint8_t flags;
    uint8_t code;
    uint8_t len;
    uint8_t origin;
} __attribute__((__packed__));
#define ORIGIN_IGP          0;
#define OROGON_EGP          1;
#define ORIGIN_INCOMPLETE   2;

struct pa_as_path {
    uint8_t flags;
    uint8_t code;
    uint16_t len;
    struct as_path_segment sgmnt;
} __attribute__((__packed__));
#define AS_SET              1;
#define AS_SEQUENCE         2;
#define AS_CONFED_SEQUENCE  3;
#define AS_CONFED_SET       4;

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






/* Functon. */
void process_sendopen(int soc, struct peer *p, struct config *cfg);
void process_recvopen(int soc);
void process_sendkeep(int soc, struct peer *p);
void process_recvkeep(int soc, struct peer *p);
void process_sendupdate(int soc);
void process_established(int soc, struct peer *p);

void store_origin(struct pa_origin *origin);
void store_as_path(struct pa_as_path *as_path);
void store_next_hop(struct pa_next_hop *next_hop);
void store_med(struct pa_multi_exit_disc *med);
void store_nlri(struct nlri *nlri);


#endif



