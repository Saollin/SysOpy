CC = gcc
FLAGS = -Wall 

all: compile

server:
	$(CC) $(FLAGS) -o server server.c -lm -lpthread

client:
	$(CC) $(FLAGS) -o client client.c

compile: clean server client

runServer: server
	./server 8000 .socket

runClientWeb: 
	./client "Greg" web 127.0.0.1 8000

runClientUnix: 
	./client "Piotr" unix .socket 

clean:
	rm -f server client