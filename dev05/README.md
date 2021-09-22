# 自作BGPとFRR間でメッセージのやりとりを行う.
* "sh bgp ipv4 unicast" の表示.
* withdrawn routes の受信処理.
* 上記で実装したBGPテーブルを参照して"ip route add, del" を行う.
* Best Path Selection アルゴリズムは実装できていない.

---
## ファイル構成: 
* main.c  
    - main()  
        - jsonファイルのopen.  
        - state_transition() の周回.  
    - state_transition()  
        - BGP STATE ごとの処理.  
        - 状態遷移関数.  
    - tcp_connect()  
        - TCP コネクションを確立.  
* bgp.c   
    - process_send"MSG"()
        - BGP MSG 格納関数を呼ぶ.
        - BGP MSG の送信.
    - process_recv"MSG"()
        - BGP MSG の受信.
        - BGP MSG 解析関数を呼ぶ.
    - store_"MSG"()
        - send"MSG"() で呼ばれる.
        - BGP MSG 情報を格納.
    - store_"PA"()
        - PATH ATTRIB 情報を格納. 
    - process_table()
        - テーブルにnetwork情報を追加、削除.
    - routing_process()
        - "ip route add, del" を行う.
* analyze.c
    - analyze_"MSG,PA"()
        - process_recv"MSG"() で呼ばれる.
        - 受信したBGP MSG を構造体に格納.
        - print関数を呼ぶ.
* print.c
    - print_"MSG,PA"()
        - analyze関数で呼ばれる.
        - 格納した構造体メンバを表示する.
    - print_table()
        - BGPテーブルを表示.
* param.c
    - jsonファイルの内容を構造体へ格納.

