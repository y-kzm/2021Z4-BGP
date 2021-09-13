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
    struct bgp_update   *bu = (struct bgp_update *)data;
    // unsigned char *ptr = bu->contents;
    int i;

    // // Path Attrib.
    // struct pa_origin origin;
    // struct pa_as_path as_path;
    // struct pa_next_hop next_hop;
    // struct pa_multi_exit_disc med;
    // struct nlri nlri;

    // // Update Msg Members.
    // struct withdrawn_routes *wr;  
    // struct total_path_attrib *tp;
    
    /* Header. */
    fprintf(stdout, "Marker: ");
    for(i=0; i<MARKER_LEN; i++){
        fprintf(stdout, " %x", bu->hdr.marker[i]);
    } fprintf(stdout, " \n");
    fprintf(stdout, "Len:  %u\n", ntohs(bu->hdr.len));
    fprintf(stdout, "Type: %u\n", bu->hdr.type); 
    printf("Continuation not implemented.\n\n");
}

//    /* withdrawn_routes. */
//    wr->len = ntohs(&ptr);
//    // Dynamic range.
//    wr = (struct withdrawn_routes *)malloc(
//        sizeof(struct withdrawn_routes) + sizeof(uint8_t) * wr->len);
//    // もし到達不能経路があるならここに処理を追加
//
//    ptr += sizeof(uint8_t) * (2 + 0);
//    fprintf(stdout, "Withdrawn Routes Len: %u", wr->len);
//    // print withdrawn routes.
//
//    /* total_path_attrib. */
//    tp->total_len = ntohs(&ptr);
//    // Dynamic range.
//    tp = (struct total_path_attrib *)malloc(
//        sizeof(struct total_path_attrib) + sizeof(uint8_t) * tp->total_len);
//
//    ptr += sizeof(uint8_t);
//    fprintf(stdout, "Total Path Len: %u", tp->total_len);   
//
//    /* Path Attrib. */
//
//
//    
//}
//
///*---------- print_origin ----------*/
//void print_origin()
//{
//
//}
//
///*---------- print_as_path ----------*/
//void print_as_path()
//{
//
//}
//
///*---------- print_next_hop ----------*/
//void print_next_hop()
//{
//
//}
//
///*---------- print_med ----------*/
//void print_med()
//{
//
//}
//
///*---------- print_nlri ----------*/
//void print_nlri()
//{
//
//}
//

