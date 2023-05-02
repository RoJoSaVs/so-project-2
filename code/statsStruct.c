

typedef struct statsStruct // Struct for visualizer module
{
    // FIFO
    int totalRequestSequential;
    double timeExecutionSequential;
    double averageRequestTimeSequential;
    double memoryConsumptionSequential;
    double fourStatSequential;

    // Heavy Process
    int totalRequestHeavyProcess;
    double timeExecutionHeavyProcess;
    double averageRequestTimeHeavyProcess;
    double memoryConsumptionHeavyProcess;
    double fourStatHeavyProcess;

    // Threads
    int totalRequestThreads;
    double timeExecutionThreads;
    double averageRequestTimeThreads;
    double memoryConsumptionThreads;
    double fourStatThreads;

    // Pre Heavy Process
    int totalRequestPreHeavy;
    double timeExecutionPreHeavy;
    double averageRequestTimePreHeavy;
    double memoryConsumptionPreHeavy;
    double fourStatPreHeavy;

    int killProcess; // Read only for server | visualizer can edit

} statsStruct;


typedef struct memoryStruct // Struct for visualizer module
{
    long fifoMemory;
    long heavyMemory;
    long threadMemory;
    long preheavyMemory;

}memoryStruct;