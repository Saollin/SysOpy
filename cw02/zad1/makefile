CC = gcc
FLAGS = -Wall
RAPORT_FILE = "wyniki.txt"
NAME = main
REC = 50
BYT = 10

all: test clean

main:
		$(CC) $(FLAGS) -o $(NAME) main.c

test: 
	make main
	rm $(RAPORT_FILE)
	echo "" > $(RAPORT_FILE)
	
	for s in 1 4 512 1024 4096 8192; do \
		for n in 16000 32000; do \
			./$(NAME) generate dane $$n $$s; \
			echo "Copying $$n records with size $$s bytes with lib functions" >> $(RAPORT_FILE); \
			./$(NAME) copy dane dane.$$n.$$s.lib $$n $$s lib >> $(RAPORT_FILE); \
			echo "Copying $$n records with size $$s bytes with sys functions" >> $(RAPORT_FILE); \
			./$(NAME) copy dane dane.$$n.$$s.sys $$n $$s sys >> $(RAPORT_FILE); \
		done; \
	done \

	for s in 1 4 512 1024 4096 8192; do \
		for n in 16000 32000; do \
			echo "Sorting $$n records with size $$s bytes with lib functions" >> $(RAPORT_FILE); \
			./$(NAME) sort dane.$$n.$$s.lib $$n $$s lib >> $(RAPORT_FILE); \
			echo "Sorting $$n records with size $$s bytes with sys functions" >> $(RAPORT_FILE); \
			./$(NAME) sort dane.$$n.$$s.sys $$n $$s sys >> $(RAPORT_FILE); \
		done; \
	done
	make clean_dane

small_test:
	make clean
	make main
	./main generate dane $(REC) $(BYT)
	./main copy dane dane2 $(REC) $(BYT) lib
	./main copy dane dane3 $(REC) $(BYT) sys
	./main sort dane $(REC) $(BYT) sys

clean_dane:
	rm -rf dane*

clean: clean_dane
	rm -f *.o
	rm -f main