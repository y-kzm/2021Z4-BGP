#!/bin/bash
docker exec -it R1 tcpdump -nni net0 -w /tmp/in.pcap 
