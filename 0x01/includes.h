#ifndef INCLUDES_H

#define FLAG_SERVER 1 // 0001 -s  
#define FLAG_CLIENT 2 // 0010 -c

#include <sys/types.h>   
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFSIZE 512
#define QUELIM 5
  
void server_side();
void client_side();

#endif
