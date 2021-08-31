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

/* GBP Peer. */
struct peer {
    enum STATE state;
};

/* BGP Header Format. */
struct bgp_hdr {
  uint8_t marker[16];  
  uint16_t len;
  uint8_t type;
};
#define MARKER_LEN  16
#define BGP_HDR_LEN 19

/* BGP Open Msg Format. */
struct bgp_open {
  // struct bgp_hdr  hdr;
  uint8_t marker[16];  
  uint16_t len;
  uint8_t type;
  
  uint8_t version;
  uint16_t myas;
  uint16_t holdtime;
  struct in_addr id;
  uint8_t opt_len;
};
#define BGP_OPEN_LEN 29

/* BGP Open Msg Opt Format. */
struct bgp_open_opt {
  // struct bgp_hdr  hdr;
  uint8_t marker[16];  
  uint16_t len;
  uint8_t type;
  
  uint8_t version;
  uint16_t myas;
  uint16_t holdtime;
  struct in_addr id;
  uint8_t opt_len;
  uint8_t opt[46];
};
#define BGP_OPEN_OPT_LEN 29+46




/* Functon. */
void process_sendopen(int soc, struct peer *p, struct config cfg);
void process_recvopen(int soc);
void process_sendkeep(int soc, struct peer *p);
void prosecc_recvkeep(int soc, struct peer *p);

#endif



