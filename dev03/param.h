#ifndef PARAM_H

/* Include. */
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <jansson.h>
#include <arpa/inet.h>

/* Config. */
// router bgp 1
//  bgp router-id 10.255.1.1
//  neighbor 10.255.1.2 remote-as 2
struct neighbor {
    struct in_addr  addr;
    uint16_t        remote_as;
};

struct config {
    uint16_t            my_as;
    struct in_addr      router_id;
    struct neighbor     ne;
};


// Extern.
extern struct config    cfg;

// Function.
struct config
parse_json(const char *buf, size_t buflen);

#endif