all: build

build: # Compile all files needed to run the project
	# gcc code/sharedObjectsInit.c -o sobel/sharedObjectsInit
	# ./sobel/sharedObjectsInit
	gcc Native_Sobel/file_operations.c Native_Sobel/image_operations.c Native_Sobel/main.c -lm -o output/sobel # Sobel filter

	clear
	gcc code/client.c -o output/client -lpthread -lrt
	gcc code/Servidor_secu.c -o output/Servidor_secu
	gcc code/threadServer.c -o output/threadServer
	gcc code/Cliente_secu.c -o Cliente_secu -lpthread -lrt


client: # -lpthread: Require to use threads
	clear
	gcc code/client.c -o output/client -lpthread -lrt
	# ./output/client $(ip) $(port) $(image) $(threads) $(loops)
	./output/client 127.0.0.1 25565 files/img/itachi.jpg 500 1


server:
	# clear
	# gcc code/serverTest.c -o sobel/serverTest
	# ./sobel/serverTest
	clear
	gcc code/Servidor_secu.c -o output/Servidor_secu
	./output/Servidor_secu 0

heavy:
	clear
	gcc code/heavyServer.c -o output/heavyServer
	./output/heavyServer


server_secu:
	./output/Servidor_secu 2

thread_server:
	gcc code/threadServer.c -o output/threadServer
	./output/threadServer


visualizer:
	clear
	# gcc code/visualizer.c -o sobel/visualizer
	# ./sobel/visualizer
	python3 code/visualizer.py


init: # Define shared variables for stats
	gcc code/sharedObjectsInit.c -o output/sharedObjectsInit
	./output/sharedObjectsInit


sobel: #Compile files needed to apply sobel filter
	clear
	gcc Native_Sobel/file_operations.c Native_Sobel/image_operations.c Native_Sobel/main.c -lm -o output/sobel
	./output/sobel files/img/itachi.jpg itachi2.jpg files/threads/

reset:
	rm output/*
	rm files/threads/*
	rm image.rgb
	rm sobel_countour.gray
	rm sobel_grad.gray
	# rm files/fifo/*
	# rm files/heavy/*
	# rm files/preheavy/*
	clear

test:
	gcc code/test.c -o output/test
	./output/test