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
    unsigned char buf[BUFSIZE];

    store_update(&bu);      // lenは固定
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
    med->med = htonl(0);
}

/*---------- nlri ----------*/
void store_nlri(struct nlri *nlri)
{
    u_int32_t addr = inet_addr("10.1.0.0");
    int len = 24;

    uint8_t *block;
    block = (uint8_t *)&addr;
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
void store_update(struct bgp_update *bu)
{
    // Path Attrib.
    struct pa_origin origin;
    struct pa_as_path as_path;
    struct pa_next_hop next_hop;
    struct pa_multi_exit_disc med;
    struct nlri nlri;

    // Update Msg Members.
    struct withdrawn_routes *wr;  
    struct total_path_attrib *tp;
    
    // Var.
    unsigned char *ptr;                 // Used to store the path attribute.
    unsigned char *data = bu->contents; // Used to store sent data.
    int i;

    /* Set header. */
    for(i=0; i<MARKER_LEN; i++){
        bu->hdr.marker[i] = 255;
    }
    bu->hdr.len = htons(55);
    bu->hdr.type = UPDATE_MSG;


    /* Set Update. */
    /* withdrawn_routes. */
    // Dynamic range.
    wr = (struct withdrawn_routes *)malloc(
        sizeof(struct withdrawn_routes) + sizeof(uint8_t) * 0);
    wr->len = htons(0);
    // もし到達不能経路があるならここに処理を追加.

    // withdrawn_routes_len(2byte) + withdrawn_routes(variable) 
    memcpy(data, wr, sizeof(uint8_t)*(2+0));    
    data += sizeof(uint8_t)*(2+0);


    /* total_path_attrib. */
    // Dynamic range.
    tp = (struct total_path_attrib *)malloc(
        sizeof(struct total_path_attrib) + sizeof(uint8_t) * 28);
    tp->total_len = htons(28);

    /* Set Path Attrib. */
    ptr = tp->path_attrib;
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
    
    // total_len(2byte) + total_len
    memcpy(data, tp, sizeof(uint8_t)*(2+28));
    data += sizeof(uint8_t)*(2+28);


    /* Set NLRI. */
    int nlri_len;
    // nlri_len = update_len - 23 - withdrawn_len - total_path_len
    // 23 = hdr(19byte) + wr_len(2byte) + tp_len(2byte)
    nlri_len = bu->hdr.len - 23 - 0 - 28;

    store_nlri(&nlri);
    memcpy(data, &nlri, sizeof(nlri) - sizeof(uint8_t)*1);
    data += sizeof(uint8_t)*nlri_len;

    /* Free. */
    free(wr);
    free(tp);
}

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
