CC = gcc
FLAGS = -Wall
RAPORT_FILE = raport4a.txt

compile: clean
	$(CC) $(FLAGS) -o sender sender.c 
	$(CC) $(FLAGS) -o catcher catcher.c 

test: 
	echo "" > $(RAPORT_FILE)
	make test_signals signals=1000
	make test_signals signals=10000
	make test_signals signals=1000000	

signals = 1000
test_signals: compile
	
	echo "$(signals) signals, kill" >> $(RAPORT_FILE)
	./catcher kill >> $(RAPORT_FILE) & ./sender `pidof -s catcher` $(signals) kill >> $(RAPORT_FILE)

	echo "$(signals) signals, sigrt" >> $(RAPORT_FILE)
	./catcher sigrt >> $(RAPORT_FILE)  & ./sender `pidof -s catcher` $(signals) sigrt >> $(RAPORT_FILE)

	echo "$(signals) signals, sigqueue" >> $(RAPORT_FILE)
	./catcher sigqueue >> $(RAPORT_FILE) & ./sender `pidof -s catcher` $(signals) sigqueue >> $(RAPORT_FILE)


clean:
	rm -f sender catcher *.o