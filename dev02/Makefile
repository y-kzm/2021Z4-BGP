CC = gcc
PROGRAM = socket
CFLAGS = -Wall -O2

OBJS = main.o server.o client.o
$(PROGRAM) : $(OBJS)
	$(CC) $(OBJS) -o $(PROGRAM)
	
main.o : main.c includes.h 
	$(CC) -c $(CFLAGS) main.c
		
server.o : server.c includes.h
	$(CC) -c $(CFLAGS) server.c
		
client.o : client.c includes.h
	$(CC) -c $(CFLAGS) client.c
			
clean:
	rm $(PROGRAM) $(OBJS)
