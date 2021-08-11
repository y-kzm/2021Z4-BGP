/*
  > main.c
  > 2021/08/11
  > Yokoo Kazuma
  > FRR で gbp peer が立つ様な BGP を作る
  > ./mybgp
*/

#include <stdio.h>
#include <unistd.h>   // getopt(), システムコール
#include "includes.h"

// 後々、設定ファイルを用意して引数に渡す感じで...
int main(int argc, char *argv[])
{
  int opt, flag = 0;

  /* 引数error  */
  if(argc != 2){
    fprintf(stderr, "./sock [-hsc]\n");
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
        fprintf(stdout, "Usage: ./sock [-hsc]\n");
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




