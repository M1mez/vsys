/*
	Client file
	Stefan Mühlbacher & Johannes Fessler
	uid: if16b011 &  if16b042
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SNDBUFF 1024
#define RCVBUFF 1023

enum{READ, LIST, SEND, DEL};

bool sendMessage(int createSocket, char buffer[], int maxInput);

int main(int argc, char **argv){

	if(argc < 2 ){
		printf("Please enter IP address and Port number\n");
		exit(EXIT_FAILURE);
	}

	printf("Client started\n");

	int createSocket;
	char buffer[1024];
	struct sockaddr_in address;
	int size;

	if((createSocket = socket(AF_INET,SOCK_STREAM,0)) == -1){
		printf("Error\n");
		exit(EXIT_FAILURE);
	}

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(atoi(argv[1]));
	inet_aton(argv[2], &address.sin_addr);

	if(connect (createSocket, (struct sockaddr *) &address,sizeof(address)) == 0){
		printf("Connected to server.\n");
		size = recv(createSocket,buffer,1023,0);
		if(size>0){
			buffer[size]='\0';
			printf("%s", buffer);
		}
	}else {
		perror("Could not connect to server");
		exit(EXIT_FAILURE);
	}

	do {
	int option = -1;
        if(strncmp(buffer, "READ",4) == 0){
			option = 0;
		}else if(strncmp(buffer,"LIST", 4) == 0){
			option = 1;
		}else if(strncmp(buffer,"SEND", 4) == 0){
			option = 2;
		}else if(strncmp(buffer,"DEL",3) == 0){
			option = 3;
		}else option = -1;


		switch(option) {
            case READ: {
                printf("client read:\n");
                break;
            }
            case LIST: {
                printf("client list: \n\n");
                break;
            }
            case SEND: {
                char *sendInfo[4] = {"Sender: ", "Empfänger: ", "Betreff: ", "Nachricht:\n"};
                int sendCount = 0;
                printf("Client send\n");

                printf("%s", sendInfo[sendCount++]);
                    if(sendMessage(createSocket, buffer, 10)) break;
                printf("%s", sendInfo[sendCount++]);
                    if(sendMessage(createSocket, buffer, 10)) break;
                printf("%s", sendInfo[sendCount++]);
                    if(sendMessage(createSocket, buffer, 82)) break;
                printf("%s", sendInfo[sendCount]);
                while(strcmp(buffer,".\n") != 0)
                    sendMessage(createSocket, buffer, SNDBUFF);
                printf("~~~~~~~~~~~~~\nMessage sent!\n");
                break;
            }
            case DEL: {
                printf("client del\n");
                break;
            }
            default: {
                printf("No valid Input detected!\ntry:\n  READ\n  LIST\n  SEND\n  DEL\n  QUIT\n");
                break;
            }
		}

		fgets(buffer,SNDBUFF,stdin);
		send(createSocket,buffer,strlen(buffer),0);
	}while(strcmp(buffer,"QUIT\n") !=0);

	close(createSocket);
return 0;
}

bool sendMessage(int createSocket, char buffer[], int maxInput){
    size_t nameSize;
    do{
        fgets(buffer,SNDBUFF,stdin);
        if(strncmp(buffer, "QUIT",4) == 0 && maxInput != SNDBUFF){
            return true;
        }
        nameSize = strlen(buffer);
        if (nameSize > maxInput){
            printf("Please enter maximum %d letters!\n",maxInput-2);
        }
    }while(nameSize > maxInput);
    send(createSocket,buffer,strlen(buffer),0);
    return false;
}




















