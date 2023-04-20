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

#define STATS_SEMAPHORE_NAME "statsSemaphore"
#define SHARED_STATS_MEMORY "sharedStats"

sem_t *semStats;
struct statsStruct *stats;
//----------------------------------------------------------------//
//----------------------------------------------------------------//


void getSemaphore() // Semaphore for stats
{
    semStats = sem_open(STATS_SEMAPHORE_NAME, O_CREAT | O_RDWR, 0666, 0);
}


void getStats() // Stats in shared memory
{
    int shm_stats;
    shm_stats = shm_open(SHARED_STATS_MEMORY, O_CREAT | O_RDWR, 0666); // Shared memory for stats
    stats = mmap(0, sizeof(struct statsStruct), PROT_READ | PROT_WRITE, MAP_SHARED, shm_stats, 0);
}


void stopServer()
{
    stats->killProcess = 1;
}

// https://www.geeksforgeeks.org/read-write-structure-from-to-a-file-in-c/
void writeStatsInFile()
{
    FILE *fp;
    fp = fopen("files/stats.txt", "w+");
    fprintf(fp, "[{'server' : 'FIFO', 'totalRequest' : %d, 'timeExecution' : %d, 'averageRequestTime' : %d, 'memoryConsumption' : %d, 'fourStat' : %d},{'server' : 'HEAVY', 'totalRequest' : %d, 'timeExecution' : %d, 'averageRequestTime' : %d, 'memoryConsumption' : %d, 'fourStat' : %d},{'server' : 'THREADS', 'totalRequest' : %d, 'timeExecution' : %d, 'averageRequestTime' : %d, 'memoryConsumption' : %d, 'fourStat' : %d},{'server' : 'PREHEAVY', 'totalRequest' : %d, 'timeExecution' : %d, 'averageRequestTime' : %d, 'memoryConsumption' : %d, 'fourStat' : %d}]", stats->totalRequestSequential, stats->timeExecutionSequential, stats->averageRequestTimeSequential, stats->memoryConsumptionSequential, stats->fourStatSequential, stats->totalRequestHeavyProcess, stats->timeExecutionHeavyProcess, stats->averageRequestTimeHeavyProcess, stats->memoryConsumptionHeavyProcess, stats->fourStatHeavyProcess, stats->totalRequestThreads, stats->timeExecutionThreads, stats->averageRequestTimeThreads, stats->memoryConsumptionThreads, stats->fourStatThreads, stats->totalRequestPreHeavy, stats->timeExecutionPreHeavy, stats->averageRequestTimePreHeavy, stats->memoryConsumptionPreHeavy, stats->fourStatPreHeavy);
    fclose(fp);
}


int main()
{
    getStats();
    stopServer();
    writeStatsInFile();
    return 0;
}



