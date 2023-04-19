#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <pthread.h>
#include <stdlib.h>
#include "color.c"

// Constants values for client
char *ip; // Ip address
int port; // Conection port
char *picture; // Picture to process


// https://www.geeksforgeeks.org/socket-programming-cc/
void *socketConnection()
{
    int status;
    int valread;
    int client_fd;
    struct sockaddr_in serv_addr;
    char buffer[1];

    FILE *pictureFile;

    pictureFile = fopen(picture, "rb"); // Load the file image to send

    if(!pictureFile)
    {
        bold_red();
        printf("\n---------------------------------------------------------------------\n");
        printf(" \t \t \t Can't open the file \n");
        printf("---------------------------------------------------------------------\n");
        reset();
		return NULL;
	}


    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        bold_red();
        printf("\n---------------------------------------------------------------------\n");
        printf(" \t \t \t Socket creation error \n");
        printf("---------------------------------------------------------------------\n");
        reset();
        return NULL;
    }
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
  

    // Convert IPv4 and IPv6 addresses from text to binary
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) 
    {
        bold_red();
        printf("\n---------------------------------------------------------------------\n");
        printf("\t \t Invalid address/ Address not supported \n");
        printf("---------------------------------------------------------------------\n");
        reset();
        return NULL;
    }
  
    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
    {
        bold_red();
        printf("\n---------------------------------------------------------------------\n");
        printf("\t \t \tConnection Failed \n");
        printf("---------------------------------------------------------------------\n");
        reset();
        return NULL;
    }


    while(!feof(pictureFile)){ // Send picture from client side
		fread(buffer, sizeof(char), 1, pictureFile);
		if(send(client_fd, buffer, 1, 0) == -1)
        {
			bold_red();
            printf("\n---------------------------------------------------------------------\n");
            printf(" \t \t \t Error sending file \n");
            printf("---------------------------------------------------------------------\n");
            reset();
            return NULL;
        }
	}

    char response[1024]; // Read message from server side
    valread = read(client_fd, response, 1024);
    bold_green();
    printf("\n---------------------------------------------------------------------\n");
    cyan();
    printf("Message received from buffer: %s\n", response);
    bold_green();
    printf("---------------------------------------------------------------------\n");
    reset();
  
    close(client_fd); // closing the connected socket
}

int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        bold_red();
        printf("---------------------------------------------------------------------\n");
        printf("\t \t \t Missing arguments \n");
        printf("---------------------------------------------------------------------\n");
        reset();
    }
    else
    {
        // Conf values assignation
        ip = argv[1];
        port = atoi(argv[2]);
        picture = argv[3];
        int totalThreads = atoi(argv[4]);
        int totalLoops = atoi(argv[5]);

        // Code section made to control loop and threads that were set by the user
        for(int loops = 0; loops < totalLoops; loops++)
        {
            pthread_t thread_id;
            // For each loop we need to use n threads
            for(int threads = 0; threads < totalThreads; threads++)
            {
                pthread_create(&thread_id, NULL, socketConnection, NULL);
            }
            pthread_join(thread_id, NULL);
        }
    }
    return 0;
}