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
    fprintf(stdout, "Len: %u\n", ntohs(bop->hdr.len));
    fprintf(stdout, "Type: %u\n", bop->hdr.type);  

    fprintf(stdout, "Version: %u\n", bop->version);
    fprintf(stdout, "MyAS: %u\n", ntohs(bop->myas));
    fprintf(stdout, "HoldTime: %u\n", ntohs(bop->holdtime));
    fprintf(stdout, "Id: %s\n", inet_ntoa(bop->id));
    fprintf(stdout, "Opt_Len: %u\n\n", bop->opt_len);
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
    fprintf(stdout, "Len: %u\n", ntohs(keep->len));
    fprintf(stdout, "Type: %u\n\n", keep->type);  
}

/*---------- print_open ----------*/
void print_update(unsigned char *data)
{
    struct bgp_update   *bu;
    int i;
 
    bu = (struct bgp_update *)data;

    fprintf(stdout, "Marker: ");
    for(i=0; i<MARKER_LEN; i++){
        fprintf(stdout, " %x", bu->hdr.marker[i]);
    } fprintf(stdout, " \n");
    fprintf(stdout, "Len: %u\n", ntohs(bu->hdr.len));
    fprintf(stdout, "Type: %u\n", bu->hdr.type); 
}



