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
    >>>>    SEND RECV MSG.    <<<<
*/
/*---------- process_sendopen ----------*/
void process_sendopen(int soc, struct peer *p, struct config *cfg)
{
    struct bgp_open bo;
    unsigned char buf[BUFSIZE];

    /* Store Open Msg. */
    store_open(&bo, cfg);
    memcpy(buf, &bo, BGP_OPEN_LEN);

    /* Send packets */
    fprintf(stdout, "--------------------\n");
    fprintf(stdout, "Sending OPEN MSG...\n\n"); 
    write(soc, buf, BGP_OPEN_LEN);

    /* State transition. */
    p->state = OPENSENT_STATE;
}

/*---------- process_recvopen ----------*/
void process_recvopen(int soc)
{
    unsigned char buf[BUFSIZE];

    /* Recv packets. */
    fprintf(stdout, "--------------------\n");    
    read(soc, buf, BGP_OPEN_OPT_TOTAL_LEN);  
    fprintf(stdout, "BGP OPEN MSG RECV...\n");

    print_open(buf);
}

/*---------- process_sendkeep ----------*/
void process_sendkeep(int soc, struct peer *p)
{
    struct bgp_hdr keep;
    unsigned char buf[BUFSIZE];

    /* Store KeepAlive Msg.*/
    store_keep(&keep);
    memcpy(buf, &keep, BGP_HDR_LEN);

    /* Send packets */
    fprintf(stdout, "--------------------\n");
    fprintf(stdout, "Sending KEEPALIVE MSG...\n\n"); 
    write(soc, buf, BGP_HDR_LEN);

    /* State transition. */
    p->state = OPENCONFIRM_STATE;
}

/*---------- process_recvkeep ----------*/
void process_recvkeep(int soc, struct peer *p)
{
    unsigned char buf[BUFSIZE];

    /* Recv packets. */
    fprintf(stdout, "--------------------\n");    
    read(soc, buf, BGP_HDR_LEN);  
    fprintf(stdout, "BGP KEEPALIVE MSG RECV...\n");

    print_keep(buf);

    p->state = ESTABLISHED_STATE; 
}

/*---------- process_sendupdate ----------*/
void process_sendupdate(int soc)
{
    struct bgp_update bu;
    // Path Attrib.
    struct pa_origin origin;
    struct pa_as_path as_path;
    struct pa_next_hop next_hop;
    struct pa_multi_exit_disc med;
    struct nlri nlri;
    uint8_t *ptr; 
    int i;
    unsigned char buf[BUFSIZE];

    /* Set header. */
    for(i=0; i<MARKER_LEN; i++){
        bu.hdr.marker[i] = 255;
    }
    bu.hdr.len = htons(55);    // とりあえず固定 
    bu.hdr.type = UPDATE_MSG;
    /* Set update msg. */
    bu.withdrawn_routes_len = 0;        // とりあえず固定
    // Total Path Attrib Len.       
    bu.total_pa_len = htons(28);     // とりあえず固定

    ptr = bu.contents;

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
    store_med(&med);
    memcpy(ptr, &med, sizeof(med));
    ptr += sizeof(med);
    
    // NLRI.
    store_nlri(&nlri);
    memcpy(ptr, &nlri, sizeof(nlri));
    ptr += sizeof(uint32_t);

    memcpy(buf, &bu, 55);
    /* Send packets */
    fprintf(stdout, "--------------------\n");
    fprintf(stdout, "Sending UPDATE MSG...\n\n"); 
    write(soc, buf, 55);     // 55: とりあえず固定
}

/*
    >>>>    STORE PATH ATTRIB.    <<<<
*/
/*---------- origin ----------*/
void store_origin(struct pa_origin *origin)
{
    // Flags: 0100 0000
    origin->flags = 0x40;
    // Code: ORIGIN (1)
    origin->code = ORIGIN;
    // Length
    origin->len = 1;
    // Origin: INCOMPLETE (2)
    origin->origin = ORIGIN_INCOMPLETE;
}

