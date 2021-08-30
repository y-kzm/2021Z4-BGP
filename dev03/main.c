/*
 * main.c
 * 2021/08/18 ~
 * Yokoo Kazuma
 * Client Side.
*/

#include <stdio.h>
#include <stdlib.h>     // exit() ...
#include <unistd.h>
#include <string.h>
#include "param.h"
#include "bgp.h"
#include "includes.h"

// State
enum STATE state;
// Socket.
int soc;
// Config.
struct config   cfg;


/*---------- main ----------*/
// ./mybgp conf.json
int main(int argc, char *argv[])
{
    // Variables for reading json file.
    int fd, size;
    char buf[4096];

    if(argc != 2) {
        fprintf(stderr, "The arguments are different.\n");
        usage();
        return -1;
    }

    // Open json file.
    fd = open(argv[1], O_RDONLY);
    if(fd < 0) {
        printf("Error open().\n");
        return -1;
    }

    // Read json file.
    size = read(fd, buf, sizeof(buf));
    if(size < 0) {
        printf("Error read().\n");
        return -1;
    }
    close(fd);

    // Read config.
    cfg = parse_json((const char *)buf, size);
    // Debug.
    printf("Loaded the following settings.\n");
    printf("--------------------\n");
    printf("> myas  : %d\n", cfg.my_as);
    printf("> id    : %s\n", inet_ntoa(cfg.router_id));
    printf("> neighbor_address: %s\n", inet_ntoa(cfg.ne.addr));
    printf("> remote_as       : %d\n\n", cfg.ne.remote_as);

    // Init state.
    state = IDLE_STATE;
    while(1) {
       state_transition(); 
    }

    return 0;
}

/*---------- usage ----------*/
void usage()
{
    fprintf(stdout, "./mybgp [JSON FILE]\n");
    fprintf(stdout, "The json file describes the configuration.\n");
}



/*---------- state_transition ----------*/
void state_transition()
{
    switch(state){
        case IDLE_STATE:
            // TCP Connection.
            tcp_connect();                  // > CONNECT
            break;
        case CONNECT_STATE:
            // Send Open Msg.
            process_connect_sendopen();     // > OPENSENT
            break;
        case OPENSENT_STATE:
            process_opensent_recvopen();
            process_opensent_sendkeep();    // > OPENCOFIRM
            break;
        case OPENCONFIRM_STATE:
            prosecc_openconfirm_recvkeep(); // > ESTABLISHED
            break;
        case ESTABLISHED_STATE:
            printf("ESTAB\n");
            exit(1);
            // prosecc_established();
            // break;
        default:
            fprintf(stderr, "State Error.\n");
            exit(EXIT_FAILURE);
    }
}


/*---------- tcp_connect ----------*/
void tcp_connect() 
{
    unsigned short sPort = 179;     // Port Num
    struct sockaddr_in da;          // Dst param

    // Client side.
    /* Create a socket */
    if((soc = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket() failed.");
        exit(EXIT_FAILURE);
    } 

    /* Storage of dst param. */
    memset(&da, 0, sizeof(da));
    da.sin_family       = AF_INET;
    da.sin_addr.s_addr  = cfg.ne.addr.s_addr;  
    da.sin_port         = htons(sPort);

    /* Connect. */
    fprintf(stdout, "Trying to connect to %s \n\n", inet_ntoa(cfg.ne.addr)); 
    connect(soc, (struct sockaddr *) &da, sizeof(da));

    /* State transition. */
    state = CONNECT_STATE;
}