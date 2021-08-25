/*
 * bgp.c
 * 2021/08/21 ~
 * Yokoo Kazuma
*/

#include <stdio.h>
#include <unistd.h>     // getopt() ...
#include <string.h>     // memset() ...
#include <stdlib.h>     // exit() ...
#include "param.h"
#include "bgp.h"
#include "includes.h"

void process_connect()
{
    struct bgp_open bo;
    int i;
    unsigned char buf[BUFSIZE];

    // Set header.
    for(i=0; i<MARKER_LEN; i++){
        bo.hdr.marker[i] = 255;
    }
    bo.hdr.len = htons(BGP_OPEN_LEN);
    bo.hdr.type = OPEN_MSG;
    
    // Set open message.
    bo.version = 4;
    bo.myas = htons(cfg.my_as);
    bo.holdtime = htons(180);
    bo.id.s_addr = cfg.router_id.s_addr;
    bo.opt_len = 0;

    //ptr = buf;
    memcpy(buf, &bo, BGP_OPEN_LEN);

    /* Send packets */
    fprintf(stdout, "--------------------\n");
    fprintf(stdout, "Sending Open Msg...\n\n"); 
  
    write(soc, buf, BGP_OPEN_LEN);

    /* State transition. */
    state = OPENSENT_STATE;
}

void process_opensent()
{
    struct bgp_open     *bo;
    unsigned char *ptr;
    unsigned char buf[BUFSIZE];
    int i;

    /* Recv packets. */
    fprintf(stdout, "--------------------\n");    
    read(soc, buf, BGP_OPEN_LEN);  
    
    ptr = buf;

    bo = (struct bgp_open *)ptr;
    ptr += BGP_OPEN_LEN;
    fprintf(stdout, "-----BGP HEADER RECV-----\n");
    fprintf(stdout, "Marker: ");
    for(i=0; i<MARKER_LEN; i++){
        fprintf(stdout, " %x", bo->hdr.marker[i]);
    } fprintf(stdout, " \n");
    fprintf(stdout, "Len: %u\n", ntohs(bo->hdr.len));
    fprintf(stdout, "Type: %u\n", bo->hdr.type);  

    fprintf(stdout, "-----OPEN MSG RECV-----\n");
    fprintf(stdout, "Version: %u\n", bo->version);
    fprintf(stdout, "MyAS: %u\n", ntohs(bo->myas));
    fprintf(stdout, "HoldTime: %u\n", ntohs(bo->holdtime));
    fprintf(stdout, "Id: %s\n", inet_ntoa(bo->id));
    fprintf(stdout, "Opt_Len: %u\n\n", bo->opt_len);

    state = OPENCONFIRM_STATE;
}