/*---------- as_path ----------*/
void store_as_path(struct pa_as_path *as_path)
{
    // Flags: 0101 0000
    as_path->flags = 0x50;
    // Code: AS_PATH (2)
    as_path->code = AS_PATH;
    // Length
    as_path->len = htons(6); 
    // Segment:  
    as_path->sgmnt.sgmnt_type = AS_SEQUENCE;
    as_path->sgmnt.sgmnt_len = 1;
    as_path->sgmnt.sgmnt_value = htonl(1); 
}

/*---------- next_hop ----------*/
void store_next_hop(struct pa_next_hop *next_hop)
{
    // Flags: 0100 0000
    next_hop->flags = 0x40;
    // Code: NEXT_HOP (3)
    next_hop->code = NEXT_HOP;
    // Length
    next_hop->len = 4;
    // NextHop: 10.255.1.1
    next_hop->nexthop.s_addr = inet_addr("10.255.1.1");     // cfgから
}

/*---------- med ----------*/
void store_med(struct pa_multi_exit_disc *med)
{
    // Flags: 1000 0000
    med->flags = 0x80;
    // Code: MULTI_EXIT_DISC (4)
    med->code = MULTI_EXIT_DISC;
    // Length
    med->len = 4;
    // Med
    med->med = 0;
}

/*---------- nlri ----------*/
void store_nlri(struct nlri *nlri)
{
    u_int32_t addr = inet_addr("10.1.0.0");
    int len = 24;

    uint8_t *block;
    block = &addr;
    int i;
    nlri->prefix_len = len;
    for(i = 0; i < (len + (BYTE_SIZE - 1) / BYTE_SIZE); i++ ) {
        nlri->prefix[i] = *block;
        block += sizeof(uint8_t);
    }
}

/*
    >>>>    STORE MSG.    <<<<
*/
/*---------- Open Msg.----------*/
void store_open(struct bgp_open *bo, struct config *cfg)
{
    int i;

    /* Set header. */
    for(i=0; i<MARKER_LEN; i++){
        bo->hdr.marker[i] = 255;
    }
    bo->hdr.len = htons(BGP_OPEN_LEN);
    bo->hdr.type = OPEN_MSG;
    
    /* Set open message. */
    bo->version = 4;
    bo->myas = htons(cfg->my_as);
    bo->holdtime = htons(180);
    bo->id.s_addr = cfg->router_id.s_addr;
    bo->opt_len = 0;
}

/*---------- KeppAlive Msg.----------*/
void store_keep(struct bgp_hdr *keep)
{
    int i;

    /* Set header. */
    for(i=0; i<MARKER_LEN; i++){
        keep->marker[i] = 255;
    }
    keep->len = htons(BGP_HDR_LEN);
    keep->type = KEEPALIVE_MSG;
}

/*---------- Update Msg.----------*/


/*
    >>>>    STATE PROCESS.    <<<<
*/
/*---------- process_established ----------*/
void process_established(int soc, struct peer *p)
{
    struct bgp_hdr     *hdr;
    unsigned char *ptr;
    unsigned char buf[BUFSIZE];

    /* Recv packets. */
    fprintf(stdout, "--------------------\n");    
    read(soc, buf, BGP_HDR_LEN); 

    ptr = buf;
    hdr = (struct bgp_hdr *)ptr;

    /* Separate cases by msg type. */
    switch(hdr->type) {
        case KEEPALIVE_MSG: 
            fprintf(stdout, "BGP KEEPALIVE MSG RECV...\n");
            print_keep(buf);
            process_sendkeep(soc, p);
            break;
        case UPDATE_MSG: 
            fprintf(stdout, "BGP UPDATE MSG RECV...\n");
            print_update(buf);
            process_sendupdate(soc);
            break;
        case NOTIFICATION_MSG:
            printf("unimplemented\n");
            exit(EXIT_FAILURE);
    }
}
