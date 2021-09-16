/*
 * print.c
 * 2021/09/06 ~
 * Yokoo Kazuma
*/

#include <stdio.h>
#include "param.h"
#include "bgp.h"
#include "includes.h"

/*
    >>>>    PRINT MSG.    <<<<
*/
/*---------- print_open ----------*/
void print_open(unsigned char *data)
{
    struct bgp_open_opt  *bop;
    int i;

    bop = (struct bgp_open_opt *)data;

    fprintf(stdout, "Marker: ");
    for(i=0; i<MARKER_LEN; i++){
        fprintf(stdout, " %x", bop->hdr.marker[i]);
    } fprintf(stdout, " \n");
    fprintf(stdout, "Len:  %u\n", ntohs(bop->hdr.len));
    fprintf(stdout, "Type: %u\n", bop->hdr.type);  

    fprintf(stdout, "Version:  %u\n", bop->version);
    fprintf(stdout, "MyAS:     %u\n", ntohs(bop->myas));
    fprintf(stdout, "HoldTime: %u\n", ntohs(bop->holdtime));
    fprintf(stdout, "Id:       %s\n", inet_ntoa(bop->id));
    fprintf(stdout, "Opt_Len:  %u\n\n", bop->opt_len);
}

/*---------- print_keep ----------*/
void print_keep(unsigned char *data)
{
    struct bgp_hdr     *keep;
    int i;

    keep = (struct bgp_hdr *)data;

    fprintf(stdout, "Marker: ");
    for(i=0; i<MARKER_LEN; i++){
        fprintf(stdout, " %x", keep->marker[i]);
    } fprintf(stdout, " \n");
    fprintf(stdout, "Len:  %u\n", ntohs(keep->len));
    fprintf(stdout, "Type: %u\n\n", keep->type);  
}

/*---------- print_open ----------*/
void print_update(unsigned char *data)
{
    unsigned char *ptr = data;
    struct bgp_update   *bu = (struct bgp_update *)ptr;
    int i;

    /* Header. */
    fprintf(stdout, "Marker: ");
    for(i=0; i<MARKER_LEN; i++){
        fprintf(stdout, " %x", bu->hdr.marker[i]);
    } fprintf(stdout, " \n");
    fprintf(stdout, "Len:  %u\n", ntohs(bu->hdr.len));
    fprintf(stdout, "Type: %u\n", bu->hdr.type); 
    
    // debug.
    // for(i=0; i<53; i++){
    //     fprintf(stdout, "   %u\n", data[i]); 
    // }
    

    // Path Attrib.
    struct pa_origin origin;
    struct pa_as_path as_path;
    struct pa_next_hop next_hop;
    struct pa_multi_exit_disc med;
    struct nlri_network networks[MAX_NETWORKS];

    // Update Msg Members.
    struct withdrawn_routes *wr;  
    struct total_path_attrib *tpa;

    ptr = bu->contents;
  
    /* withdrawn_routes. */
    // Dynamic range.
    wr = (struct withdrawn_routes *)malloc(
        sizeof(struct withdrawn_routes) + sizeof(uint8_t) * MAX_WITHDRAWN);
    wr = (struct withdrawn_routes *)ptr;
    // もし到達不能経路があるならここに処理を追加
    
    ptr += sizeof(uint8_t) * (TWOBYTE_FIELD_SIZE + ntohs(wr->len));
   
    /* total_path_attrib. */
    // Dynamic range.
    tpa = (struct total_path_attrib *)malloc(
        sizeof(struct total_path_attrib) + sizeof(uint8_t) * MAX_PATH_ATTRIB);
    tpa = (struct total_path_attrib *)ptr;

    ptr += sizeof(uint8_t) * (TWOBYTE_FIELD_SIZE);

    // print.
    fprintf(stdout, "Withdrawn Routes Len: %u\n", ntohs(wr->len));
    // もし到達不能経路があるならprint処理
    fprintf(stdout, "Total Path Len      : %u\n", ntohs(tpa->total_len)); 
    

    /* Path Attrib. */
    struct pa_code *pa_code;
    int as_path_len;
    unsigned char *pa_end = ptr;
    pa_end += sizeof(uint8_t) * (ntohs(tpa->total_len));
    fprintf(stdout, "Path Attributes.\n"); 
    while(ptr != pa_end) { 
        pa_code = (struct pa_code *)ptr;
        // printf(">> %u\n", pa_code->flags);
        // printf(">> %u\n", pa_code->code);
        switch(pa_code->code) {
            case ORIGIN:
                print_origin(ptr, &origin);
                ptr += sizeof(origin);
                break;
            case AS_PATH: 
                as_path_len = print_as_path(ptr, &as_path);
                ptr += sizeof(uint8_t) * as_path_len;
                break;
            case NEXT_HOP: 
                print_next_hop(ptr, &next_hop);
                ptr += sizeof(next_hop);
                break;
            case MULTI_EXIT_DISC: 
                print_med(ptr, &med);
                ptr += sizeof(med);
                break;
            default:
                fprintf(stderr, "This is an unimplemented path attribute.\n");
                ptr = pa_end;
                // exit(EXIT_FAILURE);
        }
    }

    /* nlri. */
    fprintf(stdout, "Network Layer Reachability Information.(NLRI)\n");
    int nlri_len, network_len; 
    unsigned char *nlri_end = ptr;
    i = 0;

    nlri_len = ntohs(bu->hdr.len) - 23 - ntohs(wr->len) - ntohs(tpa->total_len);
    nlri_end += sizeof(uint8_t) * nlri_len;
    while(ptr < nlri_end) {
        network_len = print_nlri(ptr, &networks[i]);
        ptr += sizeof(uint8_t) * network_len;
        i ++;
    }

    printf("\n");
}


