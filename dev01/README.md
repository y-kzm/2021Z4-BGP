## ソケットを用いたTCP通信でメッセージをやり取りするプログラム
- Usage: 
~~
$ ./socket [-hsc] 
-h: Help.
-s: Server Side.
-c: Client Side.
~~

- 実行例
  - Terminal1(server)
  ~~~
  ubuntu@student04:~/develop/dev01$ ./socket -s
  Server Side.
  Port to listen for ? : 8080
  Waiting for connection ...
  Connected from 0.0.0.0
  --------------------
  received: abcdefg
  received: asdfg
  received: quit
  Waiting for connection ...
  Connected from 0.0.0.0
  --------------------
  received: qwert
  received: zxcvb
  received: quit
  Waiting for connection ...
  ~~~
  
  - Terminal2(client)
  ~~~
  ubuntu@student04:~/develop/dev01$ ./socket -c
  Client Side.
  IP address connect to ? : 127.0.0.1
  Port to connect to ? : 8080
  Tring to connect 127.0.0.1 ...
  Connect to 127.0.0.1
  --------------------
  Send: abcdefg
  Send: asdfg
  Send: quit
  ubuntu@student04:~/develop/dev01$ ./socket -c
  Client Side.
  IP address connect to ? : 127.0.0.1
  Port to connect to ? : 8080
  Tring to connect 127.0.0.1 ...
  Connect to 127.0.0.1
  --------------------
  Send: qwert
  Send: zxcvb
  Send: quit
  ~~~

