CC = gcc
FLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -pthread

all: compile

server:
	$(CC) $(FLAGS) -o server server.c -lrt

client:
	$(CC) $(FLAGS) -o client client.c -lrt

compile: clean server client

clean:
	rm -f server client