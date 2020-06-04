CC = gcc
FLAGS = -Wall

all: compile test

producer:
	$(CC) $(FLAGS) -o producer producer.c 

consumer:
	$(CC) $(FLAGS) -o consumer consumer.c

tester:
	$(CC) $(FLAGS) -o tester test.c

compile: clean producer consumer tester

test:
	./tester 5
	make clean

clean:
	rm -f producer consumer tester fifo