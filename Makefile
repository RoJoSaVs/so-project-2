all: build

build: # Compile all files needed to run the project
	# gcc code/sharedObjectsInit.c -o output/sharedObjectsInit
	# ./output/sharedObjectsInit
	gcc Native_Sobel/file_operations.c Native_Sobel/image_operations.c Native_Sobel/main.c -lm -o output/sobel # Sobel filter

	clear
	gcc code/client.c -o output/client -lpthread -lrt
	gcc code/Servidor_secu.c -o output/Servidor_secu
	gcc code/Cliente_secu.c -o Cliente_secu -lpthread -lrt


ip?=127.0.0.1
port?=25565
image?=itachi.jpg
threads?=1
loops?=5
client: reset sobel init # -lpthread: Require to use threads
	clear
	gcc code/client.c -o output/client -lpthread -lrt
	# ./output/client $(ip) $(port) $(image) $(threads) $(loops)
	./output/client 127.0.0.1 25565 files/img/itachi.jpg 5 1
	./output/client 127.0.0.1 8888 files/img/itachi.jpg 5 1
	./output/client 0.0.0.0 1100 files/img/itachi.jpg 5 1


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


thread_server:
	gcc code/threadServer.c -o output/threadServer
	./output/threadServer
    
numProcess?=3
preheavy:
	clear
	gcc code/preHeavyServer.c -o output/preHeavyServer -lncurses
	./output/preHeavyServer $(numProcess)


server_secu:
	./output/Servidor_secu 2


visualizer:
	clear
	# gcc code/visualizer.c -o output/visualizer
	# ./output/visualizer
	python3 code/visualizer.py


init sobel: # Define shared variables for stats
	gcc code/sharedObjectsInit.c -o output/sharedObjectsInit
	./output/sharedObjectsInit


sobel: #Compile files needed to apply sobel filter
	clear
	gcc Native_Sobel/file_operations.c Native_Sobel/image_operations.c Native_Sobel/main.c -lm -o output/sobel
	./output/sobel files/img/itachi.jpg itachi2.jpg files/heavy/
	./output/sobel files/img/itachi.jpg itachi2.jpg files/threads/



reset:
	rm -f output/*
	rm -f image.rgb
	rm -f sobel_countour.gray
	rm -f sobel_grad.gray
	rm -f files/fifo/*
	rm -f files/heavy/*
	rm -f files/threads/*
	rm -f files/preheavy/*
	echo "[]" > files/timeRequest.json
	clear

test:
	gcc code/test.c -o output/test
	./output/test

testClient: reset sobel init # -lpthread: Require to use threads
	clear
	gcc code/testClient.c -o output/testClient -lpthread -lrt
	./output/testClient 127.0.0.1 25565 files/img/itachi.jpg 50 1
