CC = gcc
FLAGS = -Wall

test:
	make nftw
	echo "nftw implementation"
	
	find ./testFile -mtime -1
	./main ./testFile -mtime -1
	
	find ./testFile -maxdepth 1 -mtime -1
	./main ./testFile -maxdepth 1 -mtime -1
	
	find .. -maxdepth 2 -atime 1
	./main .. -maxdepth 2 -atime 1

	make dir
	echo "dir implementation"
	
	find ./testFile -mtime -1
	./main ./testFile -mtime -1
	
	find ./testFile -maxdepth 1 -mtime -1
	./main ./testFile -maxdepth 1 -mtime -1
	
	find .. -maxdepth 2 -atime 1
	./main .. -maxdepth 2 -atime 1

	make clean
	
nftw: clean
	$(CC) $(FLAGS) -o main nftwFind.c

dir: clean
	$(CC) $(FLAGS) -o main dirFind.c

clean: 
	rm -f *.o main