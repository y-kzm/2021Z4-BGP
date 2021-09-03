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
    uint8_t withdrawn_routes_len;
    uint8_t data[64];
} __attribute__((__packed__));
#define BGP_UPDATE_MINLEN 4


/* Path Atribute internal structure. */
struct flags {
    uint8_t opt: 1;
    uint8_t trans: 1;
    uint8_t part: 1;
    uint8_t ext_len: 1;
    uint8_t unused: 4;
};

struct as_path_segment {
    uint8_t sgmnt_type;
    uint8_t sgmnt_len;
    uint32_t sgmnt_value;
} __attribute__((__packed__));

/* Path Atribute. */
struct pa_origin {
    struct flags flags;
    uint8_t code;
    uint8_t len;
    uint8_t origin;
} __attribute__((__packed__));

struct pa_as_path {
    struct flags flags;
    uint8_t code;
    uint16_t len;
    struct as_path_segment sgmnt;
} __attribute__((__packed__));

struct pa_next_hop {
    struct flags flags;
    uint8_t code;
    uint8_t len;
    struct in_addr nexthop; 
} __attribute__((__packed__));

struct pa_mult_exit_disc {
    struct flags flags;
    uint8_t code;
    uint8_t len; 
    uint32_t med;
} __attribute__((__packed__));

/* NLRI. */
struct nlri {
  uint8_t prefix_len;
  struct in_addr prefix;
} __attribute__((__packed__));

/* Functon. */
void process_sendopen(int soc, struct peer *p, struct config cfg);
void process_recvopen(int soc);
void process_sendkeep(int soc, struct peer *p);
void prosecc_recvkeep(int soc, struct peer *p);

#endif



