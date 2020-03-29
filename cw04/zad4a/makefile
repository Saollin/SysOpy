CC = gcc
FLAGS = -Wall

compile: clean
	$(CC) $(FLAGS) -o sender sender.c 
	$(CC) $(FLAGS) -o catcher catcher.c 

test: compile 
	@./catcher kill - & 
	@./sender `pidof -s catcher` 1000 kill

	./catcher sigrt  & ./sender `pidof -s catcher` 1000 sigrt

	./catcher sigqueue  & ./sender `pidof -s catcher` 1000 sigqueue

clean:
	rm -f sender catcher *.o