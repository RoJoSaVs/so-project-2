all: build

build: # Compile all files needed to run the project
	# gcc code/sharedObjectsInit.c -o output/sharedObjectsInit
	# ./output/sharedObjectsInit

	clear
	gcc code/client.c -o output/client -lpthread
	gcc Servidor_secu.c -o Servidor_secu
	gcc Cliente_secu.c -o Cliente_secu -lpthread -lrt


client: # -lpthread: Require to use threads
	clear
	gcc code/client.c -o output/client -lpthread
	# ./output/client $(ip) $(port) $(image) $(threads) $(loops)
	./output/client 127.0.0.1 25565 code/test.jpg 1 2


server:
	# clear
	# gcc code/serverTest.c -o output/serverTest
	# ./output/serverTest
	clear
	gcc code/Servidor_secu.c -o output/Servidor_secu
	./output/Servidor_secu 0

heavy:
	clear
	gcc code/heavyServer.c -o output/heavyServer
	./output/heavyServer


server_secu:
	./Servidor_secu 2


visualizer:
	clear
	# gcc code/visualizer.c -o output/visualizer
	# ./output/visualizer
	python code/visualizer.py


init: # Define shared variables for stats
	gcc code/sharedObjectsInit.c -o output/sharedObjectsInit
	./output/sharedObjectsInit
	

reset:
	rm output/*
	# rm files/fifo/*
	rm files/heavy/*
	# rm files/threads/*
	# rm files/preheavy/*
	clear

test:
	gcc code/test.c -o output/test
	./output/test