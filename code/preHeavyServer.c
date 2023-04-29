#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include "colorConf.c"
#include <semaphore.h>
#include <fcntl.h>
#include <ncurses.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>



//----------------------------------------------------------------//
//---------------------- Constants Values ------------------------//
//----------------------------------------------------------------//
#define PORT 8080
#define PROCESS_SEMAPHORE_NAME "processSemaphore"
#define REQUEST_SEMAPHORE_NAME "requestSemaphore"
#define SHARED_MEM_SEMAPHORE_NAME "sharedMemSemaphore"
#define SHARED_MEMORY "sharedMemory"
#define SHARED_PROCESS_ARRAY "sharedProcessArray"
#define MAX_CLIENTS 5


//----------------------------------------------------------------//
//----------------------------------------------------------------//
typedef struct SharedProcess { //manejo de procesos
    int pid;
    int socket_client;
    int child_port;
    int available;
    int active;
    int workIndex;
    int destroyed;
} SharedProcess;

typedef struct SharedMemory{
    int endSignal;
    int numProcess;
} SharedMemory;

sem_t *semProcess;
sem_t *semRequest;
sem_t *semShared;

SharedMemory *sharedMemory; //globales para el proceso
SharedProcess *processArray;

void errorMsg(){
    yellow();
    printf("--------------------------------------------------------\n");
    cyan();
    printf("Please insert the number of processes for the server. \n");
    yellow();
    printf("--------------------------------------------------------\n");
}


void sobelFilter(char fileName[30])
{
	char command[100] = "./output/sobel ";
	strcat(command, fileName);
	strcat(command, " ");
	strcat(command, fileName);

	system(command);
}

void endAll(){
    printf("Terminando todos los procesos.\n");
    sem_wait(semShared);
    sharedMemory->endSignal=1;
    sem_post(semShared);
    return;
}

void emergencyExit(){//Child process waiting for an interuption
    int shared_mem;
    shared_mem = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, 0666); // Shared memory for stats with id "shareStats"
    sharedMemory = mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem, 0);

    semShared = sem_open(SHARED_MEM_SEMAPHORE_NAME, O_CREAT | O_RDWR, 0666, 1);

    // Esperar la señal
    /*
        initscr();
    cbreak();
    noecho();*/
    while (1) {
        // Capturar la tecla presionada
        int ch = getch();

        if (ch == 'q') {
            printf("q, bro");
            // Enviar señal para detener los procesos
            endAll();
            endwin();
            break;
        }
    }
    printf("kill everything\n");

    // Si se llegó aquí, significa que se recibió la señal
    return;
}

void openSharedMemory(){
    int shared_mem;
    shared_mem = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, 0666); // Shared memory for stats with id "shareStats"
    sharedMemory = mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem, 0);

    int shared_process_arr;
    shared_process_arr = shm_open(SHARED_PROCESS_ARRAY, O_CREAT | O_RDWR, 0666); // Shared memory for stats with id "shareStats"
    processArray = mmap(0, sizeof(SharedProcess[sharedMemory->numProcess]), PROT_READ | PROT_WRITE, MAP_SHARED, shared_process_arr, 0);
    
    //semáforos para procesos
    semProcess = sem_open(PROCESS_SEMAPHORE_NAME, O_CREAT, 0666, sharedMemory->numProcess);
    semRequest = sem_open(REQUEST_SEMAPHORE_NAME, O_CREAT, 0666, 0);
    semShared = sem_open(SHARED_MEM_SEMAPHORE_NAME, O_CREAT, 0666, 1);
    return;
    
}

