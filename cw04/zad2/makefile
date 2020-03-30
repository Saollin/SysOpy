CC = gcc
FLAGS = -Wall
RAPORT_FILE = raport2.txt

compile: clean
	$(CC) $(FLAGS) -o main_fork main_fork.c
	$(CC) $(FLAGS) -o main_exec main_exec.c
	$(CC) $(FLAGS) -o exec exec.c

test: compile
	echo "Testing program witk function fork() \n" > $(RAPORT_FILE)
	echo "\nfork ignore" >> $(RAPORT_FILE)
	./main_fork ignore >> $(RAPORT_FILE)

	echo "\nfork handler" >> $(RAPORT_FILE)
	./main_fork handler >> $(RAPORT_FILE)

	echo "\nfork mask" >> $(RAPORT_FILE)
	./main_fork mask >> $(RAPORT_FILE)

	echo "\nfork pending" >> $(RAPORT_FILE)
	./main_fork pending >> $(RAPORT_FILE)

	echo "Testing program witk function exec()" >> $(RAPORT_FILE)
	
	echo "\nexec ignore" >> $(RAPORT_FILE)
	./main_exec ignore >> $(RAPORT_FILE)

	echo "\nexec mask" >> $(RAPORT_FILE)
	./main_exec mask >> $(RAPORT_FILE)

	echo "\nexec pending" >> $(RAPORT_FILE)
	./main_exec pending >> $(RAPORT_FILE)

clean:
	rm -f main_fork main_exec exec *.o