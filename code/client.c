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

//----------------------------------------------------------------//
//---------------------- Constants Values ------------------------//
//----------------------------------------------------------------//
#define FIFO_ID "fifo"
#define HEAVY_ID "heavy"
#define THREAD_ID "thread"
#define PREHEAVY_ID "preheavy"

char *ip; // Ip address
int port; // Conection port
char *picture; // Picture to process

#define STATS_SEMAPHORE_NAME "statsSemaphore"
#define SHARED_STATS_MEMORY "sharedStats"

sem_t *semStats;
struct statsStruct *stats;
//----------------------------------------------------------------//
//----------------------------------------------------------------//


void getSemaphore()
{
    semStats = sem_open(STATS_SEMAPHORE_NAME, O_CREAT | O_RDWR, 0666, 1);
}

void getSharedStats()
{
    int shm_stats;
    shm_stats = shm_open(SHARED_STATS_MEMORY, O_CREAT | O_RDWR, 0666); // Shared memory for stats with id "shareStats"
    stats = mmap(0, sizeof(struct statsStruct), PROT_READ | PROT_WRITE, MAP_SHARED, shm_stats, 0);
}


void editStats(char *serverId, double timeExecution, int memoryConsumption, double fourStat)
{
    if(strcmp(serverId, FIFO_ID) == 0)
    {
        sem_wait(semStats);
        stats->totalRequestSequential++;
        stats->timeExecutionSequential += timeExecution;
        stats->memoryConsumptionSequential += memoryConsumption;
        stats->fourStatSequential += fourStat;
        sem_post(semStats);
    }

    else if(strcmp(serverId, HEAVY_ID) == 0)
    {
        sem_wait(semStats);
        stats->totalRequestHeavyProcess++;
        stats->timeExecutionHeavyProcess += timeExecution;
        stats->memoryConsumptionHeavyProcess += memoryConsumption;
        stats->fourStatHeavyProcess += fourStat;
        sem_post(semStats);
    }

    else if(strcmp(serverId, THREAD_ID) == 0)
    {
        sem_wait(semStats);
        stats->totalRequestThreads++;
        stats->timeExecutionThreads += timeExecution;
        stats->memoryConsumptionThreads += memoryConsumption;
        stats->fourStatThreads += fourStat;
        sem_post(semStats);
    }

    else if(strcmp(serverId, PREHEAVY_ID) == 0)
    {
        sem_wait(semStats);
        stats->totalRequestPreHeavy++;
        stats->timeExecutionPreHeavy += timeExecution;
        stats->memoryConsumptionPreHeavy += memoryConsumption;
        stats->fourStatPreHeavy += fourStat;
        sem_post(semStats);
    }
}

// https://www.geeksforgeeks.org/socket-programming-cc/
void socketConnection()
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

        exit(0);
		// return NULL;
	}


    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        bold_red();
        printf("\n---------------------------------------------------------------------\n");
        printf(" \t \t \t Socket creation error \n");
        printf("---------------------------------------------------------------------\n");
        reset();

        exit(0);
        // return NULL;
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
        // return NULL;
    }
  
    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
    {
        bold_red();
        printf("\n---------------------------------------------------------------------\n");
        printf("\t \t \tConnection Failed \n");
        printf("---------------------------------------------------------------------\n");
        reset();

        exit(0);
        // return NULL;
    }

    int byteCounter = 0; // Start byte counter

    //---------------- Measure time execution ------------------//
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL); // Start timer

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
            // return NULL;
        }
        byteCounter++; // Increase byte counter
	}

    char response[1024]; // Read message from server side
    valread = read(client_fd, response, 1024);
    
    // Finish timer
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    double timeExecution = (double)tval_result.tv_sec + ((double)tval_result.tv_usec) / CLOCKS_PER_SEC;

    editStats(response, timeExecution, byteCounter, 0);

    bold_green();
    printf("\n---------------------------------------------------------------------\n");
    cyan();
    printf("Message received from buffer: %s\n", response);
    bold_green();
    printf("---------------------------------------------------------------------\n");
    reset();
  
    close(client_fd); // closing the connected socket
}

void *loopPerThread(void *loopsPointer) // Code section made to control loop and threads that were set by the user
{
    int *totalLoops = (int *)loopsPointer;
    for(int loops = 0; loops < *totalLoops; loops++)
    {
        socketConnection();
    }
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

        // Shared Objects initialization
        getSemaphore();
        getSharedStats();

        
        // We need to use n threads
        pthread_t thread_id;
        for(int threads = 0; threads < totalThreads; threads++)
        {
            pthread_create(&thread_id, NULL, loopPerThread, (void *)&totalLoops);
        }
        pthread_join(thread_id, NULL);
    }
    return 0;
}