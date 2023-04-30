#ifndef MY_LIBRARY_H
#define MY_LIBRARY_H

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

extern void validateSocketCreation(int *server_fd){
    if (((*server_fd) = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
}

extern void validateSocketSetting(int *server_fd, int *opt){
    if (setsockopt((*server_fd), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}

int validateBind(int *server_fd, struct sockaddr_in* address) {
    if (bind(*server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    return 0;
}

extern void validateSocketListen(int *server_fd){
    if (listen((*server_fd), 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}


#endif