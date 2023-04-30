/*
 * This code creates a server that listens for incoming connections
 * and spawns a new thread to handle each connection. Once the thread
 * is finished processing the request, it exits using pthread_exit,
 * effectively ending the thread's execution. The main process continues
 * to listen for incoming connections and spawn new threads, as long
 * as it is running.
 */

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#include "serversUtil.h"

#define PORT 25565

//int static semaphore = 1;
//sem_t my_semaphore;

pthread_mutex_t semaphoreWriteRec = PTHREAD_MUTEX_INITIALIZER;


void sobelFilter(char fileName[30])
{
    char command[100] = "./output/sobel ";
    strcat(command, fileName);
    strcat(command, " ");
    strcat(command, fileName);

    strcat(command, " files/threads/");
    system(command);
}

void *handleConnection(void *arg)
{
    int new_socket = *((int *)arg);
    char buffer[1];
    int received = -1;

    char *response = malloc(strlen("thread") + 1); // +1 for null terminator
    strcpy(response, "thread");
    send(new_socket, response, strlen(response), 0);

    memset(response, 0, strlen(response));

    //bzero(response, strlen(response));

    FILE *file;
    char indexName[20];
    char extension[20] = ".jpg";
    char fileName[30] = "files/threads/";
    sprintf(indexName, "%d", (int)pthread_self());
    strcat(fileName, indexName);
    strcat(fileName, extension);
    file = fopen(fileName, "wb");

    // Rojo = 0, Verde = 1
    while ((received = recv(new_socket, buffer, 1, 0)) > 0) {

        pthread_mutex_lock(&semaphoreWriteRec);
        fwrite(buffer, sizeof(char), 1, file);
        pthread_mutex_unlock(&semaphoreWriteRec);
    }

    fclose(file);
    close(new_socket);

    //sobelFilter(fileName);

    //pthread_exit(NULL);
    pthread_detach(pthread_self());
}


int main(int argc, char *argv[])
{
    printf("Start server\n");
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // sem_init(&my_semaphore, 0, 1);

    printf("Creating socket...\n");
    validateSocketCreation(&server_fd);

    printf("Setting socket...\n");
    validateSocketSetting(&server_fd, &opt);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    printf("Binding socket...\n");
    validateBind(&server_fd, &address);

    printf("Listening to socket...\n");
    validateSocketListen(&server_fd);

    int serverIndex = 0;

    while (1)
    {
        serverIndex++;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("%d, Client connected\n", serverIndex);

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handleConnection, (void *)&new_socket) < 0)
        {
            perror("could not create thread");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
