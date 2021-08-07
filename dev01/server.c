/*
  > server.c 
  > 2021/08/17
  > Yokoo Kazuma
  > ソケットを用いたTCP通信を行うプログラム
  > ./socket [-hsc]
  > -h: Help, -s: Server Side, -c: Client Side
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "includes.h"

void server_side(){   
  unsigned short sPort;     // ポート番号
  int soc_wait;             // listen用ソケット 
  int soc;                  // 送受信用ソケット
  struct sockaddr_in sa;    // サーバ(me)情報
  char buf[BUFSIZE];        // 受信用バッファ
  char src_port[20];        // ポート番号標準入力用

  /* 待ち受けポートの指定 */
  fprintf(stdout, "Port to listen for ? : ");
  scanf("%s", src_port);
  if((sPort = (unsigned short)atoi(src_port)) == 0){
    fprintf(stderr, "Invalid port number.\n");
    exit(1);
  }

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

  while(1){
    /* 接続要求の受け入れ */
    fprintf(stdout, "Waiting for connection ...\n");
    soc = accept(soc_wait, NULL, NULL);
    fprintf(stdout, "Connected from %s\n", inet_ntoa(sa.sin_addr));

    /* パケットの受信、送信 */
    fprintf(stdout, "--------------------\n"); 
    do{
      recv(soc, buf, BUFSIZE, 0);
      fprintf(stdout, "received: %s\n", buf);
    } while(strcmp(buf, "quit") != 0);
    
    close(soc); 
  }  
}










