

typedef struct statsStruct // Struct for visualizer module
{
    // FIFO
    int totalRequestSequential;
    int timeExecutionSequential;
    int averageRequestTimeSequential;
    int memoryConsumptionSequential;
    int fourStatSequential;

    // Heavy Process
    int totalRequestHeavyProcess;
    int timeExecutionHeavyProcess;
    int averageRequestTimeHeavyProcess;
    int memoryConsumptionHeavyProcess;
    int fourStatHeavyProcess;

    // Threads
    int totalRequestThreads;
    int timeExecutionThreads;
    int averageRequestTimeThreads;
    int memoryConsumptionThreads;
    int fourStatThreads;

    // Pre Heavy Process
    int totalRequestPreHeavy;
    int timeExecutionPreHeavy;
    int averageRequestTimePreHeavy;
    int memoryConsumptionPreHeavy;
    int fourStatPreHeavy;

    int killProcess; // Read only for server | visualizer can edit

} statsStruct;