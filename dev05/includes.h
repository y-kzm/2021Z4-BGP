#ifndef INCLUDES_H

/* Include. */
#include <sys/types.h>   
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>

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
void print_update(struct bgp_update *bu, struct withdrawn_routes *wr, struct total_path_attrib *tpa, int num, struct network wr_routes[]);

void print_origin(struct pa_origin *origin);
void print_as_path(struct pa_as_path *as_path, int len);
void print_next_hop(struct pa_next_hop *next_hop);
void print_med(struct pa_multi_exit_disc *med);
void print_nlri(struct network *network);
void print_withdrawn_routes(struct network *network);
void print_table(struct List *list);

// Analyze.
void analyze_open(unsigned char *data);
void analyze_hdr(unsigned char *data);
void analyze_update(unsigned char *data, struct List *list);

struct pa_origin
analyze_origin(unsigned char *data, int *origin_len);
struct pa_as_path 
analyze_as_path(unsigned char *data, int *as_path_len);
struct pa_next_hop 
analyze_next_hop(unsigned char *data, int *next_hop_len);
struct pa_multi_exit_disc
analyze_med(unsigned char *data, int *med_len);
struct network
analyze_nlri(unsigned char *data, struct network *network, int *network_len);
struct network 
analyze_withdrawn_routes(unsigned char *data, struct network *network, int *network_len);



// Process msg.
void process_sendopen(int soc, struct peer *p, struct config *cfg);
void process_recvopen(int soc);
void process_sendkeep(int soc, struct peer *p);
void process_recvkeep(int soc, struct peer *p);
void process_sendupdate(int soc, struct config *cfg);
void process_established(int soc, struct peer *p, struct config *cfg);

// Store path attrib.
void store_origin(struct pa_origin *origin);
int store_as_path(struct pa_as_path *as_path, struct config *cfg);
void store_next_hop(struct pa_next_hop *next_hop);
void store_med(struct pa_multi_exit_disc *med);
void store_nlri(struct network *network, struct config *cfg, int nlri_mode, int index);

// Store msg.
void store_open(struct bgp_open *bo, struct config *cfg);
void store_keep(struct bgp_hdr *keep);
void store_update(struct bgp_update *bu, struct config *cfg, int index);

// Process table.
struct bgp_table_entry *AllocTableEntry(void);
void InitList(struct List *list);
void process_add_table(
    struct network *network, struct pa_next_hop *next_hop, 
    struct pa_multi_exit_disc *med, struct pa_as_path *as_path,
    struct List *list);
void process_del_table(struct network *network, struct List *list);

void routing_add(struct List *list);
void routing_del(struct network *network);

#endif