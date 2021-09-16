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
    printf("\x1b[36m");
    fprintf(stdout, "Sending OPEN MSG...\n\n"); 
    printf("\x1b[0m");
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
    printf("\x1b[35m");
    fprintf(stdout, "Recvd BGP OPEN MSG...\n");
    printf("\x1b[0m");

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
    printf("\x1b[36m");
    fprintf(stdout, "Sending KEEPALIVE MSG...\n\n"); 
    printf("\x1b[0m");
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
    printf("\x1b[35m");
    fprintf(stdout, "Recvd BGP KEEPALIVE MSG...\n");
    printf("\x1b[0m");

    print_keep(buf);

    p->state = ESTABLISHED_STATE; 
}

/*---------- process_sendupdate ----------*/
void process_sendupdate(int soc, struct config *cfg)
{
    struct bgp_update bu[cfg->networks_num];
    unsigned char buf[BUFSIZE];
    int i;

    for(i = 0; i < cfg->networks_num; i ++) {
        buf[0] = '\0';
        store_update(&bu[i], cfg, i); 
        memcpy(buf, &bu[i], ntohs(bu[i].hdr.len));

        /* Send packets */
        fprintf(stdout, "--------------------\n");
        printf("\x1b[36m");
        fprintf(stdout, "Sending UPDATE MSG [%d]...\n\n", i); 
        printf("\x1b[0m");
        write(soc, buf, ntohs(bu[i].hdr.len));
    }
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
// 可変長対応可能に！
void store_as_path(struct pa_as_path *as_path)
{
    // Flags: 0101 0000
    as_path->flags = 0x50;
    // Code: AS_PATH (2)
    as_path->code = AS_PATH;
    // Length
    as_path->len = htons(4);   // AS2 
    // Segment:  
    as_path->sgmnt.sgmnt_type = AS_SEQUENCE;
    as_path->sgmnt.sgmnt_len = 1;
    as_path->sgmnt.sgmnt_value= htons(1);  // AS2
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
    next_hop->nexthop.s_addr = inet_addr("10.255.1.1");     
    // > tcpピアで使われてる自身のIPアドレスを取得する？
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
void store_nlri(struct nlri_network *networks, struct config *cfg, int nlri_mode, int index)
{
    int i, j;
    uint8_t *block;
    uint32_t addr;
    uint8_t prefix_len;

    if(nlri_mode) {
        for (i = 0; i < cfg->networks_num; i ++) {
            addr = cfg->networks[i].prefix.addr.s_addr;
            prefix_len = cfg->networks[i].prefix.len;

            block = (uint8_t *)&addr; 
            networks[i].prefix_len = prefix_len;
            for(j = 0; j < ((prefix_len + (BYTE_SIZE - 1)) / BYTE_SIZE); j++ ) {
                networks[i].prefix[j] = *block;
                block += sizeof(uint8_t);
            }
        }
    } else {
        addr = cfg->networks[index].prefix.addr.s_addr;
        prefix_len = cfg->networks[index].prefix.len;

        block = (uint8_t *)&addr; 
        networks[0].prefix_len = prefix_len;
        for(j = 0; j < ((prefix_len + (BYTE_SIZE - 1)) / BYTE_SIZE); j++ ) {
            networks[0].prefix[j] = *block;
            block += sizeof(uint8_t);
        }
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
void store_update(struct bgp_update *bu, struct config *cfg, int index)
{
    // Path Attrib.
    struct pa_origin origin;
    struct pa_as_path as_path;
    struct pa_next_hop next_hop;
    struct pa_multi_exit_disc med;
    struct nlri_network networks[cfg->networks_num];

    // Update Msg Members.
    struct withdrawn_routes *wr;  
    struct total_path_attrib *tpa;
    uint8_t total_pa_len;
    
    // Var.
    unsigned char *ptr;                         // Used to store the path attribute.
    unsigned char *data = bu->contents;         // Used to store sent data.
    unsigned char *offset_whole = bu->contents; // Use to ask for hdr->len.
    unsigned char *offset;       // Used to find the length of each field.
    int i, nlri_mode;            // > 0: Announce the route one by once.
                                 // > 1: Multiple routes are announced at once.
    
    /* Set header. */
    for(i=0; i<MARKER_LEN; i++){
        bu->hdr.marker[i] = 255;
    }
    // bu->hdr.len > Find it at the end of this function.
    bu->hdr.type = UPDATE_MSG;


    /* Set Update. */
    /* withdrawn_routes. */
    // Dynamic range.
    wr = (struct withdrawn_routes *)malloc(
        sizeof(struct withdrawn_routes) + sizeof(uint8_t) * MAX_WITHDRAWN);
    wr->len = htons(0);
    // もし到達不能経路があるならここに処理を追加.

    // withdrawn_routes_len(2byte) + withdrawn_routes(variable) 
    memcpy(data, wr, sizeof(uint8_t)*(TWOBYTE_FIELD_SIZE+0));    
    data += sizeof(uint8_t)*(TWOBYTE_FIELD_SIZE+0);


    /* total_path_attrib. */
    // Dynamic range.
    tpa = (struct total_path_attrib *)malloc(
        sizeof(struct total_path_attrib) + sizeof(uint8_t) * MAX_PATH_ATTRIB);

    /* Set Path Attrib. */
    ptr = tpa->path_attrib;
    offset = tpa->path_attrib;
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

    total_pa_len = (ptr-offset) / sizeof(uint8_t);
    tpa->total_len = htons(total_pa_len);   

    // total_len(2byte) + total_pa_len
    memcpy(data, tpa, sizeof(uint8_t)*(total_pa_len + TWOBYTE_FIELD_SIZE));
    data += sizeof(uint8_t)*(total_pa_len + TWOBYTE_FIELD_SIZE);


    /* Set NLRI. */  
      /*--- 
          nlri_len = update_len - 23 - withdrawn_len - total_path_len
          23 = hdr(19byte) + wr_len(2byte) + tp_len(2byte)
          > nlri_len = bu->hdr.len - 23 - 0 - 28;
      ---*/
    // int nlri_len;  
    // > Remove comment outs as needed.
    int network_len;
    nlri_mode = 0;
    offset = data;

    if(nlri_mode) {
        store_nlri(networks, cfg, nlri_mode, index);
        for (i = 0; i < cfg->networks_num; i ++) {
            // +1 : nlri.prefix_len(1byte);
            network_len = ((cfg->networks[i].prefix.len + (BYTE_SIZE - 1)) / BYTE_SIZE) + 1;
            memcpy(data, &networks[i], sizeof(uint8_t) * network_len);
            data += sizeof(uint8_t) * network_len;
        }
        // nlri_len = (data-offset) / sizeof(uint8_t);
        // > Remove comment outs as needed.
    } else {
        store_nlri(networks, cfg, nlri_mode, index);
        network_len = ((cfg->networks[index].prefix.len + (BYTE_SIZE - 1)) / BYTE_SIZE) + 1;
        memcpy(data, &networks[0], sizeof(uint8_t) * network_len);
        data += sizeof(uint8_t) * network_len;
    }

    /* Find the length of the update msg.. */
    bu->hdr.len = htons((data-offset_whole) / sizeof(uint8_t) + BGP_HDR_LEN);

    /* Free. */
    free(wr);
    free(tpa);
}

/*
    >>>>    STATE PROCESS.    <<<<
*/
/*---------- process_established ----------*/
void process_established(int soc, struct peer *p, struct config *cfg)
{
    struct bgp_hdr     *hdr;
    unsigned char *ptr;
    unsigned char buf[BUFSIZE];
    static int update_flag;

    /* Send Packets. */
        /*--- 
            > 毎回送るのではない.
            > 変更分だけ送る.
            > とりあえず、初回だけ送信する仕様で.
        ---*/
    if(update_flag == 0) {
        process_sendupdate(soc, cfg);
        // > loop で cfg->networks_num 回送る.
        update_flag = 1;
    }
        
    /* Recv packets. */
        /*---
            > ヘッダを参照して msg ごとに場合分け.
        ---*/
    fprintf(stdout, "--------------------\n");    
    read(soc, buf, sizeof(buf));  

    ptr = buf;
    hdr = (struct bgp_hdr *)ptr;

    /* Separate cases by msg type. */
    switch(hdr->type) {
        case KEEPALIVE_MSG: 
            printf("\x1b[35m");
            fprintf(stdout, "Recvd BGP KEEPALIVE MSG...\n");
            printf("\x1b[0m");
            print_keep(buf);
            // Timer Reset.
            process_sendkeep(soc, p);
            break;
        case UPDATE_MSG: 
            printf("\x1b[35m");
            fprintf(stdout, "Recvd BGP UPDATE MSG...\n");
            printf("\x1b[0m");
            print_update(buf);
            // Timer Reset.
            break;
        case NOTIFICATION_MSG:
            printf("unimplemented\n");
            exit(EXIT_FAILURE);
        default:   
            fprintf(stderr, "Unexpected message.\n");
            exit(EXIT_FAILURE);
    }
}
