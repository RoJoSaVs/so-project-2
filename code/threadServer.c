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

#define PORT 25565

void sobelFilter(char fileName[30])
{
    char command[100] = "./output/sobel ";
    strcat(command, fileName);
    strcat(command, " ");
    strcat(command, fileName);

    system(command);
}

void *handleConnection(void *arg)
{
    int new_socket = *((int *)arg);
    char buffer[1];
    int received = -1;

    char *response = "OK";
    send(new_socket, response, strlen(response), 0);

    FILE *file;
    char indexName[20];
    char extension[20] = ".jpg";
    char fileName[30] = "files/threads";
    sprintf(indexName, "%d", (int)pthread_self());
    strcat(fileName, indexName);
    strcat(fileName, extension);
    file = fopen(fileName, "wb");

    while ((received = recv(new_socket, buffer, 1, 0)) > 0)
    {
        fwrite(buffer, sizeof(char), 1, file);
    }

    fclose(file);

    sobelFilter(fileName);

    close(new_socket);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the PORT
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

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
