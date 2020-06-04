CC = gcc
FLAGS = -Wall 
IMAGE = test.ascii.pgm
MENSURATION_FILE = Times.txt
OPTION = sign
EXTENSION = .ascii.pgm

all: clean compile tests

main:
	$(CC) $(FLAGS)  -o main main.c operations.c -lm -lpthread

compile: main

tests:
	echo "Sign test" > $(MENSURATION_FILE)
	make test OPTION=sign
	echo "Block test" >> $(MENSURATION_FILE)
	make test OPTION=block
	echo "Interleaved test" >> $(MENSURATION_FILE)
	make test OPTION=interleaved 
	make compare

test:
	./main 1 $(OPTION) $(IMAGE) 1_thread_$(OPTION)$(EXTENSION) >> $(MENSURATION_FILE)
	./main 2 $(OPTION) $(IMAGE) 2_thread_$(OPTION)$(EXTENSION) >> $(MENSURATION_FILE)
	./main 4 $(OPTION) $(IMAGE) 4_thread_$(OPTION)$(EXTENSION) >> $(MENSURATION_FILE)
	./main 8 $(OPTION) $(IMAGE) 8_thread_$(OPTION)$(EXTENSION) >> $(MENSURATION_FILE)

# all result files should be the same - no difference
compare:
	./compare.sh

clean:
	rm -f main