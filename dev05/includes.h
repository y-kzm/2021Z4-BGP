#ifndef INCLUDES_H

/* Include. */
#include <sys/types.h>   
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/* Define. */
#define BUFSIZE             4096
#define BYTE_SIZE           8
#define TWOBYTE_FIELD_SIZE  2
#define IPV4_BLOCKS_NUM     4

/* Mode. */
// Unimplemented.
/*
enum MODE {
    SERVER,     // 0
    CLIENT      // 1
};
*/


/* Function */     
void usage(); 
void state_transition(struct peer *p, struct config *cfg);   
int tcp_connect(struct peer *p, struct config *cfg);       

// Print.
void print_open(struct bgp_open_opt *bop);
void print_hdr(struct bgp_hdr *bh);
void print_update(struct bgp_update *bu, struct withdrawn_routes *wr, struct total_path_attrib *tpa);

void print_origin(struct pa_origin *origin);
void print_as_path(struct pa_as_path *as_path, int len);
void print_next_hop(struct pa_next_hop *next_hop);
void print_med(struct pa_multi_exit_disc *med);
void print_nlri(struct nlri_network *network);

// Analyze.
void analyze_open(unsigned char *data);
void analyze_hdr(unsigned char *data);
void analyze_update(unsigned char *data, struct bgp_table_entry *table, struct peer *p);

struct pa_origin
analyze_origin(unsigned char *data, struct pa_origin *origin, int *origin_len);
struct pa_as_path 
analyze_as_path(unsigned char *data, struct pa_as_path *as_path, int *as_path_len);
struct pa_next_hop 
analyze_next_hop(unsigned char *data, struct pa_next_hop *next_hop, int *next_hop_len);
struct pa_multi_exit_disc
analyze_med(unsigned char *data, struct pa_multi_exit_disc *med, int *med_len);
struct nlri_network
analyze_nlri(unsigned char *data, struct nlri_network *network, int *network_len);

void print_table(struct bgp_table_entry table[], int index);

#endif