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

/*
 * Apply Sobel filter to image
 */
void sobelFilter(char fileName[30])
{
    char command[100] = "./output/sobel ";
    strcat(command, fileName);
    strcat(command, " ");
    strcat(command, fileName);

    system(command);
}

/*
 * Called when a new thread is created to handle an incoming connection.
 * Takes a single argument, which is a pointer to the file descriptor of the
 * new socket that was created when the connection was accepted.
 */
void *handleConnection(void *arg)
{
    int new_socket = *((int *)arg);
    char buffer[1];
    int received = -1;

    // Connection accepted
    char *response = "OK";
    send(new_socket, response, strlen(response), 0);

    // File to receive the incoming data from the client, created based on the thread ID,
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

    // Close socket and exit thread
    close(new_socket);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor (IPv4, stream-oriented connection, OS chooses protocol)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Allow the socket to be reused immediately after it is closed
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;

    // Allows the socket to accept connections from any IP address.
    address.sin_addr.s_addr = INADDR_ANY;

    // Converted to network byte order
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the PORT, binds the socket to a specific IP
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen forncoming connections. The server_fd parameter is the socket file
    // descriptor, second parameter is the maximum number of queued connections
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Waits for an incoming connection
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
