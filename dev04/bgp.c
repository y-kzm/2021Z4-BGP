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

/*
    >>>>    SEND or RECV MSG.    <<<<
*/
/*---------- process_sendopen ----------*/
void process_sendopen(int soc, struct peer *p, struct config cfg)
{
    struct bgp_open bo;
    int i, size;
    unsigned char buf[BUFSIZE];

    /* Set header. */
    for(i=0; i<MARKER_LEN; i++){
        bo.hdr.marker[i] = 255;
    }
    bo.hdr.len = htons(BGP_OPEN_LEN);
    bo.hdr.type = OPEN_MSG;
    
    /* Set open message. */
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

/*---------- process_recvopen ----------*/
void process_recvopen(int soc)
{
    struct bgp_open_opt  *bop;
    unsigned char *ptr;
    unsigned char buf[BUFSIZE];
    int i;

    /* Recv packets. */
    fprintf(stdout, "--------------------\n");    
    read(soc, buf, BGP_OPEN_OPT_TOTAL_LEN);  
    
    ptr = buf;

    bop = (struct bgp_open_opt *)ptr;
    ptr += BGP_OPEN_OPT_TOTAL_LEN;
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

/*---------- process_sendkeep ----------*/
void process_sendkeep(int soc, struct peer *p)
{
    struct bgp_hdr keep;
    int i;
    unsigned char buf[BUFSIZE];

    /* Set header. */
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

/*---------- process_recvkeep ----------*/
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

/*---------- process_sendupdate ----------*/
void process_sendupdate(int soc)
{
    struct bgp_update *bu;
    // Path Attrib.
    struct pa_origin origin;
    struct pa_as_path as_path;
    struct pa_next_hop next_hop;
    struct pa_multi_exit_disc med;
    uint8_t *ptr; 
    int i;
    unsigned char buf[BUFSIZE];

    /* Set header. */
    for(i=0; i<MARKER_LEN; i++){
        bu.hdr.marker[i] = 255;
    }
    // bu.hdr.len = htons(BGP_HDR_LEN);
    bu.hdr.type = UPDATE_MSG;

    ptr = bu->data;

    /* Set update msg. */
    bu.withdrawn_routes_len = 0;        // とりあえず固定
    // origin.
    store_origin(&origin);
    memcpy(ptr, &origin, sizeof(origin));
    ptr += sizeof(origin);
    // as path.
    store_as_path(&as_path);
    memcpy(ptr, &as_path, sizeof(as_path));
    ptr += sizeof(as_path);
    // next hop.
    store_next_hop(&next_hop);
    memcpy(ptr, &next_hop, sizeof(next_hop));
    ptr += sizeof(next_hop);
    // med.
    store_next_hop(&med);
    memcpy(ptr, &med, sizeof(med));
    ptr += sizeof(med);
    // Total Path Attrib Len.
    

    // NLRI.



    // memcpy(buf, &keep, BGP_HDR_LEN);
    /* Send packets */
    fprintf(stdout, "--------------------\n");
    fprintf(stdout, "Sending UPDATE MSG...\n\n"); 
  
    write(soc, buf, );
}

/*
    >>>>    SET PATH ATTRIB.    <<<<
*/

/*
    >>>>    SET MSG.    <<<<
*/

/*
    >>>>    PRINT MSG.    <<<<
*/


/*
    >>>>    STATE PROCESS.    <<<<
*/
/*---------- process_established ----------*/
void process_established(int soc, struct peer *p)
{
    struct bgp_hdr     *hdr;
    unsigned char *ptr;
    unsigned char buf[BUFSIZE];
    int i;

    /* Recv packets. */
    fprintf(stdout, "--------------------\n");    
    read(soc, buf, BGP_HDR_LEN); 

    ptr = buf;
    hdr = (struct bgp_hdr *)ptr;

    /* Separate cases by msg type. */
    switch(hdr->type) {
        case KEEPALIVE_MSG: 
            fprintf(stdout, "BGP KEEPALIVE MSG RECV...\n");
            // keep を受け取る関数(debug表示) > 既存のはreadからあるから使いづらい?
            // recv, send関数のset部やprint部を別の関数にする？
            process_sendkeep(soc, p);
            break;
        case UPDATE_MSG: 
            // update を受け取る関数(debug表示)
            process_sendupdate();
            break;
        case NOTIFICATION_MSG:
            printf("unimplemented\n");
            exit(EXIT_FAILURE);
    }
}
