CC = gcc
FLAGS = -Wall 
# -Wextra -pedantic -Werror -std=c11 -pthread

all: clean compile

maker:
	$(CC) $(FLAGS) -o maker maker.c worker_common.c operations.c

packer:
	$(CC) $(FLAGS) -o packer packer.c worker_common.c operations.c

sender:
	$(CC) $(FLAGS) -o sender sender.c worker_common.c operations.c

main:
	$(CC) $(FLAGS) -o main main.c operations.c

compile: maker packer sender main

clean:
	rm -f maker packer sender main