/*---------- print_origin ----------*/
void print_origin(unsigned char *data, struct pa_origin *origin)
{
    unsigned char *ptr = data;
    origin = (struct pa_origin *)ptr;

    fprintf(stdout, "  Path Attrib: ORIGIN\n");
    fprintf(stdout, "    flags : 0x%x\n", origin->flags);
    fprintf(stdout, "    code  : %x\n", origin->code);
    fprintf(stdout, "    len   : %x\n", origin->len);
    fprintf(stdout, "    origin: %x\n", origin->origin);
}

/*---------- print_as_path ----------*/
// 可変長対応可能に！
int print_as_path(unsigned char *data, struct pa_as_path *as_path)
{
    unsigned char *ptr = data;
    as_path = (struct pa_as_path *)ptr;

    fprintf(stdout, "  Path Attrib: AS_PATH\n");
    fprintf(stdout, "    flags : 0x%x\n", as_path->flags);
    fprintf(stdout, "    code  : %x\n", as_path->code);
    fprintf(stdout, "    len   : %x\n", ntohs(as_path->len));
    fprintf(stdout, "    AS Path sgmnt.\n");
    fprintf(stdout, "      Segment type: %u\n", as_path->sgmnt.sgmnt_type);
    fprintf(stdout, "      Segment len : %u\n", as_path->sgmnt.sgmnt_len); 
    fprintf(stdout, "      AS2: %u\n", ntohs(as_path->sgmnt.sgmnt_value));

    return 4 + ntohs(as_path->len);
}

/*---------- print_next_hop ----------*/
void print_next_hop(unsigned char *data, struct pa_next_hop *next_hop)
{
    unsigned char *ptr = data;
    next_hop = (struct pa_next_hop *)ptr;

    fprintf(stdout, "  Path Attrib: NEXT_HOP\n");
    fprintf(stdout, "    flags : 0x%x\n", next_hop->flags);
    fprintf(stdout, "    code  : %x\n", next_hop->code);
    fprintf(stdout, "    len   : %x\n", next_hop->len);
    fprintf(stdout, "    Next hop: %s\n", inet_ntoa(next_hop->nexthop));
}

/*---------- print_med ----------*/
void print_med(unsigned char *data, struct pa_multi_exit_disc *med)
{
    unsigned char *ptr = data;
    med = (struct pa_multi_exit_disc *)ptr;

    fprintf(stdout, "  Path Attrib: MULTI_EXIT_DISC\n");
    fprintf(stdout, "    flags : 0x%x\n", med->flags);
    fprintf(stdout, "    code  : %x\n", med->code);
    fprintf(stdout, "    len   : %x\n", med->len);
    fprintf(stdout, "    med   : %x\n", ntohl(med->med));
}

/*---------- print_nlri ----------*/
// 改良の余地あり...
int print_nlri(unsigned char *data, struct nlri_network *networks)
{
    unsigned char *ptr = data;
    struct nlri_network *networks_raw = (struct nlri_network *)ptr;
    int i, k;

    k = (networks_raw->prefix_len + (BYTE_SIZE - 1)) / BYTE_SIZE;
    networks->prefix_len = networks_raw->prefix_len;

    fprintf(stdout, "  "); 
    for(i = 0; i < IPV4_BLOCKS_NUM; i ++) { 
        networks->prefix[i] = networks_raw->prefix[i];
        if(i > k-1)
            networks->prefix[i] = 0;
        if(i == IPV4_BLOCKS_NUM-1)
            fprintf(stdout, "%u", networks->prefix[i]); 
        else 
            fprintf(stdout, "%u.", networks->prefix[i]);
    }
    fprintf(stdout, "/%u\n", networks->prefix_len); 

    // a network len.
    return 1 + k;
}


/*---  memo
    print関数とanalyze処理がごっちゃになってる
    print関数とanalyze関数をしっかり分ける！

    analyze関数を使って"sh bgp ipv4 unicast"が作れると思う！

    update msg のnアナウンスを１つずつに変更 OK
---*/