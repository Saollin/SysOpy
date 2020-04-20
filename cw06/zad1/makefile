CC = gcc
FLAGS = -Wall

all: compile

server:
	$(CC) $(FLAGS) -o server server.c 

client:
	$(CC) $(FLAGS) -o client client.c -lrt

compile: clean server client

clean:
	rm -f server client