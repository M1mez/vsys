/*
	Server file
	Stefan Mühlbacher & Johannes Fessler
	uid: if16b011 &  if16b042
*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

#define SNDBUFF 1024
#define RCVBUFF 1023

using namespace std;

enum{READ, LIST, SEND, DEL};

void listDir(DIR *mStorageDir);
int chooseMode(char buffer[]);
DIR *searchDir(DIR *mStorageDir, string name, string path);


int main(int argc, char **argv){
	if(argc < 2){
		printf("Please enter Port number and path!\n");
		exit(EXIT_FAILURE);
	}
	printf("%s %s\n", argv[1],argv[2]);

	int size;

	//build connection
	socklen_t addrlen;
	struct sockaddr_in address, clientAddress;
	int newSocket;
	char buffer[1024];
	int createSocket = socket(AF_INET,SOCK_STREAM,0);
	memset(&address,0,sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(atoi(argv[1]));

    //directory
    string path(argv[2]);
    DIR *mStorageDir = opendir(path.c_str());
    DIR *inboxDir;
	DIR *outboxDir;
 



	if(bind (createSocket, (struct sockaddr *) &address, sizeof(address)) !=0){
		perror("bind_error");
		exit(EXIT_FAILURE);
	}


	listen(createSocket, 5);
	addrlen = sizeof(struct sockaddr_in);

	while(1){
		printf("Waiting for connections. \n");
		newSocket = accept(createSocket, (struct sockaddr *)&clientAddress, &addrlen);
		if(newSocket > 0){
			printf("Connected!\n");
			strcpy(buffer,"Welcome. Please enter your command:\n");
			send(newSocket,buffer,strlen(buffer),0);
		}
	do{
		size = recv(newSocket,buffer,1023,0);
		if(size>0){
			buffer[size] = '\0';

		switch(chooseMode(buffer)) {
            case READ: {
                printf("READ mail\n");
                break;
            }
            case LIST: {
                printf("LIST mails: \n\n");
                break;
            }
            case SEND: {
				printf("SEND mail\n");

            	string sendInfo[4] = {"Sender: ", "Empfänger: ", "Betreff: ", "Nachricht:\n"};
            	string sendStep[3];
            	int rcvCount = 0;

            	


                
                //buffer[size] = '\0';
                size = recv(newSocket,buffer,RCVBUFF,0);
                string Sender(buffer);
                cout << "SENDER: " << Sender << endl;
                //outboxDir = searchUser(mStorageDir, buffer);
                size = recv(newSocket,buffer,RCVBUFF,0);
                buffer[size] = '\0';
                printf("Empfänger: %s",buffer);
                size = recv(newSocket, buffer, RCVBUFF,0);
                buffer[size] = '\0';
                printf("Betreff: %s", buffer);
                printf("Nachricht: ");
                do{
                    size = recv(newSocket,buffer,RCVBUFF,0);
                    buffer[size] = '\0';
                    printf("%s", buffer);
                }while(strcmp(buffer,".\n") != 0);
                printf("~~~~~~~~~~~~~\nMessage NOT YET saved!\n"); //TODO
                break;
            }
            case DEL: {
                printf("DELETE mail\n");
                break;
            }
            default: {
                printf("No valid Input detected!\ntry:\n  READ\n  LIST\n  SEND\n  DEL\n");
                break;
            }
		}
	}
	}while(strncmp(buffer,"quit",4) !=0);
	}

	close(newSocket);
	close(createSocket);
	return 0;
}

void listDir(DIR* mStorageDir) {
    struct dirent* mFile;
    while ((mFile=readdir(mStorageDir))){// if dp is null, there's no more content to read
        if(!strncmp(mFile->d_name,".",1) || !strncmp(mFile->d_name,"..",2)) continue;
        printf("%s\n", mFile->d_name);
    }
}
DIR *searchDir(DIR *mStorageDir, string name, string path){
    string dirName = path+'/'+name; //pathCat(path, name).c_str();

    struct dirent* userDir;
    while ((userDir=readdir(mStorageDir))){// if dp is null, there's no more content to read
        if(!strncmp(userDir->d_name,".",1) || !strncmp(userDir->d_name,"..",2) || userDir->d_type != DT_DIR){
        	continue;
        }
        if((string)userDir->d_name == name){
        	cout << "Found DIR: " << name << endl;
			closedir(mStorageDir);
			return opendir(dirName.c_str());
        }
    }
    mkdir(dirName.c_str(), 777);
    cout << "Created DIR: " << name << endl;
    closedir(mStorageDir);
	return opendir(dirName.c_str());
}

int chooseMode(char buffer[]){
	if(strncmp(buffer, "READ",4) == 0) return 0;
	if(strncmp(buffer,"LIST", 4) == 0) return 1;
	if(strncmp(buffer,"SEND", 4) == 0) return 2;
	if(strncmp(buffer,"DEL",3) == 0) return 3;
	return -1;
}

string rcvMessage(int newSocket, char buffer[], bool noMessage){
	if(noMessage){
		if(strncmp(buffer, "QUIT",4) == 0){
			return "";	
		}

	}else {
    	do{
            size = recv(newSocket,buffer,RCVBUFF,0);
            buffer[size] = '\0';
            printf("%s", buffer);
            //TODO save message
        }while(strcmp(buffer,".\n") != 0);		
	}
	size = recv(newSocket,buffer,RCVBUFF,0);
    buffer[size] = '\0';
}


//newSocket,buffer,RCVBUFF,0
















