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
void analyze_update(unsigned char *data, struct List *list)
{
    unsigned char *ptr = data;
    struct bgp_update   *bu = (struct bgp_update *)ptr;
    int i;
    int network_len = 0;    // withdrawn routes, nlri networks Length. 
    int networks_num = 0;   // nlri networks Num.
    int wr_num = 0;         // withdrawn routes num.
    int nlri_len = 0;       // "nlri filed" len.

    // Path Attrib.
    struct pa_code *pa_code;            // Use path attrib types to separate cases.
    struct pa_origin origin;
    struct pa_as_path as_path;
    struct pa_next_hop next_hop;
    struct pa_multi_exit_disc med;
    struct network networks[MAX_NETWORKS];
    struct network wr_networks[MAX_NETWORKS];

    // Update Msg Members.
    struct withdrawn_routes *wr;        // withdrawn_routes. > include "struct network routes[MAX_NETWORKS]"
    struct total_path_attrib *tpa;      // total_path_attribes.
    ptr = bu->contents;                 // From the next to the bgp header.
  

    /* withdrawn_routes. */
    wr = (struct withdrawn_routes *)ptr;
    if(ntohs(wr->len) != 0) {
        // Initializing variables.
        network_len = 0;
        wr_num = 0; 
        unsigned char *wr_end = ptr;
        i = 0;

        // Save the end pointer of "withdrawn routes" filed.
        wr_end += sizeof(uint8_t) * (TWOBYTE_FIELD_SIZE + ntohs(wr->len));
        // Advance the pointer by "withdrawn routes len" filed.
        ptr += sizeof(uint8_t) * TWOBYTE_FIELD_SIZE; 

        while(ptr < wr_end) {
            wr_networks[i] = analyze_withdrawn_routes(ptr, &wr_networks[i], &network_len);
            ptr += sizeof(uint8_t) * network_len;
            i ++;
            wr_num ++; 
        }
    } else 
        // If "withdrawn routes" is nothing.
        ptr += sizeof(uint8_t) * (TWOBYTE_FIELD_SIZE + ntohs(wr->len));
   
    /* total_path_attrib. */
    tpa = (struct total_path_attrib *)ptr;
    // Advance the pointer by "total path attrib len" filed. 
    ptr += sizeof(uint8_t) * (TWOBYTE_FIELD_SIZE);

    /* Print. */
    print_update(bu, wr, tpa, networks_num, wr_networks);

    /* Path Attrib. */
    // Initializing variables. 
    int as_path_len = 0;
    int origin_len = 0;
    int next_hop_len = 0;
    int med_len = 0;
    unsigned char *pa_end = ptr;
    // Save the end pointer of "path attributes" filed. 
    pa_end += sizeof(uint8_t) * (ntohs(tpa->total_len));

    while(ptr != pa_end) { 
        pa_code = (struct pa_code *)ptr;
        // printf(">> %u\n", pa_code->flags);
        // printf(">> %u\n", pa_code->code);
        switch(pa_code->code) {
            case ORIGIN:
                origin = analyze_origin(ptr, &origin_len);
                ptr += sizeof(uint8_t) * origin_len;
                print_origin(&origin); 
                break;
            case AS_PATH: 
                as_path = analyze_as_path(ptr, &as_path_len);
                ptr += sizeof(uint8_t) * as_path_len;
                print_as_path(&as_path, as_path.sgmnt.sgmnt_len);
                break;
            case NEXT_HOP: 
                next_hop = analyze_next_hop(ptr,  &next_hop_len);
                ptr += sizeof(uint8_t) * next_hop_len;
                print_next_hop(&next_hop);
                break;
            case MULTI_EXIT_DISC: 
                med = analyze_med(ptr, &med_len);
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
    nlri_len = 0;
    network_len = 0;
    networks_num = 0; 
    unsigned char *nlri_end = ptr;
    i = 0;
     /*--- 
          nlri_len = update_len - 23 - withdrawn_len - total_path_len
          23 = hdr(19byte) + wr_len(2byte) + tp_len(2byte)
          > nlri_len = bu->hdr.len - 23 - 0 - 28;
      ---*/
    nlri_len = ntohs(bu->hdr.len) - 23 - ntohs(wr->len) - ntohs(tpa->total_len);
    
    if(nlri_len > 0) {
        fprintf(stdout, "Network Layer Reachability Information.(NLRI)\n");
        // Save the end pointer of "nlri" filed. 
        nlri_end += sizeof(uint8_t) * nlri_len;
        while(ptr < nlri_end) {
            networks[i] = analyze_nlri(ptr, &networks[i], &network_len);
            print_nlri(&networks[i]);
            ptr += sizeof(uint8_t) * network_len;
            i ++;
            networks_num ++;
        } 
    } else 
        printf("\n");

    /* Process Table. */ 
    // If withdrawn route exists, perform the route deletion process.
    if(ntohs(wr->len) != 0) { 
        for(i = 0; i < wr_num; i ++) {
            process_del_table(&wr_networks[i], list);
            routing_del(&wr_networks[i]);
        }
    }

    // Loop through the number of ASes that have passed through.
    for(i = 0; i < as_path.sgmnt.sgmnt_len; i ++) {
        if(ntohs(as_path.sgmnt.sgmnt_value[i]) == 1) {      // 1: myas
            printf("\nWarning: ");
            printf("It does not write to the table because the AS_PATH attribute contains its own AS number.\n\n");
            return;
        }
    }

    // Write as many nlri's as received into the table.
    for(i = 0; i < networks_num; i ++) {
        process_add_table(&networks[i], &next_hop, &med, &as_path, list);
    }
    routing_add(list);
}

/*
    >>>>    ANALYZE PATH ATTRIB..    <<<<
*/
/*---------- analyze_origin ----------*/
struct pa_origin
analyze_origin(unsigned char *data, int *origin_len)
{
    struct pa_origin *origin;
    unsigned char *ptr = data;
    origin = (struct pa_origin *)ptr;

    *origin_len = 3 + origin->len;
    return *origin;
}

/*---------- analyze_as_path ----------*/
struct pa_as_path 
analyze_as_path(unsigned char *data, int *as_path_len)
{
    struct pa_as_path *as_path;
    unsigned char *ptr = data;
    as_path = (struct pa_as_path *)ptr;
    
    *as_path_len = 4 + ntohs(as_path->len);
    return *as_path;
}

/*---------- analyze_next_hop ----------*/
struct pa_next_hop 
analyze_next_hop(unsigned char *data, int *next_hop_len)
{
    struct pa_next_hop *next_hop;
    unsigned char *ptr = data;
    next_hop = (struct pa_next_hop *)ptr;

    *next_hop_len = 3 + next_hop->len;
    return *next_hop;
}

/*---------- analyze_med ----------*/
struct pa_multi_exit_disc
analyze_med(unsigned char *data, int *med_len)
{
    struct pa_multi_exit_disc *med;
    unsigned char *ptr = data;
    med = (struct pa_multi_exit_disc *)ptr;
    
    *med_len = 3 + med->len;
    return *med;
}

/*---------- analyze_nlri ----------*/
struct network
analyze_nlri(unsigned char *data, struct network *network, int *network_len)
{
    unsigned char *ptr = data;
    int i;
    struct network *network_raw = (struct network *)ptr;
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


/*---------- analyze_withdrawn_routes ----------*/
struct network 
analyze_withdrawn_routes(unsigned char *data, struct network *network, int *network_len)
{
    unsigned char *ptr = data;
    int i;
    struct network *network_raw = (struct network *)ptr;
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

