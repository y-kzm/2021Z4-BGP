#ifndef INCLUDES_H

/* Include. */
#include <sys/types.h>   
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/* Define. */
#define BUFSIZE    4096
#define BACK_LOG   5

/* Mode. */
// Unimplemented.
/*
enum MODE {
    SERVER,     // 0
    CLIENT      // 1
};
*/

// extern.
extern int soc;

/* Function */     
void usage();
void state_transition();        // Process by state.
void tcp_connect();             // Establish tcp connection.

#endif