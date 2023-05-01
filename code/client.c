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


void getSemaphore()
{
    semStats = sem_open(STATS_SEMAPHORE_NAME, O_CREAT | O_RDWR, 0666, 1);
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
        char serverName[1024];


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

            memset(response, 0, strlen(response));

            valread = read(client_fd, response, 1024);

            bold_green();
            printf("\n---------------------------------------------------------------------\n");
            cyan();
            printf("Message received from buffer: %s\n", response);
            bold_green();
            printf("---------------------------------------------------------------------\n");
            reset();
            strcpy(serverName, response);


            close(client_fd); // closing the connected socket
        }

        // Finish timer
        gettimeofday(&tval_after, NULL);
        timersub(&tval_after, &tval_before, &tval_result);
        double timeExecution = (double)tval_result.tv_sec + ((double)tval_result.tv_usec) / CLOCKS_PER_SEC;

        // Sem Stats
        save(serverName, totalRequest, timeExecution, 0, 0, byteCounter);
        
        
    }
    return 0;
}