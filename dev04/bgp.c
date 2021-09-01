/*
 * bgp.c
 * 2021/08/21 ~
 * Yokoo Kazuma
*/

#include <stdio.h>
#include <string.h>     // memset() ...
#include "param.h"
#include "bgp.h"
#include "includes.h"

/*---------- prosecc_connect_sendopen ----------*/
void process_sendopen(int soc, struct peer *p, struct config cfg)
{
    struct bgp_open bo;
    int i, size;
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

    memcpy(buf, &bo, BGP_OPEN_LEN);

    /* Send packets */
    fprintf(stdout, "--------------------\n");
    fprintf(stdout, "Sending OPEN MSG...\n\n"); 
  
    size = write(soc, buf, BGP_OPEN_LEN);

    /* State transition. */
    p->state = OPENSENT_STATE;
}

/*---------- prosecc_opensent_recvopen ----------*/
void process_recvopen(int soc)
{
    struct bgp_open_opt  *bop;
    unsigned char *ptr;
    unsigned char buf[BUFSIZE];
    int i;

    /* Recv packets. */
    fprintf(stdout, "--------------------\n");    
    read(soc, buf, BGP_OPEN_OPT_LEN);  
    
    ptr = buf;

    bop = (struct bgp_open_opt *)ptr;
    ptr += BGP_OPEN_OPT_LEN;
    fprintf(stdout, "BGP OPEN MSG RECV...\n");
    fprintf(stdout, "Marker: ");
    for(i=0; i<MARKER_LEN; i++){
        fprintf(stdout, " %x", bop->hdr.marker[i]);
    } fprintf(stdout, " \n");
    fprintf(stdout, "Len: %u\n", ntohs(bop->hdr.len));
    fprintf(stdout, "Type: %u\n", bop->hdr.type);  

    // fprintf(stdout, "\n");
    fprintf(stdout, "Version: %u\n", bop->version);
    fprintf(stdout, "MyAS: %u\n", ntohs(bop->myas));
    fprintf(stdout, "HoldTime: %u\n", ntohs(bop->holdtime));
    fprintf(stdout, "Id: %s\n", inet_ntoa(bop->id));
    fprintf(stdout, "Opt_Len: %u\n\n", bop->opt_len);
}

/*---------- prosecc_opensent_sendkeep ----------*/
void process_sendkeep(int soc, struct peer *p)
{
    struct bgp_hdr keep;
    int i;
    unsigned char buf[BUFSIZE];

    // Set header.
    for(i=0; i<MARKER_LEN; i++){
        keep.marker[i] = 255;
    }
    keep.len = htons(BGP_HDR_LEN);
    keep.type = KEEPALIVE_MSG;
    
    memcpy(buf, &keep, BGP_HDR_LEN);

    /* Send packets */
    fprintf(stdout, "--------------------\n");
    fprintf(stdout, "Sending KEEPALIVE MSG...\n\n"); 
  
    write(soc, buf, BGP_HDR_LEN);

    p->state = OPENCONFIRM_STATE;
}

/*---------- prosecc_openfirm_recvkeep ----------*/
void prosecc_recvkeep(int soc, struct peer *p)
{
    struct bgp_hdr     *keep;
    unsigned char *ptr;
    unsigned char buf[BUFSIZE];
    int i;

    /* Recv packets. */
    fprintf(stdout, "--------------------\n");    
    read(soc, buf, BGP_HDR_LEN);  
    
    ptr = buf;

    keep = (struct bgp_hdr *)ptr;
    ptr += BGP_HDR_LEN;
    fprintf(stdout, "BGP KEEPALIVE MSG RECV...\n");
    fprintf(stdout, "Marker: ");
    for(i=0; i<MARKER_LEN; i++){
        fprintf(stdout, " %x", keep->marker[i]);
    } fprintf(stdout, " \n");
    fprintf(stdout, "Len: %u\n", ntohs(keep->len));
    fprintf(stdout, "Type: %u\n", keep->type);  

    p->state = ESTABLISHED_STATE; 
}
