CC = gcc
FLAGS = -Wall

all: compile test

compile:
	$(CC) $(FLAGS) -o main main.c 

test:
	./main test.txt
	make clean

clean:
	rm main