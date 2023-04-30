#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
 #include <sys/types.h>
 #include <signal.h>

//----------------------------------------------------------------//
//---------------------- Constants Values ------------------------//
//----------------------------------------------------------------//
#define PORT 25565
#define STATS_SEMAPHORE_NAME "statsSemaphore"
#define SHARED_STATS_MEMORY "sharedStats"

int server_fd;
int new_socket;
int valread;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);

// sem_t *semStats;
// struct statsStruct *stats;
//----------------------------------------------------------------//
//----------------------------------------------------------------//

void sobelFilter(char fileName[30])
{
	char command[100] = "./output/sobel ";
	strcat(command, fileName);
	strcat(command, " ");
	strcat(command, fileName);
	strcat(command, " files/heavy/");

	system(command);
}

void receiveFile(int new_socket, int index, pid_t mainProcessPid)
{
	fork(); //https://www.geeksforgeeks.org/fork-system-call/
	if(getpid() != mainProcessPid)
	{
		FILE *file;
		char* response = "heavy";
		char buffer[1];
		int received = -1;

		char indexName[20];
		char extension[20] = ".jpg";
		char fileName[30] = "files/heavy/received";
		sprintf(indexName, "%d", index);
		strcat(fileName, indexName);
		strcat(fileName, extension);


		send(new_socket, response, strlen(response), 0);

        // guardar en directorio
        // down
        file = fopen(fileName, "wb");

		while((received = recv(new_socket, buffer, 1, 0)) > 0) // Receive the whole file
		{
			fwrite(buffer, sizeof(char), 1, file);
		}

		fclose(file);
        // up

		sobelFilter(fileName); // Process Picture
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

	if(getpid() != mainProcessPid) // Destroy the process created by fork
	{
		exit(0); //https://stackoverflow.com/questions/6501522/how-to-kill-a-child-process-by-the-parent-process
	}
}


int main(int argc, char const* argv[])
{
	int totalPictures = 5;
	for(int counter = 1; counter <= totalPictures; counter++)
	{
		runServer(counter);
		printf("Picture #%d processed\n", counter);
	}
	return 0;
}
