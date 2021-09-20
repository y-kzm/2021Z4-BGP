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
void analyze_update(unsigned char *data, struct bgp_table_entry table[], struct peer *p)
{
    unsigned char *ptr = data;
    struct bgp_update   *bu = (struct bgp_update *)ptr;
    int i;

    // Path Attrib.
    struct pa_origin origin;
    // memset(&origin, 0, sizeof(origin));
    struct pa_as_path as_path;
    // memset(&as_path, 0, sizeof(as_path));
    struct pa_next_hop next_hop;
    // memset(&next_hop, 0, sizeof(next_hop));
    struct pa_multi_exit_disc med;
    // memset(&med, 0, sizeof(med));
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
    int as_path_len = 0;
    int origin_len = 0;
    int next_hop_len = 0;
    int med_len = 0;
    unsigned char *pa_end = ptr;
    pa_end += sizeof(uint8_t) * (ntohs(tpa->total_len));
    while(ptr != pa_end) { 
        pa_code = (struct pa_code *)ptr;
        // printf(">> %u\n", pa_code->flags);
        // printf(">> %u\n", pa_code->code);
        switch(pa_code->code) {
            case ORIGIN:
                origin = analyze_origin(ptr, &origin, &origin_len);
                ptr += sizeof(uint8_t) * origin_len;
                print_origin(&origin); 
                break;
            case AS_PATH: 
                as_path = analyze_as_path(ptr, &as_path, &as_path_len);
                ptr += sizeof(uint8_t) * as_path_len;
                print_as_path(&as_path, as_path.sgmnt.sgmnt_len);
                break;
            case NEXT_HOP: 
                next_hop = analyze_next_hop(ptr, &next_hop, &next_hop_len);
                ptr += sizeof(uint8_t) * next_hop_len;
                print_next_hop(&next_hop);
                break;
            case MULTI_EXIT_DISC: 
                med = analyze_med(ptr, &med, &med_len);
                ptr += sizeof(uint8_t) * med_len;
                print_med(&med);
                break;
            default:
                fprintf(stderr, "This is an unimplemented path attribute.\n");
                ptr = pa_end;
                // exit(EXIT_FAILURE);
        }
    }

    /* nlri. */
    int nlri_len = 0;
    int network_len = 0;
    int networks_num = 0; 
    unsigned char *nlri_end = ptr;
    i = 0;

    fprintf(stdout, "Network Layer Reachability Information.(NLRI)\n");
    nlri_len = ntohs(bu->hdr.len) - 23 - ntohs(wr->len) - ntohs(tpa->total_len);
    nlri_end += sizeof(uint8_t) * nlri_len;
    while(ptr < nlri_end) {
        networks[i] = analyze_nlri(ptr, &networks[i], &network_len);
        print_nlri(&networks[i]);
        ptr += sizeof(uint8_t) * network_len;
        i ++;
        networks_num ++;
    }
    printf("\n");
    
    
    /* Process Table. */
     for(i = 0; i < as_path.sgmnt.sgmnt_len; i ++) {
        if(ntohs(as_path.sgmnt.sgmnt_value[i]) == 1) {
            printf("Warning: ");
            printf("It does not write to the table because the AS_PATH attribute contains its own AS number.\n\n");
            p->entry_num = 255;
            return;
        }
    }
    static int entry_index = 0;
    for(i = 0; i < networks_num; i ++) {
        process_table(&networks[i], &next_hop, &med, &as_path, table, entry_index);
        entry_index ++;
    }
    p->entry_num = entry_index;
}

/*
    >>>>    ANALYZE PATH ATTRIB..    <<<<
*/
/*---------- analyze_origin ----------*/
struct pa_origin
analyze_origin(unsigned char *data, struct pa_origin *origin, int *origin_len)
{
    unsigned char *ptr = data;
    origin = (struct pa_origin *)ptr;

    *origin_len = 3 + origin->len;
    return *origin;
}

/*---------- analyze_as_path ----------*/
struct pa_as_path 
analyze_as_path(unsigned char *data, struct pa_as_path *as_path, int *as_path_len)
{
    unsigned char *ptr = data;
    as_path = (struct pa_as_path *)ptr;
    
    *as_path_len = 4 + ntohs(as_path->len);
    return *as_path;
}

/*---------- analyze_next_hop ----------*/
struct pa_next_hop 
analyze_next_hop(unsigned char *data, struct pa_next_hop *next_hop, int *next_hop_len)
{
    unsigned char *ptr = data;
    next_hop = (struct pa_next_hop *)ptr;

    *next_hop_len = 3 + next_hop->len;
    return *next_hop;
}

/*---------- analyze_med ----------*/
struct pa_multi_exit_disc
analyze_med(unsigned char *data, struct pa_multi_exit_disc *med, int *med_len)
{
    unsigned char *ptr = data;
    med = (struct pa_multi_exit_disc *)ptr;
    
    *med_len = 3 + med->len;
    return *med;
}

/*---------- analyze_nlri ----------*/
struct nlri_network
analyze_nlri(unsigned char *data, struct nlri_network *network, int *network_len)
{
    unsigned char *ptr = data;
    int i;
    struct nlri_network *network_raw = (struct nlri_network *)ptr;
    int addr_len = (network_raw->prefix_len + (BYTE_SIZE - 1)) / BYTE_SIZE; 

    network->prefix_len = network_raw->prefix_len;
    for(i = 0; i < IPV4_BLOCKS_NUM; i ++) { 
        network->prefix[i] = network_raw->prefix[i];
        if(i > addr_len-1) {
            network->prefix[i] = 0;
        }
    }
    

    // a network len.
    *network_len = 1 + addr_len;
    return *network;
}

