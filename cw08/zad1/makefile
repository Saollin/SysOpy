CC = gcc
FLAGS = -Wall 
IMAGE = test.ascii.pgm
MENSURATION_FILE = Times.txt
OPTION = sign
EXTENSION = .ascii.pgm

all: compile tests

main:
	# $(CC) $(FLAGS) -o operations -c operations.c
	$(CC) $(FLAGS)  -o main main.c operations.c -lm -lpthread

compile: main

tests:
	echo "" > $(MENSURATION_FILE)
	make test OPTION=sign
	make test OPTION=block
	make test OPTION=interleaved 
	make compare
	make clean

test:
	./main 1 $(OPTION) $(IMAGE) 1_thread_$(OPTION)$(EXTENSION) >> $(MENSURATION_FILE)
	./main 2 $(OPTION) $(IMAGE) 2_thread_$(OPTION)$(EXTENSION) >> $(MENSURATION_FILE)
	./main 4 $(OPTION) $(IMAGE) 4_thread_$(OPTION)$(EXTENSION) >> $(MENSURATION_FILE)
	./main 8 $(OPTION) $(IMAGE) 8_thread_$(OPTION)$(EXTENSION) >> $(MENSURATION_FILE)

# all result files should be the same
compare:
	./compare.sh

clean:
	rm -f main