CC = gcc
FLAGS = -Wall -ggdb
RAPORT_FILE = raport2.txt

all: compile
	make test > $(RAPORT_FILE)

compile: clean
	$(CC) $(FLAGS) -o main_fork main_fork.c
	$(CC) $(FLAGS) -o main_exec main_exec.c
	$(CC) $(FLAGS) -o exec exec.c

test: compile
	#Testing program witk function fork()" 
	./main_fork ignore 

	./main_fork handler

	./main_fork mask

	./main_fork pending

	
	#Testing program witk function exec() 
	
	./main_exec ignore

	./main_exec mask

	./main_exec pending

clean:
	rm -f main_fork main_exec exec *.o

test_handler:
	./main_fork ignore > $(RAPORT_FILE)