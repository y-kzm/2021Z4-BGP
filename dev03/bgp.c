/*
    > bgp.c
    > 2021/08/21
    > Yokoo Kazuma
*/

#include <stdio.h>
#include <unistd.h>     // getopt() ...
#include <string.h>     // memset() ...
#include <stdlib.h>     // exit() ...
#include "param.h"
#include "bgp.h"
#include "includes.h"

void sending_open(struct param   p) // modeで場合わけ
{
    struct bgp_open bo;
    int i;
    unsigned char buf[BUFSIZE];

    for(i=0; i<MARKER_LEN; i++){
        bo.marker[i] = 255;
    }
    bo.len = htons(BGP_OPEN_LEN);
    bo.type = 1;            // Open
    
    bo.version = 4;
    bo.myas = htons(atoi(p.MyAS));
    bo.holdtime = htons(180);
    bo.id.s_addr = inet_addr(p.RouterId);
    bo.opt_len = 0;

    //ptr = buf;
    memcpy(buf, &bo, BGP_OPEN_LEN);

    /* パケットの送信 */
    fprintf(stdout, "--------------------\n");
    fprintf(stdout, "Sending Open Msg...\n\n"); 
  
    write(soc, buf, BGP_OPEN_LEN);

    /* State transition. */
    state = OPENSENT_STATE;
}

void waiting_open()
{
    struct bgp_open     *bo;
    unsigned char *ptr;
    int i;
    unsigned char buf[BUFSIZE];
    
    /* パケットの受信 */
    fprintf(stdout, "--------------------\n");    
    read(soc, buf, BGP_OPEN_LEN);  
    
    ptr = buf;

    bo = (struct bgp_open *)ptr;
    ptr += BGP_OPEN_LEN;
    fprintf(stdout, "-----BGP HEADER: Recv-----\n");
    fprintf(stdout, "Marker: ");
    for(i=0; i<MARKER_LEN; i++){
        fprintf(stdout, " %x", bo->marker[i]);
    } fprintf(stdout, " \n");
    fprintf(stdout, "Len: %u\n", ntohs(bo->len));
    fprintf(stdout, "Type: %u\n", bo->type);  

    fprintf(stdout, "-----OPEN MSG: Recv-----\n");
    fprintf(stdout, "Version: %u\n", bo->version);
    fprintf(stdout, "MyAS: %u\n", ntohs(bo->myas));
    fprintf(stdout, "HoldTime: %u\n", ntohs(bo->holdtime));
    fprintf(stdout, "Id: %s\n", inet_ntoa(bo->id));
    fprintf(stdout, "Opt_Len: %u\n", bo->opt_len);

    state = CONNECT_STATE;
}