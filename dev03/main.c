/*
    > main.c
    > 2021/08/18
    > Yokoo Kazuma
*/

#include <stdio.h>
#include <unistd.h>     // getopt() ...
#include <string.h>     // memset() ...
#include <stdlib.h>     // exit() ...
#include "param.h"
#include "bgp.h"
#include "includes.h"

// param R1> , R2>
const struct param     param1 = {"1", "10.255.1.1", "2", "10.255.1.2"};
const struct param     param2 = {"2", "10.255.1.2", "1", "10.255.1.1"};

// state, mode, socket
unsigned int state = IDLE_STATE;
unsigned int mode;
int soc;

/*---------- main ----------*/
int main(int argc, char *argv[])
{
    int opt;    // getopt()

    if(argc != 2){
        fprintf(stderr, "Too many arguments.\n");
        return -1;
    }

    /* Options */
    opterr = 0;
    while((opt = getopt(argc, argv, "sch")) != -1){
        switch(opt){
            case 's':         // server side.
                server();
                break; 
            case 'c':         // client side.
                client();
                break;
            case 'h':         // help.       
                fprintf(stdout, "Usage: ./mybgp [-hsc]\n");
                fprintf(stdout, "s: Server Side.\nc: Client Side.\nh: HELP.\n");
                return 0;
            default:
                fprintf(stderr, "Option Error.\n");
                return -1;
        } 
    }

    return 0;
}



/*---------- server ----------*/
void server()
{
    int i = 0;
    mode = SERVER_MODE;
    while(i!=3){
        state_transition();
        i++;
    }
}



/*---------- client ----------*/
void client()
{
    int i = 0;
    mode = CLIENT_MODE;
    while(i!=3){
        state_transition();
        i++;
    }
}



/*---------- state_transition ----------*/
void state_transition()
{
    switch(state){
        case IDLE_STATE:
            tcp_connect();
            break;
        case CONNECT_STATE:
            if(mode == SERVER_MODE)
                sending_open(param1);
            else if(mode == CLIENT_MODE)
                sending_open(param2); 
            break;
        case OPENSENT_STATE:
            waiting_open();
            break;
        default:
            fprintf(stderr, "State Error.\n");
            exit(EXIT_FAILURE);
    }
}



/*---------- tcp_connect ----------*/
void tcp_connect() 
{
    unsigned short sPort = 179;     // Port Num
    // int srcSoc;                     // Src Socketfd
    // int dstSoc;                     // Dst Socketfd
    struct sockaddr_in sa;          // Src param
    struct sockaddr_in da;          // Dst param

    // Server side.
    if(mode == SERVER_MODE){
        /* Create a socket */
        if((soc = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            perror("socket() failed.");
            exit(EXIT_FAILURE);
        }

        /* Storage of src param */
        memset(&sa, 0, sizeof(sa));
        sa.sin_family       = AF_INET;
        sa.sin_addr.s_addr  = htonl(INADDR_ANY);  // Any address(0.0.0.0)
        sa.sin_port         = htons(sPort);

        /* Bind src param to the socket. */
        if(bind(soc, (struct sockaddr *) &sa, sizeof(sa)) < 0){
            perror("bind() failed.");
            exit(EXIT_FAILURE);
        }

        /* Waiting for connection. */
        if(listen(soc, BACK_LOG) < 0){
            perror("listen() failed.");
            exit(EXIT_FAILURE);
        }
        fprintf(stdout, "Waiting for connection ...\n");

        /* Accepting the connection. */
        // if((soc = accept(soc, (struct sockaddr *) &da, sizeof(da))) < 0){
        if((soc = accept(soc, NULL, NULL)) < 0){
            perror("accept() failed.");
            exit(EXIT_FAILURE);
        }
        fprintf(stdout, "Connected from %s\n", inet_ntoa(da.sin_addr));

        /* State transition. */
        state = OPENSENT_STATE;

    // Client side.
    } else if(mode == CLIENT_MODE){
        /* Create a socket */
        if((soc = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            perror("socket() failed.");
            exit(EXIT_FAILURE);
        } 

        /* Storage of dst param. */
        memset(&da, 0, sizeof(sa));
        da.sin_family       = AF_INET;
        da.sin_addr.s_addr  = inet_addr(param2.Neighbor);  
        da.sin_port         = htons(sPort);

        /* Connect. */
        fprintf(stdout, "Trying to connect to %s \n", param2.Neighbor); 
        connect(soc, (struct sockaddr *) &da, sizeof(da));

        /* State transition. */
        state = CONNECT_STATE;

    } else 
        exit(EXIT_FAILURE);
}