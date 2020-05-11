CC = gcc
FLAGS = -Wall 
# -Wextra -pedantic -Werror -std=c11 -pthread

all: clean compile

main:
	$(CC) $(FLAGS) -o main main.c -lm -lpthread

compile: main

clean:
	rm -f main