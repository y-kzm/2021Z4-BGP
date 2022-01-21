#ifndef INCLUDES_H

struct bgp_hdr {
  uint8_t marker[16];  
  uint16_t len;
  uint8_t type;
};
#define BGP_HDR_LEN 19

struct bgp_open {
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

#endif

