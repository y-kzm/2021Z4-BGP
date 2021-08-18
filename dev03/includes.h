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

int server();       // main > server() servモード -> state_transition()
int client();       // main > client() recvモード -> state_transition()


#endif