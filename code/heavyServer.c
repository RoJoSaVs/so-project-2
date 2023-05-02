#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
 #include <sys/types.h>
 #include <signal.h>
 #include <sys/resource.h>

#include <semaphore.h>

#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>

//----------------------------------------------------------------//
//---------------------- Constants Values ------------------------//
//----------------------------------------------------------------//
#define PORT 25565
#define STATS_SEMAPHORE_NAME "heavyStatsSemaphore"
#define HEAVY_MEMORY_STAT "heavyMemoryStats"

int server_fd;
int new_socket;
int valread;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);

void *heavyMemory;
//----------------------------------------------------------------//
//----------------------------------------------------------------//


void setSharedMemory()
{
	int shm_stats;
    shm_stats = shm_open(HEAVY_MEMORY_STAT, O_CREAT | O_RDWR, 0666); // Shared memory for stats with id "shareStats"
	ftruncate(shm_stats, sizeof(long)); // Configure the size of the shared memory block
    heavyMemory = mmap(0, sizeof(long), PROT_READ | PROT_WRITE, MAP_SHARED, shm_stats, 0);

	heavyMemory = 0;
}

void getMemoryConsumption()
{
	struct rusage myUsage;
    getrusage(RUSAGE_SELF, &myUsage);
	heavyMemory += myUsage.ru_maxrss;
    // return myUsage.ru_maxrss;
}


void sobelFilter(char fileName[30], int index)
{
	char command[100] = "./output/sobel ";
	strcat(command, fileName);
	strcat(command, " ");
	strcat(command, fileName);
	strcat(command, " files/heavy/");

	// char snum[5];
	// sprintf(snum, "%d", index);
	// strcat(command, snum);

	system(command);
}

void receiveFile(int new_socket, int index, pid_t mainProcessPid)
{
	fork(); //https://www.geeksforgeeks.org/fork-system-call/
	if(getpid() != mainProcessPid)
	{
		FILE *file;
		char response[5] = "heavy";
		char buffer[1];
		int received = -1;

		char indexName[20];
		char extension[20] = ".jpg";
		char fileName[30] = "files/heavy/received";
		sprintf(indexName, "%d", index);
		strcat(fileName, indexName);
		strcat(fileName, extension);
		file = fopen(fileName, "wb");

		// Get memory consumption by server
		getMemoryConsumption();

		send(new_socket, response, (strlen(response) - 1), 0);

		while((received = recv(new_socket, buffer, 1, 0)) > 0) // Receive the whole file
		{
			fwrite(buffer, sizeof(char), 1, file);
		}

		fclose(file);

		sobelFilter(fileName, index); // Process Picture

		close(new_socket); // closing the connected socket
		shutdown(server_fd, SHUT_RDWR);// closing the listening socket
		exit(0); //https://stackoverflow.com/questions/6501522/how-to-kill-a-child-process-by-the-parent-process
	}
}


void runServer(int index)
{
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	
    address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching socket to the PORT
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) 
    {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) 
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}

	// Heavy process 
	pid_t mainProcessPid = getpid();
	receiveFile(new_socket, index, mainProcessPid);
	
	close(new_socket); // closing the connected socket
	shutdown(server_fd, SHUT_RDWR);// closing the listening socket
}


int main(int argc, char const* argv[])
{
	setSharedMemory();

	int totalPictures = 100;
	for(int counter = 1; counter <= totalPictures; counter++)
	{
		runServer(counter);
		printf("Picture #%d processed\n", counter);
	}
	
	return 0;
}