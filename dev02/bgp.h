#ifndef INCLUDES_H

struct bgp_hdr {
  uint8_t marker[16];  
  uint16_t len;
  uint8_t type;
};

struct bgp_open {
  uint8_t version;
  uint16_t myas;
  uint16_t holdtime;
  uint32_t id;
  uint8_t opt_len;
};

#endif

