CC = gcc
PROGRAM = mybgp
CFLAGS = -Wall -O2

OBJS = main.o bgp.o 
$(PROGRAM) : $(OBJS)
	$(CC) $(OBJS) -o $(PROGRAM)
	
main.o : main.c includes.h bgp.h param.h
	$(CC) -c $(CFLAGS) main.c
		
bgp.o : bgp.c includes.h bgp.h param.h
	$(CC) -c $(CFLAGS) bgp.c
		
clean:
	rm $(PROGRAM) $(OBJS)