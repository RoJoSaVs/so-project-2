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

#endif