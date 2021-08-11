/*
  > server.c 
  > 2021/08/11
  > Yokoo Kazuma
  > FRR で BGP peer が立つ様な BGP を作る
  > ./mybgp [-hsc]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "includes.h"

// int にしてエラー処理入れても良いかも
void server_side(){   
  unsigned short sPort = 179;   // ポート番号
  int soc_wait;                 // listen用ソケット 
  int soc;                      // 送受信用ソケット
  struct sockaddr_in sa;        // サーバ(me)情報
  unsigned char buf[BUFSIZE];   // 受信用バッファ
  int size;

  /* ソケット生成 */
  if((soc_wait = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("socket() failed.");
    exit(1);
  }

  /* サーバ情報を格納 */
  memset(&sa, 0, sizeof(sa));
  sa.sin_family       = AF_INET;
  sa.sin_addr.s_addr  = htonl(INADDR_ANY);  // 任意のアドレスを示す(0.0.0.0)
  sa.sin_port         = htons(sPort);

  /* サーバ情報をソケットに設定 */
  if(bind(soc_wait, (struct sockaddr *) &sa, sizeof(sa)) < 0){
    perror("bind() failed.");
    exit(1);
  }

  /* 待ち受け */
  if(listen(soc_wait, QUELIM) < 0){
    perror("listen() failed.");
    exit(1);
  }
    
  fprintf(stdout, "Waiting for connection ...\n");
  soc = accept(soc_wait, NULL, NULL);
  fprintf(stdout, "Connected from %s\n", inet_ntoa(sa.sin_addr));

  /* パケットの受信 */
  fprintf(stdout, "--------------------\n"); 
  while(1){
    size = read(soc, buf, BUFSIZE);
    BgpRecv(buf, size);
  }   
  
  close(soc); 
    
}


