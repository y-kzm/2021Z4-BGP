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
#include "includes.h"

static struct param     p;

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


    state_transition(IDLE_STATE, SERVER_MODE);
}



/*---------- client ----------*/
void client()
{


    state_transition(IDLE_STATE, CLIENT_MODE);
}



/*---------- state_transition ----------*/
void state_transition(int state, int mode)
{
    switc(state){
        case IDLE_MODE:
            break;
        case CONNECT_STATE:
            break;
        case OPNESENT_STATE:
            break;
        default:
            fprintf(stderr, "State Error.\n");
            exit(EXIT_FAILURE);
    }
}



/*---------- tcp_connect ----------*/
int tcp_connect(int mode)   // 引数にIPアドレス？
{
    unsigned short sPort = 179;     // Port Num
    int srcSoc;                     // Src Socketfd
    int dstSoc;                     // Dst Socketfd
    struct sockaddr_in sa;          // Src param
    struct sockaddr_in da;          // Dst param

    if(mode == SERVER_MODE){
        /* Create a socket */
        if((srcSoc = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            perror("socket() failed.");
            return -1;
        }

        /* Storage of src param */
        memset(&sa, 0, sizeof(sa));
        sa.sin_family       = AF_INET;
        sa.sin_addr.s_addr  = htonl(INADDR_ANY);  // Any address(0.0.0.0)
        sa.sin_port         = htons(sPort);

        /* Bind src param to the socket. */
        if(bind(srcSoc, (struct sockaddr *) &sa, sizeof(sa)) < 0){
            perror("bind() failed.");
            return -1;
        }

        /* Waiting for connection. */
        if(listen(srcSoc, BACK_LOG) < 0){
            perror("listen() failed.");
            return -1;
        }
        fprintf(stdout, "Waiting for connection ...\n");

        /* Accepting the connection. */
        if((dstSoc = accept(srcSoc, (struct sockaddr *) &da, sizeof(da))) < 0){
            perror("accept() failed.");
            return -1;
        }
        fprintf(stdout, "Connected from %s\n", inet_ntoa(da.sin_addr));

    } else if(mode == CLIENT_MODE){
        /* Create a socket */
        if((dstSoc = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            perror("socket() failed.");
            return -1;
        } 

        /* Storage of dst param. */
        memset(&da, 0, sizeof(sa));
        da.sin_family       = AF_INET;
        da.sin_addr.s_addr  = inet_addr(DST);              // アドレス格納
        da.sin_port         = htons(sPort);

        /* Connect. */
        fprintf(stdout, "Trying to connect to %s: \n", DST);      // アドレス格納
        connect(dstSoc, (struct sockaddr *) &da, sizeof(da));

    } else 
        return -1;

    return 0;
}