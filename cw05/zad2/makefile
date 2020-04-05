CC = gcc
FLAGS = -Wall

all: compile test

compile:
	$(CC) $(FLAGS) -o main main.c 

test:
	./main f.txt
	make clean

clean:
	rm main