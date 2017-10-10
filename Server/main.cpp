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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

enum{READ, LIST, SEND, DEL};

int main(int argc, char **argv){
	FILE *fp;
	char buff[20], buffer[1024];
	DIR *dirp;
	struct stat sb;
	int createSocket, newSocket;
	socklen_t addrlen;
	int size;
	struct sockaddr_in address, clientAddress;

	if(argc < 2){
		printf("Please enter Port number and path!\n");
		exit(EXIT_FAILURE);
	}

   //DIR *mStorage = opendir(argv[2]);

    printf("%s", argv[2]);
	//printf("%s %s\n", argv[1],argv[2]);

	createSocket = socket(AF_INET,SOCK_STREAM,0);
	memset(&address,0,sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(atoi(argv[1]));

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
        int option = -1;
	do{
        option = -1;
		size = recv(newSocket,buffer,1023,0);
		if(size>0){
			buffer[size] = '\0';
			printf("%s", buffer);

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
                printf("READ mail\n");
                break;
            }
            case LIST: {
                printf("LIST mails: \n\n");
                break;
            }
            case SEND: {
                printf("SEND mail\n");
                size = recv(newSocket, buffer,1023, 0);
                buffer[size] = '\0';
                printf("Sender: %s", buffer);
                size = recv(newSocket,buffer,1023,0);
                buffer[size] = '\0';
                printf("Empfänger: %s",buffer);
                size = recv(newSocket, buffer, 1023,0);
                buffer[size] = '\0';
                printf("Betreff: %s", buffer);
                printf("Nachricht: ");
                do{
                    size = recv(newSocket,buffer,1023,0);
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


	fp = fopen("allowedAdresses","r");
	//reading from file
	while(fscanf(fp,"%s",buff) != EOF ){
		printf("%s \n", buff);
		//check if for each id is a folder
	/*	if((dirp = opendir(buff)) == NULL){
			perror("Directory doesn't exist.Creating one. \n");
		}else{
			printf("Directory exists. \n");
		}
	*/
		if(stat(buff,&sb) == -1){
			printf("New User found. Creating directory for user: %s\n",buff );
			//create a new directory with inbox and outbox for each user
			mkdir(buff,0700);
			sprintf(buff,"%s/inbox",buff);
			mkdir(buff,0700);
			sprintf(buff, "%s/../outbox", buff);
			mkdir(buff,0700);
		}
	}
	fclose(fp);

	close(newSocket);
	close(createSocket);
	//closedir(*mStorage);
	return 0;
}
