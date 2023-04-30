#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>

#include <pthread.h>
#include <stdlib.h>

#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>

#include "color.c"
#include "statsStruct.c"
#include "json_read_write.c"


//----------------------------------------------------------------//
//---------------------- Constants Values ------------------------//
//----------------------------------------------------------------//
#define FIFO_ID "fifo"
#define HEAVY_ID "heavy"
#define THREAD_ID "thread"
#define PREHEAVY_ID "preheavy"

char *ip; // Ip address
int port; // Conection port
// char *picture; // Picture to process

#define STATS_SEMAPHORE_NAME "statsSemaphore"
#define SHARED_STATS_MEMORY "sharedStats"

sem_t *semStats;
//----------------------------------------------------------------//
//----------------------------------------------------------------//

static int createdLoops = 0;

// Define a structure to hold the arguments for the thread function
struct thread_args {
    int totalLoops;
    char *image;
};

pthread_mutex_t semaphore = PTHREAD_MUTEX_INITIALIZER;

void *sendImages(void *args){
    int byteCounter = 0; // Start byte counter
    char serverName[1024];

    struct thread_args *args_ptr = (struct thread_args *) args;
    int loops = args_ptr->totalLoops;
    char *img = args_ptr->image;

    int client_fd;
    struct sockaddr_in serv_addr;

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        bold_red();
        printf("\n---------------------------------------------------------------------\n");
        printf(" \t \t \t Socket creation error \n");
        printf("---------------------------------------------------------------------\n");
        reset();

        exit(0);
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

        exit(0);
    }

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0)
    {
        bold_red();
        printf("\n---------------------------------------------------------------------\n");
        printf("\t \t \tConnection Failed \n");
        printf("---------------------------------------------------------------------\n");
        reset();

        exit(0);
    }

    for(int loop = 0; loop < loops; loop++)
    {
        // https://www.geeksforgeeks.org/socket-programming-cc/
        createdLoops++;

        FILE *pictureFile;
        pictureFile = fopen(img, "rb"); // Load the file image to send
        if(pictureFile == NULL)
        {
            bold_red();
            printf("\n---------------------------------------------------------------------\n");
            printf(" \t \t \t Can't open the file \n");
            printf("---------------------------------------------------------------------\n");
            reset();

            exit(0);
        }

        printf("%d sending file\n", loop);
        char buffer[1];
        while(!feof(pictureFile)){ // Send picture from client side
            fread(buffer, sizeof(char), 1, pictureFile);
            int sentResult =  send(client_fd, buffer, 1, 0);
            if (sentResult  < 0)
            {
                perror(" \t \t \t Error sending file \n");
                exit(0);
            }
            else if (sentResult == 0) {
                printf("%d file sent successfully\n", loop);
            }
            byteCounter++; // Increase byte counter
        }
        memset(buffer, 0, sizeof(buffer));
        fclose(pictureFile);

        char response[1024]; // Read message from server side
        if(read(client_fd, response, 1024) < 0){
            perror("read failed");
            exit(EXIT_FAILURE);
        }

        bold_green();
        printf("\n---------------------------------------------------------------------\n");
        cyan();
        printf("%d Message received from buffer: %s\n", loop, response);
        bold_green();
        printf("---------------------------------------------------------------------\n");
        reset();
        strcpy(serverName, response);

        memset(response, 0, sizeof(response));
    }

    close(client_fd); // closing the connected socket
    pthread_exit(NULL);
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
        int totalThreads = atoi(argv[4]);
        int totalLoops = atoi(argv[5]);

        struct thread_args args = { totalLoops, argv[3]};

        printf("Creating %d threads\n", totalThreads);
        int createdThreads = 0;
        for (int i = 0; i < totalThreads; ++i) {
            pthread_t thread_id;
            if (pthread_create(&thread_id, NULL, sendImages, (void *)&args) != 0)
            {
                perror("could not create thread");
                exit(EXIT_FAILURE);
            }
            createdThreads++;
        }
        printf("Created %d threads\n", createdThreads);

        //---------------- Measure time execution ------------------//
        struct timeval tval_before, tval_after, tval_result;
        gettimeofday(&tval_before, NULL); // Start timer

        // Finish timer
        gettimeofday(&tval_after, NULL);
        timersub(&tval_after, &tval_before, &tval_result);
        double timeExecution = (double)tval_result.tv_sec + ((double)tval_result.tv_usec) / CLOCKS_PER_SEC;

        // Sem Stats
        //save(serverName, totalRequest, timeExecution, 0, 0, byteCounter);

        while (1){
            sleep(5);
            printf("Created %d loops\n", createdLoops);
        }

    }
    return 0;
}