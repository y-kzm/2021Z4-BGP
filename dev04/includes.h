#ifndef INCLUDES_H

/* Include. */
#include <sys/types.h>   
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/* Define. */
#define BUFSIZE     4096
#define BYTE_SIZE   8

/* Mode. */
// Unimplemented.
/*
enum MODE {
    SERVER,     // 0
    CLIENT      // 1
};
*/


/* Function */     
void usage(); 
void state_transition(struct peer *p, struct config *cfg);   
int tcp_connect(struct peer *p, struct config *cfg);       

// Print.
void print_open(unsigned char *data);
void print_keep(unsigned char *data);
void print_update(unsigned char *data);

#endif