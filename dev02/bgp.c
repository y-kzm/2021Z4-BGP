/*
  > bgp.c 
  > 2021/08/11
  > Yokoo Kazuma
  > FRR で BGP peer が立つ様な BGP を作る
  > ./mybgp [-hsc]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "includes.h"
#include "bgp.h"

// dataを先頭から解析
void analyze_bgp(unsigned char *data, int size){
  struct bgp_hdr *bh;
  struct bgp_open *bo;
  unsigned char *ptr
  int lest;
  
  ptr = data;
  lest = size;

  bh = (struct bgp_hdr *)ptr;
  ptr += sizeof(struct bgp_hdr);
  fprintf(stdout, "-----BGP HEADER-----\n");
  fprintf(stdout, "Marker: ");
  for(i=0; i<16; i++){
    fprintf(stdout, " %u", bh->marker[i]);
  } fprintf(stdout, " \n");
  fprintf(stdout, "Len: %u\n", bh->len);
  fprintf(stdout, "Type: %u\n", bh->type);  

  bp = (struct bgp_open *)ptr;
  ptr += sizeof(struct bgp_open);
  fprintf(stdout, "-----OPEN MSG-----\n");
  fprintf(stdout, "Version: %u\n", b0->version);
  fprintf(stdout, "MyAS: %u\n", bo->myas);
  fprintf(stdout, "HoldTime: %u\n", bo->holdtime);
  fprintf(stdout, "Id: %u\n", bo->id);
  fprintf(stdout, "Opt_Len: %u\n", bo->opt_len);
}

// 構造体に情報を入れて送信
void send_bgp(int soc, int size){
  struct bgp_hdr *bh;
  struct bgp_open *bo;
  int i;
  unsigned char *ptr, buf[BUFSIZE];

  // とりあえず全て固定値
  for(i=0; i<16; i++){
    bh->marker[i] = 1;
  }
  bh->len = 29;
  bh->type = 1;     // open
  
  bo->version = 4;
  bo->myas = 1;
  bo->holdtime = 180;
  bo->id = inet_addr(10.255.1.1);
  bo->opt_len = 0;

  ptr = buf;
  memcpy(ptr, &bh, sizeof(struct bgp_hdr));
  ptr += sizeof(struct bgp_hdr);
  memcpy(ptr, &bo, sizeof(struct bgp_open));
  ptr += sizeof(struct bgp_open);

  while(1){
    write(soc, buf, size);
  }
}
