/*
  > client.c 
  > 2021/08/11
  > Yokoo Kazuma
  > FRR で BGP peer が立つ様な BGP を作る
  > ./mybgp [-hsc]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "includes.h"

void client_side(){
  unsigned short sPort = 179;   // ポート番号
  int soc;                      // 送受信用ソケット
  struct sockaddr_in sa;        // サーバ情報
  char buf[BUFSIZE];            // 受信用バッファ
  /* 標準入力用 */
  char dst_ip[50];              // configに書いて読み込む方式に...

  /* 宛先IPアドレスと宛先ポートを指定 */
  fprintf(stdout, "IP address connect to ? : ");
  scanf("%s", dst_ip);

   /* ソケット生成 */
  if((soc = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("socket() failed.");
    exit(1);
  }

  /* サーバ情報を格納 */
  memset(&sa, 0, sizeof(sa));
  sa.sin_family       = AF_INET;
  sa.sin_addr.s_addr  = inet_addr(dst_ip);
  sa.sin_port         = htons(sPort);

  /* サーバに接続 */
  fprintf(stdout, "Tring to connect %s ...\n", dst_ip);
  if(connect(soc, (struct sockaddr*) &sa, sizeof(sa)) < 0) {
    perror("connect() failed.");
    exit(1);
  } 
  fprintf(stdout, "Connect to %s\n", dst_ip);

  /* BGP情報を格納 */
  bgp_send();  

  /* パケットの送信 */
  fprintf(stdout, "--------------------\n");
  while(1){ 
    write(soc, buf, strlen(buf)+1, 0);
  }

  close(soc);
}


