CC = gcc
FLAGS = -Wall -ggdb
RAPORT_FILE = raport3.txt

compile: clean
	$(CC) $(FLAGS) -o main main.c

test: compile
	./main 4 > $(RAPORT_FILE)
	./main 8 >> $(RAPORT_FILE)
	./main 11 >> $(RAPORT_FILE)
	./main 17 >> $(RAPORT_FILE)
	./main 20 >> $(RAPORT_FILE)

clean:
	rm -f main *.o