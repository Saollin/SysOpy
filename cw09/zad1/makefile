CC = gcc
FLAGS = -Wall 
# -Wextra -pedantic -Werror -std=c11 -pthread

all: compile

main:
	$(CC) $(FLAGS) -o main main.c -lm -lpthread

compile: clean main

test: 
	./main 2 12
	./main 3 20  

clean:
	rm -f main