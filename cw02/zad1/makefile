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
	echo "" > $(RAPORT_FILE)
	
	for s in 1 4 512 1024 4096 8192; do \
		for n in 200 4000; do \
			./$(NAME) generate dane $$n $$s; \
			echo "Copying $$n records with size $$s bytes with lib functions" >> $(RAPORT_FILE); \
			./$(NAME) copy dane dane.$$n.$$s.lib.txt $$n $$s lib >> $(RAPORT_FILE); \
			echo "Copying $$n records with size $$s bytes with sys functions" >> $(RAPORT_FILE); \
			./$(NAME) copy dane dane.$$n.$$s.sys.txt $$n $$s sys >> $(RAPORT_FILE); \
		done; \
	done \

	# for s in 1 4 512 1024 4096 8192; do \
	# 	for n in 200 4000; do \
	# 		echo "Sorting $$n records with size $$s bytes with lib functions" >> $(RAPORT_FILE); \
	# 		./$(NAME) sort dane.$$n.$$s.lib $$n $$s lib >> $(RAPORT_FILE); \
	# 		echo "Sorting $$n records with size $$s bytes with sys functions" >> $(RAPORT_FILE); \
	# 		./$(NAME) sort dane.$$n.$$s.sys $$n $$s sys >> $(RAPORT_FILE); \
	# 	done; \
	# done

	rm -rf dane*

clean:
	rm -rf dane*
	rm -f *.o
	rm -f main