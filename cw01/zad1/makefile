CC = gcc
FLAGS = -Wall
LIB_NAME = libcw01
SOURCE_NAME = libcw01

static:
	$(CC) $(FLAGS) -c $(SOURCE_NAME).c -o $(LIB_NAME).o
	ar rcs $(LIB_NAME).a $(LIB_NAME).o
	make clear

shared:
	$(CC) -fPIC $(FLAGS) -c $(SOURCE_NAME).c -o $(LIB_NAME).o
	$(CC) -fPIC $(FLAGS) -shared -o $(LIB_NAME).so $(LIB_NAME).o
	make clear

all:
	make static
	make shared

clear:
	rm -f *.o

clear_all:
	rm -f *.so
	rm -f *.a
	