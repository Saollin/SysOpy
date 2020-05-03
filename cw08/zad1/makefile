CC = gcc
FLAGS = -Wall 
# -Wextra -pedantic -Werror -std=c11 -pthread

all: clean compile test

main:
	# $(CC) $(FLAGS) -o operations -c operations.c
	$(CC) $(FLAGS)  -o main main.c operations.c -lm -lpthread

compile: main

test:
	./main 2 sign lena.ascii.pgm 2_thread_sign
	./main 8 block lena.ascii.pgm 2_thread_block 

clean:
	rm -f main