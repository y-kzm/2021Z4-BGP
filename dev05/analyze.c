/*
 * analyze.c
 * 2021/09/17 ~
 * Yokoo Kazuma
*/

#include <stdio.h>
#include "param.h"
#include "bgp.h"
#include "includes.h"

/*
    >>>>    ANALYZE BGP MSG.    <<<<
*/
/*---------- header ----------*/
void analyze_hdr(unsigned char *data)
{
    struct bgp_hdr     *keep;

    keep = (struct bgp_hdr *)data;

    print_hdr(keep);
}

/*---------- open ----------*/
void analyze_open(unsigned char *data)
{
    struct bgp_open_opt  *bop;

    bop = (struct bgp_open_opt *)data;

    print_open(bop);
}

/*---------- update ----------*/
void analyze_update(unsigned char *data)
{
    unsigned char *ptr = data;
    struct bgp_update   *bu = (struct bgp_update *)ptr;
    int i;

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

    // Print.
    print_update(bu, wr, tpa);

    /* Path Attrib. */
    struct pa_code *pa_code;
    int as_path_len;
    unsigned char *pa_end = ptr;
    pa_end += sizeof(uint8_t) * (ntohs(tpa->total_len));
    while(ptr != pa_end) { 
        pa_code = (struct pa_code *)ptr;
        // printf(">> %u\n", pa_code->flags);
        // printf(">> %u\n", pa_code->code);
        switch(pa_code->code) {
            case ORIGIN:
                analyze_origin(ptr, &origin);
                ptr += sizeof(origin);
                break;
            case AS_PATH: 
                as_path_len = analyze_as_path(ptr, &as_path);
                ptr += sizeof(uint8_t) * as_path_len;
                break;
            case NEXT_HOP: 
                analyze_next_hop(ptr, &next_hop);
                ptr += sizeof(next_hop);
                break;
            case MULTI_EXIT_DISC: 
                analyze_med(ptr, &med);
                ptr += sizeof(med);
                break;
            default:
                fprintf(stderr, "This is an unimplemented path attribute.\n");
                ptr = pa_end;
                // exit(EXIT_FAILURE);
        }
    }

    /* nlri. */
    int nlri_len, network_len; 
    unsigned char *nlri_end = ptr;
    i = 0;

    fprintf(stdout, "Network Layer Reachability Information.(NLRI)\n");
    nlri_len = ntohs(bu->hdr.len) - 23 - ntohs(wr->len) - ntohs(tpa->total_len);
    nlri_end += sizeof(uint8_t) * nlri_len;
    while(ptr < nlri_end) {
        network_len = analyze_nlri(ptr, &networks[i]);
        ptr += sizeof(uint8_t) * network_len;
        i ++;
    }

    printf("\n");
}

/*
    >>>>    ANALYZE PATH ATTRIB..    <<<<
*/
/*---------- analyze_origin ----------*/
void analyze_origin(unsigned char *data, struct pa_origin *origin)
{
    unsigned char *ptr = data;
    origin = (struct pa_origin *)ptr;

    print_origin(origin);
}

/*---------- analyze_as_path ----------*/
// 可変長対応可能に！
int analyze_as_path(unsigned char *data, struct pa_as_path *as_path)
{
    unsigned char *ptr = data;
    as_path = (struct pa_as_path *)ptr;

    print_as_path(as_path);

    return 4 + ntohs(as_path->len);
}

/*---------- analyze_next_hop ----------*/
void analyze_next_hop(unsigned char *data, struct pa_next_hop *next_hop)
{
    unsigned char *ptr = data;
    next_hop = (struct pa_next_hop *)ptr;

    print_next_hop(next_hop);
}

/*---------- analyze_med ----------*/
void analyze_med(unsigned char *data, struct pa_multi_exit_disc *med)
{
    unsigned char *ptr = data;
    med = (struct pa_multi_exit_disc *)ptr;
    
    print_med(med);
}

/*---------- analyze_nlri ----------*/
// 改良の余地あり...
int analyze_nlri(unsigned char *data, struct nlri_network *networks)
{
    unsigned char *ptr = data;
    struct nlri_network *networks_raw = (struct nlri_network *)ptr;
    int k;

    k = (networks_raw->prefix_len + (BYTE_SIZE - 1)) / BYTE_SIZE;
    networks->prefix_len = networks_raw->prefix_len;

    print_nlri(networks_raw, networks, k);

    // a network len.
    return 1 + k;
}

