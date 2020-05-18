CC = gcc
FLAGS = -Wall 
# -Wextra -pedantic -Werror -std=c11 -pthread

all: compile

server:
	$(CC) $(FLAGS) -o server server.c -lm -lpthread

client:
	$(CC) $(FLAGS) -o client client.c

compile: clean server client

runServer: server
	./server 8000 .socket

name="Greg"
runClientWeb: client
	./client $(name) web 127.0.0.1 8000

runClientUnix: client
	./client $(name) unix .socket 

clean:
	rm -f server client