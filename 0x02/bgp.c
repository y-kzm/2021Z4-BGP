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
  struct bgp_open *bo;
  unsigned char *ptr;
  int i;
  
  ptr = data;

  bo = (struct bgp_open *)ptr;
  ptr += BGP_OPEN_LEN;
  fprintf(stdout, "-----BGP HEADER-----\n");
  fprintf(stdout, "Marker: ");
  for(i=0; i<16; i++){
    fprintf(stdout, " %x", bo->marker[i]);
  } fprintf(stdout, " \n");
  fprintf(stdout, "Len: %u\n", ntohs(bo->len));
  fprintf(stdout, "Type: %u\n", bo->type);  

  fprintf(stdout, "-----OPEN MSG-----\n");
  fprintf(stdout, "Version: %u\n", bo->version);
  fprintf(stdout, "MyAS: %u\n", ntohs(bo->myas));
  fprintf(stdout, "HoldTime: %u\n", ntohs(bo->holdtime));
  fprintf(stdout, "Id: %s\n", inet_ntoa(bo->id));
  fprintf(stdout, "Opt_Len: %u\n", bo->opt_len);

}

// 構造体に情報を入れて送信
void set_bgp(unsigned char *buf, int size){
  struct bgp_open bo;
  int i;

  for(i=0; i<16; i++){
    bo.marker[i] = 255;
  }
  bo.len = htons(BGP_OPEN_LEN);
  bo.type = 1;     // open
  
  bo.version = 4;
  bo.myas = htons(2);
  bo.holdtime = htons(180);
  bo.id.s_addr = inet_addr("10.255.1.2");
  bo.opt_len = 0;

  //ptr = buf;
  memcpy(buf, &bo, BGP_OPEN_LEN);
  //ptr += sizeof(struct bgp_hdr);

}
