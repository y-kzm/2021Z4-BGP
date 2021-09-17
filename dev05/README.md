# 自作BGPとFRR間でメッセージのやりとりを行う.
* "sh bgp ipv4 unicast"みたいなのを作る.

---
### Overall composition: 
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
* analyze.c
    - analyze_"MSG,PA"()
        - process_recv"MSG"() で呼ばれる.
        - 受信したBGP MSG を構造体に格納.
        - print関数を呼ぶ.
* print.c
    - print_"MSG,PA"()
        - analyze関数で呼ばれる.
        - 格納した構造体メンバを表示する.
* param.c
    - jsonファイルの内容を構造体へ格納.


