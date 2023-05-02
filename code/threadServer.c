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
#include <sys/resource.h>
#include "serversUtil.h"

#include <time.h>
#include <sys/time.h>

#define PORT 25565

int static semaphore = 1;
sem_t my_semaphore;
int static transferredBytes = 0;
double static timeExecution = 0;

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

    struct rusage usage;

    char *response = "thread";
    send(new_socket, response, strlen(response), 0);

    memset(buffer, 0, strlen(buffer));

    FILE *file;
    char indexName[20];
    char extension[20] = ".jpg";
    char fileName[30] = "files/threads/";
    sprintf(indexName, "%d", (int)pthread_self());
    strcat(fileName, indexName);
    strcat(fileName, extension);
    file = fopen(fileName, "wb");

    //---------------- Measure time execution ------------------//
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL); // Start timer

    while ((received = recv(new_socket, buffer, 1, 0)) > 0) {
        sem_wait(&my_semaphore);
        fwrite(buffer, sizeof(char), 1, file);
        transferredBytes += received;
        sem_post(&my_semaphore);
    }
    // Finish timer
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    timeExecution = (double)tval_result.tv_sec + ((double)tval_result.tv_usec) / CLOCKS_PER_SEC;

    printf("Time elapsed: %f seconds\n", timeExecution);


    //printf("Size of buffer: %d\n", transferredBytes);

    fclose(file);

    sobelFilter(fileName);

    close(new_socket);
    pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
    printf("Start server\n");
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);


    sem_init(&my_semaphore, 0, 1);

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
        printf("%d, Client connected\n", serverIndex);
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        serverIndex++;

       pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handleConnection, (void *)&new_socket) < 0)
        {
            perror("could not create thread");
            exit(EXIT_FAILURE);
        }

        pthread_detach(thread_id);
    }

    return 0;
}
