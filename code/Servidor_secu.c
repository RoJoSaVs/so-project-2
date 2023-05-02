
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

/*Definición de constantes*/
#define BUFFSIZE 1
#define PUERTO		1100
#define ERROR		-1

/*Prototipos de función*/
void recibirArchivo(int SocketFD, FILE *file,int index2);
void enviarConfirmacion(int SocketFD);
void enviarMD5SUM(int SocketFD);
void getIP(int tipo, char * IP);
/*Recibe la clave de la interfaz que va a manejar:
 * lo : 0
 * wlan: 1
 * eth0: 2
*/

void sobelFilter(char fileName[30])
{
	char command[100] = "./output/sobel ";
	strcat(command, fileName);
	strcat(command, " ");
	strcat(command, fileName);
	strcat(command, " files/fifo/");

	system(command);
}

int main(int argc, char *argv[]){
	struct sockaddr_in stSockAddr;
    struct sockaddr_in clSockAddr;
	FILE *archivo;
	char *direccIP;
	int SocketServerFD;
	int SocketClientFD;
	int clientLen;
	int serverLen;
	direccIP = malloc(20);

	char portName[20];
	sprintf(portName, "%d", PUERTO);
	char port_aux[30] = "Socket atado en el puerto: ";
	strcat(port_aux, portName);
	char port_aux1[30] = "\n";
	strcat(port_aux, port_aux1);
	printf(port_aux);

	int contador =0;

	/*Verifica que el número de parametros sea el correcto*/
	if(argc < 2){
		perror("Uso ./Servidor_secu <clave de interfaz 0:lo 1:wlan 2:eth0>");
		exit(EXIT_FAILURE);	
	}

	/*Se obtiene la IP de la interfaz solicitada*/
	getIP(atoi(argv[1]),direccIP);

	/*Se crea el socket*/
	if((SocketServerFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == ERROR){
		perror("No se puede crear el socket");
		exit(EXIT_FAILURE);
	}//End if-SocketFD
 
	/*Se configura la dirección del socket*/
	memset(&stSockAddr, 0, sizeof stSockAddr);
	 
	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(PUERTO);
	stSockAddr.sin_addr.s_addr = INADDR_ANY;
 
	if(bind(SocketServerFD,(struct sockaddr *)&stSockAddr, sizeof stSockAddr) == ERROR){
		perror("Error en bind");
		close(SocketServerFD);
		exit(EXIT_FAILURE);
	}//End if-bind
	inet_pton(AF_INET, direccIP, &stSockAddr.sin_addr);
	
	printf("Socket atado a la dirección %s\n",(char *)inet_ntoa(stSockAddr.sin_addr));	
	

	if(listen(SocketServerFD, 10) == ERROR){
		perror("Error listen");
		close(SocketServerFD);
		exit(EXIT_FAILURE);
	}//End if-listen

	// Configurar la entrada estándar para no bloquear
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
	while (1){

		fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(SocketServerFD, &readfds);
        FD_SET(STDIN_FILENO, &readfds);
		
		if (select(SocketServerFD + 1, &readfds, NULL, NULL, NULL) == -1) {
            perror("Error en select");
            exit(EXIT_FAILURE);
        }
		// Verificar si se ha presionado una tecla
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            printf("Se ha presionado una tecla. Cerrando el servidor...\n");
			close(SocketServerFD);
            exit(EXIT_FAILURE);
			break;
        }
		clientLen = sizeof(clSockAddr);
		//Espera por la conexión de un cliente//
		if ((SocketClientFD = accept(SocketServerFD, 
						    (struct sockaddr *) &clSockAddr,
						    &clientLen)) < 0){
			perror("Fallo para acpetar la conexión del cliente");
		}//End if-accept
		/*Se configura la dirección del cliente*/
		clSockAddr.sin_family = AF_INET;
		clSockAddr.sin_port = htons(PUERTO);
		printf("Cliente conectado: %s\n",inet_ntoa(clSockAddr.sin_addr));
		if(contador<100){
		/*Se recibe el archivo del cliente*/
		recibirArchivo(SocketClientFD, archivo,contador);
		contador++;
		}


	}//End infinity while

 	close(SocketClientFD);
	close(SocketServerFD);
	
	return 0;
}//End main program

void recibirArchivo(int SocketFD, FILE *file,int index2){
	char buffer[BUFFSIZE];
	int recibido = -1;
	printf("Imagen recibida-----------------------------------------\n");
	/*Se abre el archivo para escritura*/
	int index = index2;
	char indexName[20];
	char extension[20] = ".jpg";
	char fileName[30] = "files/fifo/received";
	sprintf(indexName, "%d", index);
	strcat(fileName, indexName);
	strcat(fileName, extension);
	file = fopen(fileName, "wb");
	enviarConfirmacion(SocketFD);
	// enviarMD5SUM(SocketFD);
	while((recibido = recv(SocketFD, buffer, BUFFSIZE, 0)) > 0){
		//printf("%s",buffer);
		fwrite(buffer,sizeof(char),1,file);
	}//Termina la recepción del archivo

	fclose(file);
	sobelFilter(fileName);

}//End recibirArchivo procedure

void enviarConfirmacion(int SocketFD){
	// char mensaje[80] = "Paquete Recibido";
	char mensaje[5] = "fifo1";
	int lenMensaje = strlen(mensaje);
	printf("\nConfirmación enviada\n");
	if(write(SocketFD,mensaje,sizeof(mensaje)) == ERROR)
			perror("Error al enviar la confirmación:");

	
}//End enviarConfirmacion

void enviarMD5SUM(int SocketFD){
	FILE *tmp;//Apuntador al archivo temporal que guarda el MD5SUM del archivo.
	char *fileName = "verificacion";
	char md5sum[80];
	system("md5sum archivoRecibido >> verificacion");
	
	tmp = fopen(fileName,"r");
	fscanf(tmp,"%s",md5sum);	
	printf("\nMD5SUM:%s\n",md5sum);	
	write(SocketFD,md5sum,sizeof(md5sum));
	fclose(tmp);

}//End enviarMD5DUM

void getIP(int tipo, char * IP){
	FILE *tmpIP;
	char dIP[20];
	char dIP2[20];
	int i,j;
	switch(tipo){
		case 0:
			system("ifconfig lo | grep inet > tmp");
			break;
		case 1:
			system("ifconfig wlan | grep inet > tmp");
			break;
		case 2:
			system("ifconfig eth | grep inet > tmp");
			break;

	
	}//End switch
	j = 0;
	tmpIP = fopen("tmp","r");
	fscanf(tmpIP,"%s %s",dIP,dIP);
	for(i = 5;i<20;i++){
		IP[j] = dIP[i];	
		j++;		
	}
}//End getIP