CC = gcc
FLAGS = -Wall
RESULT_FILE = raport3.txt

all: clean_all compile_all
	echo "Raport file" > $(RESULT_FILE)
	echo "mode == 0 -> working with many files (use function paste)" >> $(RESULT_FILE)
	echo "mode == 1 -> working with one file" >> $(RESULT_FILE)
	
	echo "Pairs: 10, Range: 10-12, Processes: 2, Time: 1s, Mode: 0, Max CPU time: 1s, Max memory: 10 MB" >> $(RESULT_FILE)
	./generator 20 10 12
	./matrix list.txt 2 1 0 1 10 >> $(RESULT_FILE)
	./check list.txt >> $(RESULT_FILE)
	make clean

	echo "Pairs: 10, Range: 30-40, Processes: 2, Time: 2s, Mode: 1, Max CPU time: 1s, Max memory: 5 MB" >> $(RESULT_FILE)
	./generator 10 30 40
	./matrix list.txt 2 2 1 1 5 >> $(RESULT_FILE)
	./check list.txt >> $(RESULT_FILE)
	make clean

compile_all:
	$(CC) $(FLAGS) -o generator generator.c
	$(CC) $(FLAGS) -o matrix matrix.c
	$(CC) $(FLAGS) -o check check.c


clean:
	rm -f m*.txt result* list.txt
	
clean_all: clean
	rm -f matrix generator check 

debug:
	$(CC) $(FLAGS) -g generator generator.c
	gdb ./generator 100 20 30


