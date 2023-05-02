#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>
#include <ncurses.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>




//----------------------------------------------------------------//
//---------------------- Constants Values ------------------------//
//----------------------------------------------------------------//
#define PORT 8080
#define PROCESS_SEMAPHORE_NAME "processSemaphore"
#define REQUEST_SEMAPHORE_NAME "requestSemaphore"
#define SHARED_MEM_SEMAPHORE_NAME "sharedMemSemaphore"
#define SHARED_MEMORY "sharedMemory"
#define SHARED_PROCESS_ARRAY "sharedProcessArray "
#define MAX_CLIENTS 5


//----------------------------------------------------------------//
//----------------------------------------------------------------//
typedef struct SharedProcess { //manejo de procesos
    int pid;
    int socket_client;
    int child_port;
    int readyToProcess;
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

int shared_mem_fd; 
int shared_process_fd; 

SharedMemory *sharedMemory; //globales para el proceso
SharedProcess *processArray;


void sobelFilter(char fileName[30])
{
	char command[100] = "./output/sobel ";
	strcat(command, fileName);
	strcat(command, " ");
	strcat(command, fileName);

	system(command);
}

void closeSemaphores(){
    
    
    if (sem_close(semProcess) == -1) {
        perror("Error al eliminar el semáforo 1");
        exit(1);
    }

    if (sem_close(semShared) == -1) {
        perror("Error al eliminar el semáforo 2");
        exit(1);
    }

    if (sem_close(semRequest) == -1) {
        perror("Error al eliminar el semáforo 3");
        exit(1);
    }
    return;
}

 void closeSharedMemory(){

    munmap(processArray, sizeof(SharedProcess)*sharedMemory->numProcess);
    close(shared_process_fd);

    munmap(sharedMemory, sizeof(SharedMemory));
    close(shared_mem_fd);
    return;
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
    //shared_mem = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, 0666); // Shared memory for stats with id "shareStats"
    //sharedMemory = mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem, 0);

    //semShared = sem_open(SHARED_MEM_SEMAPHORE_NAME, O_CREAT | O_RDWR, 0666, 1);

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
    
    if (sem_destroy(semShared) == -1) {
        perror("Error al eliminar el semáforo s");
        exit(1);
    }
    if (shm_unlink(SHARED_MEM_SEMAPHORE_NAME) == -1) {
        perror("Error al eliminar el semáforo a");
        exit(1);
    }
    //if (munmap(sharedMemory, sizeof(struct SharedMemory)) == -1) {
        //perror("Error al liberar la memoria compartida s");
      //  exit(1);
    //}
    //if (shm_unlink(SHARED_MEMORY) == -1) {
        //perror("Error al eliminar la memoria compartida a");
      //  exit(1);
    //}
    printf("Emergency exit triggered and closed\n");
    // Si se llegó aquí, significa que se recibió la señal
    return;
}


/*
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
*/
void enviarConfirmacion(int SocketFD){
     char mensaje[80] = "Paquete Recibido";
    //char mensaje[5] = "prehe";
    int lenMensaje = strlen(mensaje);
    if(write(SocketFD,mensaje,sizeof(mensaje)) == -1)
            perror("Error al enviar la confirmación:");
    printf("Confirmación enviada\n");

}//End enviarConfirmacion


void createServerChild(int i, int numProcess){
    (processArray[i]).pid = getpid();
    (processArray[i]).readyToProcess = 0;
    
    printf("Child -- %d -- %d ----------------- CREATED ---- %d .-.--.-..- \n", processArray[i].pid, processArray[i].child_port, i);
    while(!sharedMemory->endSignal){
        
        //Preparar todo para procesar una posible solicitud

        FILE *file;
        char* response = "preheavy";
        char buffer[1];

        char indexName[30];
        char extension[30] = ".png";
        char fileName[30] = "files/preheavy/nuevoArchivote";
        sprintf(indexName, "%d", (processArray[i]).workIndex);
        strcat(fileName, indexName);
        strcat(fileName, extension);
        file = fopen(fileName, "w+b");

        printf("Child -- %d -- %d ---------------- Awaiting request ----\n", processArray[i].pid, processArray[i].child_port);
    
        sem_wait(semRequest);
         
        processArray[i].readyToProcess = 1;
        printf("Child -- %d -- %d ---------------- Ready for assignament ----\n", processArray[i].pid, processArray[i].child_port);
        
        sem_post(semProcess);
        sem_wait(semShared); //Permite que todos esperen un request, solo el que pasa el semáforo se postea como listo
        (processArray[i]).readyToProcess = 0;

        if(sharedMemory->endSignal){ //Si se enciende la señal estando esperando
            sem_post(semRequest);
            sem_post(semShared);
            fclose(file);
            break;
        }
        printf("Child -- %d -- %d ----------------- Processing request by socket %d ----\n", processArray[i].pid, processArray[i].child_port, processArray[i].socket_client);
        
       
        enviarConfirmacion(processArray[i].socket_client);
        
        int bytes_received = -1;

        while ((bytes_received = recv(processArray[i].socket_client, buffer, 1, 0)) > 0) {

            fwrite(buffer, sizeof(char), 1, file);
        }
     
        fclose(file);
        
        close(processArray[i].socket_client);

        //sobelFilter(fileName); // Process Picture


        printf("Child -- %d -- %d ----------------- Image processed ----\n", processArray[i].pid, processArray[i].child_port); 
    
    }
    //close(socket_child);
    processArray[i].destroyed = 1;
    closeSharedMemory();
    closeSemaphores();
    printf("Child %d has been killed, congratulations!\n", processArray[i].pid);
    exit(0);

}


int main(int argc, char const *argv[]){
    
    if(argc != 2){
        printf("Error de parámetros, ingrese la cantidad de procesos deseada\n");
        exit(1);
    }
    //=====================================================================================
    //ABRIR MEORIA COMPARTIDA
    
    int numProcess = atoi(argv[1]);
    processArray = (SharedProcess*) malloc(numProcess * sizeof(SharedProcess));
    sharedMemory = (SharedMemory*) malloc(sizeof(SharedMemory));
    // ----------------------------------------------------------------------------------- //
    // Shared memory for info and semaphores


    shared_mem_fd = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, 0666);
    if(shared_mem_fd < 0){
        perror("shared memory fd error");
        exit(1);
    }
    ftruncate(shared_mem_fd, sizeof(SharedMemory));

    sharedMemory = mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem_fd, 0);
    if(sharedMemory == MAP_FAILED){
        perror("shared memory map error");
        exit(1);
    }
    

    sharedMemory->numProcess = numProcess;
    sharedMemory->endSignal = 0;
    

    // ----------------------------------------------------------------------------------- //
    // Shared memory for processes
    shared_process_fd = shm_open(SHARED_PROCESS_ARRAY, O_CREAT | O_RDWR, 0666);
    if(shared_process_fd < 0){
        perror("shared process fd error");
        exit(1);
    }
    ftruncate(shared_process_fd, sizeof(SharedProcess)*numProcess);

    processArray = mmap(0, sizeof(SharedProcess)*numProcess, PROT_READ | PROT_WRITE, MAP_SHARED, shared_process_fd, 0);
    if(processArray == MAP_FAILED){
        perror("shared memory map error");
        exit(1);
    }
    int processIndex = 0;
    

    
    // ----------------------------------------------------------------------------------- //
    // Semaphores for critical region
    //Sempahore for shared info

    semShared = sem_open(SHARED_MEM_SEMAPHORE_NAME, O_CREAT, 0666, 0);
    semProcess = sem_open(PROCESS_SEMAPHORE_NAME, O_CREAT, 0666, 0);
    semRequest = sem_open(REQUEST_SEMAPHORE_NAME, O_CREAT, 0666, 0);

    sem_init(semShared, 1, 0);
    sem_init(semProcess, 1, 0);
    sem_init(semRequest, 1, 0);

    if (semShared == SEM_FAILED) {
        printf("Error al crear semáforo: %s\n", strerror(errno));
    } 

    //===================================================================================//
    // Child process por manual interruption 
    /*pid_t pid = fork();
    if (pid == 0){ // child process to handle exit interruption
        processIndex=1;
        emergencyExit();
        return 0;
    }*/

    // ================================================================================= //
    // CREAR SOCKET DEL PADRE
    
    int socket_father, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);


    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_family = AF_INET;
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
    printf("========================================================\n");
    printf("======== MAIN SERVER INITIALIZED IN PORT %d ============\n", PORT);
    printf("========================================================\n");
    printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
    printf("Server -- PID -- PORT ------------------ STATUS --------\n");
    printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n\n");
    


    // Creating multiple child processes
    if(processIndex == 0){
        for (int i = 0; i < numProcess; i++){
            pid_t pid = fork();

            if (pid == 0) // child process
            {
                processIndex = pid;
                createServerChild(i, numProcess);
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
        int progressCounter = 0;
        
        struct sockaddr_in clSockAddr;
	    int SocketClientFD;
	    int clientLen;
        clientLen = sizeof(clSockAddr);
        
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
        while (!(sharedMemory)->endSignal) //Parent process main loop
        {

            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(socket_father, &readfds);
            FD_SET(STDIN_FILENO, &readfds);
            
            if (select(socket_father + 1, &readfds, NULL, NULL, NULL) == -1) {
                perror("Error en select");
                exit(EXIT_FAILURE);
            }
            // Verificar si se ha presionado una tecla
            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                printf("Se ha presionado una tecla. Cerrando el servidor...\n");
                sharedMemory->endSignal=1;
                sem_post(semShared);
                break;
            }

            if(progressCounter == totalPictures){
                sem_wait(semShared);
                sharedMemory->endSignal=1;
                printf("max pictures processed\n");
                sem_post(semShared);
                break;
            }
            
            printf("Main ---- 0 ---- %d ----------------- Awaiting new request ----\n", PORT);            
            SocketClientFD = accept(socket_father, (struct sockaddr *)&clSockAddr, &clientLen);
            
            if (SocketClientFD < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
           
            clSockAddr.sin_family = AF_INET;
            clSockAddr.sin_port = htons(PORT);

            sem_post(semRequest);

            printf("\n\nMain ---- 0 ---- %d ----------------- Assigning process for socket %d ----\n", PORT, SocketClientFD);            

            sleep(1);
            sem_wait(semProcess);
            
            for(int i = 0; i < numProcess; i++){
                if((processArray[i]).readyToProcess){
                    (processArray[i]).socket_client = SocketClientFD;
                    (processArray[i]).workIndex = progressCounter;
                    
                    printf("Main ----- 0 ---- %d ----------------- Request assigned to Child %d for socket %d ----\n", PORT, processArray[i].pid, processArray[i].socket_client);
                    progressCounter++;
                    sem_post(semShared);
                    break;
                }
            }
        
        }
        // Closing server socket

        int i = 0;
        int readyToEnd = 0;
        printf("Deteniendo todos los procesos...\n");
        sem_post(semShared);
        sem_post(semRequest);
        while(!readyToEnd){
            if(processArray[i].destroyed){
                i++;
                printf("Child -- %d -- %d ----------------- Killed ----\n", processArray[i].pid, processArray[i].child_port);
                    
                if(i == numProcess){
                    readyToEnd=1;
                    printf("Todos los procesos se han detenido exitosamente!\n");
                }else{
                    printf("not ready to end yet\n");
                }
            }
        }
        close(SocketClientFD);
        close(socket_father);


        closeSemaphores();
        closeSharedMemory();
        shm_unlink(SHARED_MEMORY);
        shm_unlink(SHARED_PROCESS_ARRAY);
        if (sem_unlink(PROCESS_SEMAPHORE_NAME) == -1) {
            perror("Error al eliminar el semáforo a");
            exit(1);
        }
        if (sem_unlink(SHARED_MEM_SEMAPHORE_NAME) == -1) {
            perror("Error al eliminar el semáforo b");
            exit(1);
        }
        if (sem_unlink(REQUEST_SEMAPHORE_NAME) == -1) {
            perror("Error al eliminar el semáforo c");
            exit(1);
        }
        
        return 0;
    }
    return 0;
}
