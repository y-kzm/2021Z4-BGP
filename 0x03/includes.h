#ifndef INCLUDES_H

/* Include. */
#include <sys/types.h>   
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/* Define. */
#define BUFSIZE    4096

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
// Process by state.
void state_transition(struct peer *p, struct config cfg);   
// Establish tcp connection.
int tcp_connect(struct peer *p, struct config cfg);             

#endif