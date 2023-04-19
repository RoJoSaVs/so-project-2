all: build

build: # Compile all files needed to run the project
	gcc code/client.c -o output/client -lpthread
	gcc Servidor_secu.c -o Servidor_secu
	gcc Cliente_secu.c -o Cliente_secu -lpthread -lrt




client: # -lpthread: Require to use threads
	gcc code/client.c -o output/client -lpthread


clientExec:
	clear
	gcc code/client.c -o output/client -lpthread
	# ./output/client $(ip) $(port) $(image) $(threads) $(loops)
	./output/client 127.0.0.1 25565 itachi.jpg 1 1


server:
	clear
	gcc code/serverTest.c -o output/serverTest
	./output/serverTest

server_secu:
	./Servidor_secu 2

client_secu:
	./Cliente_secu 0.0.0.0 1100 test.jpg 7 7

reset:
	rm output/*
	clear

test:
	gcc code/test.c -o output/test
	./output/test