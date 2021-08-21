#ifndef INCLUDES_H

#include <sys/types.h>   
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVER_MODE 1 // 0001 -s  
#define CLIENT_MODE 2 // 0010 -c

#define BUFSIZE 1024
#define BACK_LOG 5

extern int soc;
extern unsigned int state;
extern unsigned int mode;

void server();       // main > server() servモード -> state_transition()
void client();       // main > client() recvモード -> state_transition()

void state_transition();        // Process by state.
void tcp_connect();             // Establish tcp connection.

#endif