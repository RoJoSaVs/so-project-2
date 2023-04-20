#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/shm.h>
#include <sys/mman.h>
#include <semaphore.h>

#include "statsStruct.c"
//----------------------------------------------------------------//
//---------------------- Constants Values ------------------------//
//----------------------------------------------------------------//
#define FIFO_ID 0
#define HEAVY_ID 1
#define THREAD_ID 2
#define PREHEAVY_ID 3

#define STATS_SEMAPHORE_NAME "statsSemaphore"
#define SHARED_STATS_MEMORY "sharedStats"

sem_t *semStats;
struct statsStruct *stats;
//----------------------------------------------------------------//
//----------------------------------------------------------------//

struct statsStruct defaultValues()
{
    struct statsStruct initialStats;

    // FIFO
    initialStats.totalRequestSequential = 0;
    initialStats.timeExecutionSequential = 0;
    initialStats.averageRequestTimeSequential = 0;
    initialStats.memoryConsumptionSequential = 0;
    initialStats.fourStatSequential = 0;

    // Heavy Process
    initialStats.totalRequestHeavyProcess = 0;
    initialStats.timeExecutionHeavyProcess = 0;
    initialStats.averageRequestTimeHeavyProcess = 0;
    initialStats.memoryConsumptionHeavyProcess = 0;
    initialStats.fourStatHeavyProcess = 0;

    // Threads
    initialStats.totalRequestThreads = 0;
    initialStats.timeExecutionThreads = 0;
    initialStats.averageRequestTimeThreads = 0;
    initialStats.memoryConsumptionThreads = 0;
    initialStats.fourStatThreads = 0;

    // Pre Heavy Process
    initialStats.totalRequestPreHeavy = 0;
    initialStats.timeExecutionPreHeavy = 0;
    initialStats.averageRequestTimePreHeavy = 0;
    initialStats.memoryConsumptionPreHeavy = 0;
    initialStats.fourStatPreHeavy = 0;

    initialStats.killProcess = 0;

    return initialStats;
}


void setSemaphore()
{
    semStats = sem_open(STATS_SEMAPHORE_NAME, O_CREAT | O_RDWR, 0666, 1);

    while(1)
    {
        if(sem_trywait(semStats) == -1)
        {
            break;
        }
    }
    
    sem_post(semStats);
}


void setSharedStats() // Shared memory for stats
{
    int shm_stats;
    shm_stats = shm_open(SHARED_STATS_MEMORY, O_CREAT | O_RDWR, 0666); // Shared memory for stats with id "shareStats"

    ftruncate(shm_stats, sizeof(struct statsStruct));

    stats = mmap(0, sizeof(struct statsStruct), PROT_READ | PROT_WRITE, MAP_SHARED, shm_stats, 0);
    *stats = defaultValues();
}


void editStats(int id, int totalRequest, int timeExecution, int averageRequestTime, int memoryConsumption, int fourStat)
{
    if(id == FIFO_ID)
    {
        sem_wait(semStats);
        stats->totalRequestSequential += totalRequest;
        stats->timeExecutionSequential += timeExecution;
        stats->averageRequestTimeSequential += averageRequestTime;
        stats->memoryConsumptionSequential += memoryConsumption;
        stats->fourStatSequential += fourStat;
        sem_post(semStats);
    }

    else if(id == HEAVY_ID)
    {
        sem_wait(semStats);
        stats->totalRequestHeavyProcess += totalRequest;
        stats->timeExecutionHeavyProcess += timeExecution;
        stats->averageRequestTimeHeavyProcess += averageRequestTime;
        stats->memoryConsumptionHeavyProcess += memoryConsumption;
        stats->fourStatHeavyProcess += fourStat;
        sem_post(semStats);
    }

    else if(id == THREAD_ID)
    {
        sem_wait(semStats);
        stats->totalRequestThreads += totalRequest;
        stats->timeExecutionThreads += timeExecution;
        stats->averageRequestTimeThreads += averageRequestTime;
        stats->memoryConsumptionThreads += memoryConsumption;
        stats->fourStatThreads += fourStat;
        sem_post(semStats);
    }

    else if(id == PREHEAVY_ID)
    {
        sem_wait(semStats);
        stats->totalRequestPreHeavy += totalRequest;
        stats->timeExecutionPreHeavy += timeExecution;
        stats->averageRequestTimePreHeavy += averageRequestTime;
        stats->memoryConsumptionPreHeavy += memoryConsumption;
        stats->fourStatPreHeavy += fourStat;
        sem_post(semStats);
    }
}


int main()
{
    setSemaphore();
    setSharedStats();
    return 0;
}