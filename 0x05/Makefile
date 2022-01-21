CC = gcc
PROGRAM = mybgp
CFLAGS = -Wall -O2
OBJS = main.o bgp.o param.o print.o analyze.o
$(PROGRAM) : $(OBJS)
	$(CC) $(OBJS) -o $(PROGRAM) -lm -ljansson

main.o : main.c includes.h bgp.h param.h
	$(CC) -c $(CFLAGS) main.c		
bgp.o : bgp.c includes.h bgp.h param.h
	$(CC) -c $(CFLAGS) bgp.c
param.o : param.c includes.h bgp.h param.h
	$(CC) -c $(CFLAGS) param.c
print.o : print.c includes.h bgp.h param.h 
	$(CC) -c $(CFLAGS) print.c	
analyze.o : analyze.c includes.h bgp.h param.h 
	$(CC) -c $(CFLAGS) analyze.c	
		
clean:
	rm $(PROGRAM) $(OBJS)


