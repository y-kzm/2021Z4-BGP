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


/*---------- main ----------*/
// ./mybgp conf.json
int main(int argc, char *argv[])
{
    // Variables for reading json file.
    int fd, size;
    char buf[4096];
    struct config  *cfg;

    // BGP peer variables.
    struct peer p = {IDLE_STATE};

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
    
    cfg = parse_json((const char *)buf, size);
    
    // Debug.
    printf("--------------------\n");
    printf("Loaded the following settings.\n");
    // printf("Currently limited to one neighbor...\n");
    printf("> myas  : %d\n", cfg->my_as);
    printf("> id    : %s\n", inet_ntoa(cfg->router_id));
    printf("> neighbor_address: %s\n", inet_ntoa(cfg->neighbors[0].addr));
    printf("> remote_as       : %d\n", cfg->neighbors[0].remote_as);
    printf("> networks prefix:  %s/%d\n\n", inet_ntoa(cfg->networks[0].prefix.addr), cfg->networks[0].prefix.len);
    

    // State transition.
    while(1) {
       state_transition(&p, cfg); 
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
void state_transition(struct peer *p, struct config *cfg)
{
    static int soc;
    switch(p->state){
        case IDLE_STATE:
            // Start TCP Connection.       
            soc = tcp_connect(p, cfg);        
            break;
        case CONNECT_STATE:
            // If the TCP connection is successful.
            // Send Open Msg.
            process_sendopen(soc, p, cfg);
            break;
        case OPENSENT_STATE:
            // Waiting Open Msg.
            process_recvopen(soc);
            // If the OPEN message is successful.
            // Send KEEPALIVE Msg.
            process_sendkeep(soc, p);
            break;
        case OPENCONFIRM_STATE:
            // Waiting KEEPALIVE Msg.
            process_recvkeep(soc, p);
            break;
        case ESTABLISHED_STATE:
            process_established(soc, p, cfg);
            printf("Fin.\n");
            exit(1);
            break;
        default:
            fprintf(stderr, "State Error.\n");
            exit(EXIT_FAILURE);
    }
}


/*---------- tcp_connect ----------*/
int tcp_connect(struct peer *p, struct config *cfg) 
{
    unsigned short sPort = 179;     // Port Num
    struct sockaddr_in da;          // Dst param
    int soc;

    // Client side.
    /* Create a socket */
    if((soc = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket() failed");
        exit(EXIT_FAILURE);
    } 

    /* Storage of dst param. */
    memset(&da, 0, sizeof(da));
    da.sin_family       = AF_INET;
    da.sin_addr.s_addr  = cfg->neighbors[0].addr.s_addr;  
    da.sin_port         = htons(sPort);

    /* Connect. */
    fprintf(stdout, "--------------------\n");   
    fprintf(stdout, "Trying to connect to %s \n\n", inet_ntoa(cfg->neighbors[0].addr)); 
    if(connect(soc, (struct sockaddr *) &da, sizeof(da)) < 0) {
        perror("connect() failed");
        exit(EXIT_FAILURE);
    }

    /* State transition. */
    p->state = CONNECT_STATE;

    return soc;
}