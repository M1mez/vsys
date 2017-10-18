/*
	Client file
	Stefan Mühlbacher & Johannes Fessler
	uid: if16b011 &  if16b042
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <climits>

#define SNDBUFF 1024
#define RCVBUFF 1023

enum{READ, LIST, SEND, DEL, QUIT};
enum{ISMESSAGE, NOMESSAGE, ONEORTWO};

bool sendMessage(int createSocket, char buffer[], int maxInput, int messageType);
int chooseMode(char buffer[]);

using namespace std;

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
		size = recv(createSocket,buffer,RCVBUFF,0);
		if(size>0){
			buffer[size]='\0';
			printf("%s", buffer);
		}
	}else {
		perror("Could not connect to server");
		exit(EXIT_FAILURE);
	}

	do {
	   
        printf("Type one of the following:\n  LIST\n  SEND\n  DEL\n  QUIT\n");
        fgets(buffer,SNDBUFF,stdin);
        send(createSocket,buffer,strlen(buffer),0);
		switch(chooseMode(buffer)) {
            case READ: {
                printf("client read:\n");

                while(strcmp(buffer,".\n") != 0)
                {
                    size = recv(createSocket, buffer, RCVBUFF, 0);
                    buffer[size] = '\0';
                    string Sender(buffer);
                    cout << Sender << endl;
                }

                break;
            }
            case LIST: {
                printf("client list: \n\n");
                cout << "Whose mails would you like to list? (Sender)" << endl;
                    if(sendMessage(createSocket, buffer,  8, NOMESSAGE)) break;
                cout << "1: List my inbox-entries!"  << endl;
                cout << "2: List my outbox-entries!" << endl;
                cout << "9: Quit to menu!" << endl;
                    if(sendMessage(createSocket, buffer,  1, ONEORTWO)) break;
                do{
                    size = recv(createSocket,buffer,RCVBUFF,0);
                    buffer[size] = '\0';
                    if(strcmp(buffer,".\n") == 0) break;
                    printf("%s", buffer);
                }while(strcmp(buffer,".\n") != 0);  

                break;
            }
            case SEND: {
                string sendInfo[4] = {"Sender: ", "Empfänger: ", "Betreff: ", "Nachricht:\n"};
                int sendCount = 0;
                printf("Client send\n");

                cout << sendInfo[sendCount++] << endl;
                    if(sendMessage(createSocket, buffer,  8, NOMESSAGE)) break;
                cout << sendInfo[sendCount++] << endl;
                    if(sendMessage(createSocket, buffer,  8, NOMESSAGE)) break;
                cout << sendInfo[sendCount++] << endl;
                    if(sendMessage(createSocket, buffer, 80, NOMESSAGE)) break;
                cout << sendInfo[sendCount++] << endl;
                    sendMessage(createSocket, buffer, SNDBUFF, ISMESSAGE);
                printf("~~~~~~~~~~~~~\nMessage sent!\n");
                break;
            }
            case DEL: {
                printf("client del\n");
                break;
            }
            case QUIT: { 
                close(createSocket);
                break;
            }
            default: {
                printf("No valid Input detected!\ntry:\n  READ\n  LIST\n  SEND\n  DEL\n  QUIT\n");
                break;  
            }
		}

		//fgets(buffer,SNDBUFF,stdin);
		//send(createSocket,buffer,strlen(buffer),0);
	}while(strcmp(buffer,"QUIT\n") !=0);

    return 0;
}

bool sendMessage(int createSocket, char buffer[], int maxInput, int messageType){
    size_t nameSize;
    bool isQuit = false;
    //maxInput;

    switch(messageType){
        case ISMESSAGE: {
            do{
                fgets(buffer,SNDBUFF,stdin);
                if(strlen(buffer) > (unsigned)maxInput+1){
                    cout << "Line too long! Please stay under" << maxInput << "signs!" << endl;
                    continue;
                }
                send(createSocket,buffer,strlen(buffer),0);
            }while(strcmp(buffer,".\n") != 0);  
            break;
        }
        case NOMESSAGE: {
            do{
                fgets(buffer,SNDBUFF,stdin);

                if(strncasecmp(buffer, "QUIT",4) == 0) isQuit = true;
            
                nameSize = strlen(buffer);
                if (nameSize > (unsigned)maxInput+1){
                    printf("Please enter maximum %d letters!\n",maxInput);
                }
            }while(nameSize > (unsigned)maxInput+1);
            send(createSocket,buffer,strlen(buffer),0);
            break;
        }
        case ONEORTWO: {
            int tempNum;
            do{
                //cin.clear();
                //cin.ignore(INT_MAX);
                //cin >> tempNum;
                memset(buffer,'0', SNDBUFF);
                fgets(buffer,SNDBUFF,stdin);
                tempNum = atoi(buffer);
                printf("INPUT: %d\n", tempNum );
                if(tempNum == 9) {
                    cout << "Quit to menu..." << endl;
                    isQuit = true;
                }
                if ((tempNum != 1 && tempNum != 2 && tempNum != 9) || cin.fail()){
                    printf("Please enter only 1, 2 or 9\n");
                }
            }while((tempNum != 1 && tempNum != 2 && tempNum != 9) || cin.fail());
            buffer[0] =  tempNum + '0';
            buffer[1] = '\n';
            buffer[2] =  '\0';
            send(createSocket,buffer,strlen(buffer),0);
            break;
        }
    }
    return isQuit;
}

int chooseMode(char buffer[]){
    if(strncasecmp(buffer, "READ", 4) == 0) return 0;
    if(strncasecmp(buffer, "LIST", 4) == 0) return 1;
    if(strncasecmp(buffer, "SEND", 4) == 0) return 2;
    if(strncasecmp(buffer, "DEL",  3) == 0) return 3;
    if(strncasecmp(buffer, "QUIT", 4) == 0) return 4;
    return -1;
}