int openSocket(int i){
    // ================================================================================= //
    // Create socket 
    int socket_child;
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT+i+1); //i+1 para tener puertos únicos

    // Creating socket file descriptor
    if ((socket_child = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // Forcefully attaching socket to the PORT
    int port_child = htons(server_address.sin_port+i);
    if (bind(socket_child, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    // Listen to the port
    if (listen(socket_child, MAX_CLIENTS) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }


    processArray[i].child_port = port_child;
    return socket_child;

}

void createServerChild(int i){
    openSharedMemory();
    (processArray[i]).pid = getpid();
    (processArray[i]).available = 1;
    int valRead;
    sem_post(semShared);
    
    printf("Child -- %d -- %d ----------------- CREATED ----\n", processArray[i].pid, processArray[i].child_port);
    while(!sharedMemory->endSignal){
        
        printf("Child -- %d -- %d ---------------- Awaiting request ----\n", processArray[i].pid, processArray[i].child_port);
        sem_wait(semRequest);

        (processArray[i]).available = 0;
        if(sharedMemory->endSignal){
            sem_post(semRequest);
            break;
        }

        if(processArray[i].active == 0){
            sem_post(semRequest); //skip turn
        }else{
            int socket_child = openSocket(i);
            
            printf("Child -- %d -- %d ----------------- Processing request ----\n", processArray[i].pid, processArray[i].child_port);

            FILE *file;
            char* response = "preheavy";
            char buffer[1024];
            int socket_client;

            char indexName[30];
            char extension[30] = ".jpg";
            char fileName[30] = "files/preheavy/received";
            sprintf(indexName, "%d", (processArray[i]).workIndex);
            strcat(fileName, indexName);
            strcat(fileName, extension);
            file = fopen(fileName, "wb");

            valRead = read(processArray[i].socket_client, buffer, 1024);
            if (valRead == -1) {
                perror("read failed");
                exit(EXIT_FAILURE);
            } else if (valRead == 0) {
                // El socket del cliente se ha cerrado
                break;
            }
            // Escribir los datos recibidos en el archivo
            fwrite(buffer, 1, valRead, file);
            fflush(file);

            fclose(file);

            //sobelFilter(fileName); // Process Picture

            (processArray[i]).available = 1;
            sem_post(semProcess);
            close(socket_child);

            printf("Child -- %d -- %d ----------------- Image processed ----\n", processArray[i].pid, processArray[i].child_port);
        }
    
    }
    printf("Child %d has been killed, congratulations!\n", processArray[i].pid);
    processArray[i].destroyed = 1;
    sem_close(semProcess);
    sem_close(semRequest);
    exit(0);

}


int main(int argc, char const *argv[]){
    
    if(argc != 2){
        errorMsg();
        exit(1);
    }
    //=====================================================================================
    //ABRIR MEORIA COMPARTIDA
    int numProcess = atoi(argv[1]);
    processArray = (SharedProcess*) malloc(numProcess * sizeof(SharedProcess));

    // ----------------------------------------------------------------------------------- //
    // Shared memory for info and semaphores
    int shared_mem; //file descriptor of shared memory file
    shared_mem = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, 0666); // Shared memory for data

    ftruncate(shared_mem, sizeof(struct SharedMemory)); // Configure the size of the shared memory block

     // Map the shared memory segment in process address space
    sharedMemory = mmap(0, sizeof(struct SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem, 0);
    sharedMemory->numProcess = numProcess;
    sharedMemory->endSignal = 0;

    // ----------------------------------------------------------------------------------- //
    // Shared memory for processes
    int shm_process_arr;
    shm_process_arr = shm_open(SHARED_PROCESS_ARRAY, O_CREAT | O_RDWR, 0666); // Shared memory for stats with id "shareStats"

    ftruncate(shm_process_arr, sizeof(struct SharedProcess[numProcess]));

    
    processArray = mmap(0, sizeof(struct SharedProcess[numProcess]), PROT_READ | PROT_WRITE, MAP_SHARED, shm_process_arr, 0);
    

    // ----------------------------------------------------------------------------------- //
    // Semaphores for critical region
    semProcess = sem_open(PROCESS_SEMAPHORE_NAME, O_CREAT, 0666, numProcess);
    semRequest = sem_open(REQUEST_SEMAPHORE_NAME, O_CREAT, 0666, 0);
    semShared = sem_open(SHARED_MEM_SEMAPHORE_NAME, O_CREAT, 0666, 1);
      
    // ================================================================================= //
    // CREAR SOCKET DEL PADRE
    
    int socket_father, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);


    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Creating socket file descriptor
    if ((socket_father = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Forcefully attaching socket to the PORT
    if (bind(socket_father, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(socket_father, MAX_CLIENTS) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    printf("======================================================\n");
    printf("======== MAIN SERVER INITIALIZED IN PORT %d ========\n", PORT);
    printf("======================================================\n");
    printf("------------------------------------------------------\n");
    printf("Server -- PID -- PORT ------------------ STATUS ------\n");
    printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
    
    int processIndex = 0;

    pid_t pid = fork();
    if (pid == 0){ // child process to handle exit interruption
        processIndex=1;
        emergencyExit();
        return 0;
    }

    // Creating multiple child processes
    if(processIndex == 0){
        for (int i = 0; i < numProcess; i++){
            pid_t pid = fork();
            if (pid == 0) // child process
            {
                processIndex = pid;
                sem_wait(semShared);
                createServerChild(i);
                return 0;
            }
            else if (pid < 0) // error occurred
            {
                printf("Fork Failed\n");
                exit(1);
            }
        }
    }
    
    // Only parent process continues
    if (processIndex == 0)
    {
        // Parent process waits for incoming connections and assigns them to the waiting child processes
        int totalPictures = 5;
        int progressCounter = 1;
        int socket_client;
        while (!(sharedMemory)->endSignal) //Parent process main loop
        {
            if(progressCounter == totalPictures){
                sem_wait(semShared);
                sharedMemory->endSignal=1;
                sem_post(semShared);
            }
            
            printf("Main ---- 0 ---- %d ----------------- Awaiting new request ----\n", PORT);            
            
            socket_client = accept(socket_father, (struct sockaddr *)&address, (socklen_t *)&addrlen);
            if (socket_client < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            int value;
            sem_getvalue(semProcess, &value);

            printf("Main ---- 0 ---- %d ----------------- Assigning process ----\n", PORT);            
            
            sem_wait(semProcess);
            for(int i = 0; i < numProcess; i++){
                if((processArray[i]).available){
                    (processArray[i]).socket_client = socket_client;
                    (processArray[i]).workIndex = progressCounter;
                    (processArray[i]).available = 0;
                    processArray[i].active = 1;
                    sem_post(semRequest);
                    printf("Child -- %d -- %d ----------------- Request accepted ----\n", processArray[i].pid, processArray[i].child_port);
                    progressCounter++;
                    break;
                }
            }           
        }
        // Closing server socket

        int i = 0;
        int readyToEnd = 0;
        printf("waiting for the end to come...\n");
        while(!readyToEnd){
            if(processArray[i].destroyed){
                i++;
                printf("Child -- %d -- %d ----------------- Killed ----\n", processArray[i].pid, processArray[i].child_port);
                    
                if(i == numProcess){
                    readyToEnd=1;
                }else{
                    printf("not ready to end yet\n");
                }
            }
        }
        close(socket_client);
        close(socket_father);
        sem_close(semProcess);
        sem_unlink(PROCESS_SEMAPHORE_NAME);
        sem_close(semShared);
        sem_unlink(SHARED_MEM_SEMAPHORE_NAME);
        sem_close(semRequest);
        sem_unlink(REQUEST_SEMAPHORE_NAME);

        munmap(processArray, sizeof(struct SharedProcess[numProcess]));
        shm_unlink(SHARED_PROCESS_ARRAY);
        munmap(sharedMemory, sizeof(struct SharedMemory));
        shm_unlink(SHARED_MEMORY);
        

        return 0;
    }
    return 0;
}
