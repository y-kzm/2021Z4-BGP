/*
  > main.c
  > 2021/08/11
  > Yokoo Kazuma
  > FRR で gbp peer が立つ様な BGP を作る
  > ./mybgp
*/

#include <stdio.h>
#include <unistd.h>   // getopt(), システムコール
#include <stdlib.h>
#include <string.h>
#include "includes.h"
#include "bgp.h"

// 後々、設定ファイルを用意して引数に渡す感じで...
int main(int argc, char *argv[])
{
  int opt, flag = 0;

  /* 引数error  */
  if(argc != 2){
    fprintf(stderr, "./mybgp [-hsc]\n");
    return -1;
  }
  
  /* opt処理 */
  opterr = 0;
  // opt-argを持たない
  while((opt = getopt(argc, argv, "sch")) != -1){
    switch(opt){
      case 's':
        fprintf(stdout, "Server Side.\n");
        flag |= FLAG_SERVER;
        break; 
      case 'c':
        fprintf(stdout, "Client Side.\n");
        flag |= FLAG_CLIENT;
        break;
      case 'h': 
        fprintf(stdout, "Usage: ./mybgp [-hsc]\n");
        fprintf(stdout, "s: Server Side.\nc: Client Side.\nh: HELP.\n");
        return 0;
      default:
        fprintf(stderr, "Option Error.\n");
        return -1;
    } 
  }

  /* Optごとの処理 */
  if(flag & FLAG_SERVER){
    server_side();    
  } else if(flag & FLAG_CLIENT){
    client_side();
  }

  return 0;
}


/* サーバ側の処理 */
void server_side(){   
  unsigned short sPort = 179;   // ポート番号
  int soc_wait;                 // listen用ソケット 
  int soc;                      // 送受信用ソケット
  struct sockaddr_in sa;        // サーバ(me)情報
  unsigned char buf[BUFSIZE];   // 受信用バッファ

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

  /* パケットの受信 */
  fprintf(stdout, "--------------------\n");    
  
  read(soc, buf, BGP_OPEN_LEN);  
  analyze_bgp(buf, BGP_OPEN_LEN);
  
  close(soc); 
    
}


/* クライアント側の処理 */
void client_side(){
  unsigned short sPort = 179;   // ポート番号
  int soc;                      // 送受信用ソケット
  struct sockaddr_in sa;        // サーバ情報
  unsigned char buf[BUFSIZE];   // 送信用バッファ

   /* ソケット生成 */
  if((soc = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("socket() failed.");
    exit(1);
  }

  /* サーバ情報を格納 */
  memset(&sa, 0, sizeof(sa));
  sa.sin_family       = AF_INET;
  sa.sin_addr.s_addr  = inet_addr("10.255.1.1");
  sa.sin_port         = htons(sPort);

  /* サーバに接続 */
  //fprintf(stdout, "Tring to connect %s ...\n", );
  if(connect(soc, (struct sockaddr*) &sa, sizeof(sa)) < 0) {
    perror("connect() failed.");
    exit(1);
  } 

  /* BGP情報を格納 */
  set_bgp(buf, BGP_OPEN_LEN);       

  // パケットの送信 
  fprintf(stdout, "--------------------\n");
  fprintf(stdout, "Sending...\n"); 
  
  write(soc, buf, BGP_OPEN_LEN);
  
  close(soc);
}



