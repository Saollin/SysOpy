CC = gcc
FLAGS = -Wall
RESULT_FILE = raport.txt

all: clean_all compile_all
	echo "Raport file" > $(RESULT_FILE)
	echo "mode == 0 -> working with many files (use funciton paste)" >> $(RESULT_FILE)
	echo "mode == 1 -> working with one file" >> $(RESULT_FILE)
	
	echo "Pairs: 10, Range: 10-12, Processes: 2, Time: 1s, Mode: 0" >> $(RESULT_FILE)
	./generator 10 10 12
	./matrix list.txt 2 1 0 >> $(RESULT_FILE)
	./check list.txt 10 >> $(RESULT_FILE)
	make clean

	echo "Pairs: 10, Range: 30-40, Processes: 10, Time: 1s, Mode: 1" >> $(RESULT_FILE)
	./generator 10 30 40
	./matrix list.txt 10 1 1 >> $(RESULT_FILE)
	./check list.txt 10 >> $(RESULT_FILE)
	make clean

	echo "Pairs: 10, Range: 30-40, Processes: 10, Time: 1s, Mode: 0" >> $(RESULT_FILE)
	./generator 10 30 40
	./matrix list.txt 10 1 0 >> $(RESULT_FILE)
	./check list.txt 10 >> $(RESULT_FILE)
	make clean

	echo "Pairs: 50, Range: 20-30, Processes: 5, Time: 5s, Mode: 0" >> $(RESULT_FILE)
	./generator 50 20 30
	./matrix list.txt 5 5 0 >> $(RESULT_FILE)
	./check list.txt 50 >> $(RESULT_FILE)
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


