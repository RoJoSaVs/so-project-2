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

#define AVG_STATS_SEMAPHORE_NAME "avgStatsSemaphore"
#define LIS_STATS_SEMAPHORE_NAME "loopStatsSemaphore"

sem_t *avgSemStats;
sem_t *loopSemStats;
//----------------------------------------------------------------//
//----------------------------------------------------------------//


void getSemaphore()
{
    avgSemStats = sem_open(AVG_STATS_SEMAPHORE_NAME, O_CREAT | O_RDWR, 0666, 1);
    loopSemStats = sem_open(LIS_STATS_SEMAPHORE_NAME, O_CREAT | O_RDWR, 0666, 1);

    // while(1)
    // {
    //     if((sem_trywait(avgSemStats) == -1) && (sem_trywait(loopSemStats) == -1))
    //     {
    //         break;
    //     }
    // }

    // sem_post(avgSemStats);
    // sem_post(loopSemStats);
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


        int totalRequest = totalThreads*totalLoops;
        int byteCounter = 0; // Start byte counter


        // Shared Objects initialization
        getSemaphore();

        //---------------- Measure time execution ------------------//
        struct timeval tval_before, tval_after, tval_result;
        gettimeofday(&tval_before, NULL); // Start timer

        for(int loops = 0; loops < totalRequest; loops++)
        {
            // https://www.geeksforgeeks.org/socket-programming-cc/
            int valread;
            int client_fd;
            struct sockaddr_in serv_addr;
            char buffer[1];

            memset(buffer, 0, strlen(buffer));

            FILE *pictureFile;

            pictureFile = fopen(argv[3], "rb"); // Load the file image to send

            if(!pictureFile)
            {
                bold_red();
                printf("\n---------------------------------------------------------------------\n");
                printf(" \t \t \t Can't open the file \n");
                printf("---------------------------------------------------------------------\n");
                reset();

                exit(0);
            }


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

            if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
            {
                bold_red();
                printf("\n---------------------------------------------------------------------\n");
                printf("\t \t \tConnection Failed \n");
                printf("---------------------------------------------------------------------\n");
                reset();
                exit(0);
            }
            printf("Connection\n");

            //---------------- Measure time request execution ------------------//
            struct timeval tval_beforeReq, tval_afterReq, tval_resultReq;
            gettimeofday(&tval_beforeReq, NULL); // Start timer

            while(!feof(pictureFile)){ // Send picture from client side
                fread(buffer, sizeof(char), 1, pictureFile);
                if(send(client_fd, buffer, 1, 0) == -1)
                {
                    bold_red();
                    printf("\n---------------------------------------------------------------------\n");
                    printf(" \t \t \t Error sending file \n");
                    printf("---------------------------------------------------------------------\n");
                    reset();

                    exit(0);
                }
                byteCounter++; // Increase byte counter
            }

            char response[1024]; // Read message from server side

            printf("Pre read\n");
            valread = read(client_fd, response, 1024);

            printf("Post read\n");
            // Parse response string into tokens
            char *token = strtok(response, ", []");
            char *tokens[2];

            int i = 0;
            while (token != NULL) {
                tokens[i++] = token;
                token = strtok(NULL, ", []");
            }

            // Convert token to integer
            int memoryUsage = atoi(tokens[1]);

            char *serverNameReceived = tokens[0];


            // Finish timer
            gettimeofday(&tval_afterReq, NULL);
            timersub(&tval_afterReq, &tval_beforeReq, &tval_resultReq);
            double timeExecutionReq = (double)tval_resultReq.tv_sec + ((double)tval_resultReq.tv_usec) / CLOCKS_PER_SEC;

            printf("Pre save\n");
            // Semaphore for time per request stat
            sem_wait(loopSemStats);
            printf("Post wait\n");

            save("files/timeRequest.json", serverNameReceived, loops + 1, timeExecutionReq, 0, memoryUsage, byteCounter);
            sem_post(loopSemStats);

            printf("Post save\n");

            bold_green();
            printf("\n---------------------------------------------------------------------\n");
            cyan();
            printf("Message received from buffer: %s\n", serverNameReceived);
            bold_green();
            printf("---------------------------------------------------------------------\n");
            reset();

            close(client_fd); // closing the connected socket

            memset(response, 0, strlen(response));

        }

        // Finish timer
        gettimeofday(&tval_after, NULL);
        timersub(&tval_after, &tval_before, &tval_result);
        double timeExecution = (double)tval_result.tv_sec + ((double)tval_result.tv_usec) / CLOCKS_PER_SEC;

        // Sem Stats
        sem_wait(avgSemStats);
        // save("files/stats.json", "", totalRequest, timeExecution, 0, 0, byteCounter);
        sem_post(avgSemStats);

    }
    return 0;
}