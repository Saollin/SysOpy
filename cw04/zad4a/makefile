CC = gcc
FLAGS = -Wall
RAPORT_FILE = raport4a.txt

compile: clean
	$(CC) $(FLAGS) -o sender sender.c 
	$(CC) $(FLAGS) -o catcher catcher.c 

test: compile
	echo "1000 signals, kill" > $(RAPORT_FILE)
	./catcher kill - & ./sender `pidof -s catcher` 1000 kill >> $(RAPORT_FILE)

	echo "1000 signals, sigrt" >> $(RAPORT_FILE)
	./catcher sigrt  & ./sender `pidof -s catcher` 1000 sigrt >> $(RAPORT_FILE)

	echo "1000 signals, sigqueue" >> $(RAPORT_FILE)
	./catcher sigqueue  & ./sender `pidof -s catcher` 1000 sigqueue >> $(RAPORT_FILE)

clean:
	rm -f sender catcher *.o