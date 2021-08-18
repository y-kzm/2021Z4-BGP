#ifndef BGP_H

// BGP Header Format
struct bgp_hdr {
  uint8_t marker[16];  
  uint16_t len;
  uint8_t type;
};
#define BGP_HDR_LEN 19

// BGP Open Msg Format
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

// BGP State
#define IDLE_STATE 1
#define CONNECT_STATE 2
#define OPNESENT_STATE 3
// Add morte state ...

// state_transition()
/*
- IDLE_STATE 
    > tcp_connect() socket準備とか 
    > clientはidleからopensentへ
    > serverはidleからconnectへ
- CONNECT_STATE 
    > sending_open() 
    > serverはopenmsgを送信してopesent状態へ
    > client    
- OPENSENT_STATE 
    > waiting_open()
    > openmsg待ち状態
    > clientはopenmsgを受け取った後、connect状態へ
    > server 
※ 呼ばれる関数は基本的にservモードとrecvモードで場合わけ
*/

void state_transition();            // Process by state.
int tcp_connect(int mode);          // Establish tcp connection.



#endif