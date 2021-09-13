FROM ubuntu:focal
RUN apt -y update
RUN apt -y install iproute2 iputils-ping net-tools build-essential tcpdump
RUN apt -y install libjansson-dev

WORKDIR /work
COPY ./Makefile /work/Makefile
COPY ./main.c   /work/main.c
COPY ./bgp.c    /work/bgp.c
COPY ./param.c  /work/param.c
COPY ./print.c  /work/print.c

COPY ./bgp.h      /work/bgp.h
COPY ./includes.h /work/includes.h
COPY ./param.h    /work/param.h

COPY ./conf.json  /work/conf.json

RUN make mybgp
CMD ["/bin/bash"]

