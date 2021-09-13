#ifndef PARAM_H

/* Includes. */
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <jansson.h>
#include <arpa/inet.h>

/* Deinfe. */
#define MAX_NEIGH   32
#define MAX_NETWORK 256

/* Config. */
// router bgp 1
//  bgp router-id 1.1.1.1
//  neighbor 10.255.1.2 remote-as 2
//  address-family ipv4 unicast
struct neighbor {
    struct in_addr addr;
    uint16_t remote_as;
};
struct prefix {
    struct in_addr addr;
    uint8_t len;
};
struct network {
    struct prefix prefix;
};

struct config {
    uint16_t my_as;
    struct in_addr router_id;
    uint8_t neighbors_num;
    struct neighbor neighbors[MAX_NEIGH];
    uint8_t networks_num;
    struct network networks[MAX_NETWORK];
};

/* Function. */
struct config
*parse_json(const char *buf, size_t buflen);

#endif