---
## 実行例.
- ./mybgp 実行画面.
    ~~~
    root@R1:/work# ./mybgp conf.jsona 
    --------------------
    Loaded the following settings.
    > myas  : 1
    > id    : 1.1.1.1
    > neighbor_address: 10.255.1.2
    > remote_as       : 2
    > networks prefix[0]:  10.1.0.0/24
    > networks prefix[1]:  10.2.0.0/24
    > networks prefix[2]:  10.255.1.0/24
    
    --------------------
    Trying to connect to 10.255.1.2 
    
    --------------------
    Sending OPEN MSG...
    
    --------------------
    Recvd OPEN MSG...
    Marker:  ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
    Len:  75
    Type: 1
    Version:  4
    MyAS:     2
    HoldTime: 180
    Id:       2.2.2.2
    Opt_Len:  46
    
    --------------------
    Sending KEEPALIVE MSG...
    
    --------------------
    Recvd KEEPALIVE MSG...
    Marker:  ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
    Len:  19
    Type: 4
    
    --------------------
    Sending UPDATE MSG [0]...
    
    --------------------
    Sending UPDATE MSG [1]...
    
    --------------------
    Sending UPDATE MSG [2]...
    
    --------------------
    Recvd UPDATE MSG...
    Marker:  ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
    Len:  52
    Type: 2
    Withdrawn Routes Len: 0
    Total Path Len: 21
    Path Attributes.
      Path Attrib: ORIGIN
        flags : 0x40
        code  : 1
        len   : 1
        origin: 2
      Path Attrib: AS_PATH
        flags : 0x50
        code  : 2
        len   : 6
        AS Path sgmnt.
          Segment type: 2
          Segment len : 2
          AS2: 2
          AS2: 1
      Path Attrib: NEXT_HOP
        flags : 0x40
        code  : 3
        len   : 4
        Next hop: 10.255.1.1
    Network Layer Reachability Information.(NLRI)
      10.1.0.0/24
      10.2.0.0/24
    
    Warning: It does not write to the table because the AS_PATH attribute contains its own AS number.
    
    --------------------
    Recvd UPDATE MSG...
    Marker:  ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
    Len:  53
    Type: 2
    Withdrawn Routes Len: 0
    Total Path Len: 26
    Path Attributes.
      Path Attrib: ORIGIN
        flags : 0x40
        code  : 1
        len   : 1
        origin: 2
      Path Attrib: AS_PATH
        flags : 0x50
        code  : 2
        len   : 4
        AS Path sgmnt.
          Segment type: 2
          Segment len : 1
          AS2: 2
      Path Attrib: NEXT_HOP
        flags : 0x40
        code  : 3
        len   : 4
        Next hop: 10.255.1.2
      Path Attrib: MULTI_EXIT_DISC
        flags : 0x80
        code  : 4
        len   : 4
        med   : 0
    Network Layer Reachability Information.(NLRI)
      10.3.0.0/24
    
    +--------------------+-----------------+---------+-------------+
      Network:           | NextHop:        | Metric: | Path:        
      10.3.0.0       /24 | 10.255.1.2      | 0       | 2 
    +--------------------+-----------------+---------+-------------+
    >> ip route add 10.3.0.0/24 via 10.255.1.2
    
    --------------------
    Recvd UPDATE MSG...
    Marker:  ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
    Len:  53
    Type: 2
    Withdrawn Routes Len: 0
    Total Path Len: 26
    Path Attributes.
      Path Attrib: ORIGIN
        flags : 0x40
        code  : 1
        len   : 1
        origin: 2
      Path Attrib: AS_PATH
        flags : 0x50
        code  : 2
        len   : 4
        AS Path sgmnt.
          Segment type: 2
          Segment len : 1
          AS2: 2
      Path Attrib: NEXT_HOP
        flags : 0x40
        code  : 3
        len   : 4
        Next hop: 10.255.1.2
      Path Attrib: MULTI_EXIT_DISC
        flags : 0x80
        code  : 4
        len   : 4
        med   : 0
    Network Layer Reachability Information.(NLRI)
      10.4.0.0/24
    
    +--------------------+-----------------+---------+-------------+
      Network:           | NextHop:        | Metric: | Path:        
      10.3.0.0       /24 | 10.255.1.2      | 0       | 2 
      10.4.0.0       /24 | 10.255.1.2      | 0       | 2 
    +--------------------+-----------------+---------+-------------+
    >> ip route add 10.4.0.0/24 via 10.255.1.2
    
    --------------------
    Recvd UPDATE MSG...
    Marker:  ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
    Len:  31
    Type: 2
    Withdrawn Routes Len: 8
    Withdrawn Routes.
    Total Path Len: 0
    
    
    +--------------------+-----------------+---------+-------------+
      Network:           | NextHop:        | Metric: | Path:        
      10.4.0.0       /24 | 10.255.1.2      | 0       | 2 
    +--------------------+-----------------+---------+-------------+
    >> ip route del 10.3.0.0/24
    
    +--------------------+-----------------+---------+-------------+
      Network:           | NextHop:        | Metric: | Path:        
    +--------------------+-----------------+---------+-------------+
    >> ip route del 10.4.0.0/24
    
    --------------------
    Recvd UPDATE MSG...
    Marker:  ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
    Len:  48
    Type: 2
    Withdrawn Routes Len: 0
    Total Path Len: 21
    Path Attributes.
      Path Attrib: ORIGIN
        flags : 0x40
        code  : 1
        len   : 1
        origin: 2
      Path Attrib: AS_PATH
        flags : 0x50
        code  : 2
        len   : 6
        AS Path sgmnt.
          Segment type: 2
          Segment len : 2
          AS2: 2
          AS2: 1
      Path Attrib: NEXT_HOP
        flags : 0x40
        code  : 3
        len   : 4
        Next hop: 10.255.1.1
    Network Layer Reachability Information.(NLRI)
      10.255.1.0/24
    
    Warning: It does not write to the table because the AS_PATH attribute contains its own AS number.
    
    --------------------
    Recvd KEEPALIVE MSG...
    Marker:  ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
    Len:  19
    Type: 4
    
    --------------------
    Sending KEEPALIVE MSG...
    
    --------------------
    ^C
    ~~~




