/*
	Server file
	Stefan Mühlbacher & Johannes Fessler
	uid: if16b011 &  if16b042
*/

#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
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
#include <vector>
#include <algorithm>

#define SNDBUFF 1024
#define RCVBUFF 1023

using namespace std;

enum{READ, LIST, SEND, DEL, QUIT};
enum{ISMESSAGE, NOMESSAGE};
enum{INBOX=1, OUTBOX};
typedef struct Information {
	DIR* mStorageDir;
	int newS;
	int createS;
	string path;
} Information;

vector<string> listDir(DIR *target);
int chooseMode(char buffer[]);
DIR *searchDir(Information *info, string name);
string rcvMessage(Information *info, bool noMessage, string sendInfo[3]);
DIR *inOrOut(Information *info, string user, int option);

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
	char buffer[1024];
	memset(&address,0,sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(atoi(argv[1]));

    //directory
    string path(argv[2]);

	Information info;
	info.path = string(argv[2]);
	info.mStorageDir = opendir(info.path.c_str());
	info.createS = socket(AF_INET,SOCK_STREAM,0);
	


	if(bind (info.createS, (struct sockaddr *) &address, sizeof(address)) !=0){
		perror("bind_error");
		exit(EXIT_FAILURE);
	}


	listen(info.createS, 5);
	addrlen = sizeof(struct sockaddr_in);

	while(1){
		printf("Waiting for connections. \n");
		info.newS = accept(info.createS, (struct sockaddr *)&clientAddress, &addrlen);
		if(info.newS > 0){
			printf("Connected!\n");
			strcpy(buffer,"Welcome. Please enter your command:\n");
			send(info.newS,buffer,strlen(buffer),0);
		}
		do{
			size = recv(info.newS,buffer,1023,0);
			if(size>0){
				buffer[size] = '\0';

				switch(chooseMode(buffer)) {
			        case READ: {
		                /*printf("READ mail\n");
		                ifstream file;
		                file.open("a.txt");
		                string doneRead;

		                while(getline(file,doneRead))
		                {		 
							const char* pointer = doneRead.c_str();
							*copy(doneRead.begin(), doneRead.end(), buffer) = '\0';
							buffer[strlen(pointer)-1] = '\n';
		                	send(newSocket, buffer, strlen(buffer), 0);
		                }
		                file.close();*/
		                break;
            		}
			        case LIST: {
			            printf("LIST mails: \n\n");
			            string user = rcvMessage(&info, NOMESSAGE, NULL);

			            switch(stoi(rcvMessage(&info, NOMESSAGE, NULL))){
			            	case INBOX: {
			            		cout << "HIER INBOX";
			            		//vector<string> entries(listDir(inOrOut(&info, user, INBOX)));
			            		for (string i : listDir(inOrOut(&info, user, INBOX))){
			            			i += '\n';
			            			cout << i;
			            			send(info.createS, i.c_str(), strlen(i.c_str()),0);
			            		}
			            			send(info.createS, ".\n", strlen(".\n"),0);
			            		break;
			            	}
			            	case OUTBOX: {
			            		//vector<string> entries(listDir(inOrOut(&info, user, OUTBOX)));
			            		for (string i : listDir(inOrOut(&info, user, OUTBOX))){
		            			send(info.createS, (i + '\n').c_str(), strlen((i + '\n').c_str()),0);
			            		}
			            		break;
			            	}
			            	case QUIT: {
			            		cout << "User chose to quit to menu!" << endl;
			            		break;
			            	}
			            	default: {
			            		cout << "ERROR IN SERVER LIST" << endl;
			            		break;
			            	}
			            }
			            break;
			        }
			        case SEND: {
			        	string sendInfo[4] = {"Sender: ", "Empfänger: ", "Betreff: ", "Nachricht:\n"};
			        	string sendStep[3];
			        	int rcvCount = 0;
			        	bool isQuit = false;
						
						printf("SEND mail\n");
						do{
			        		sendStep[rcvCount] = rcvMessage(&info, NOMESSAGE, NULL);
			        		if (sendStep[rcvCount].empty()) {
			        			isQuit = true;
			        			break;
			        		}
			        		cout << sendInfo[rcvCount] << sendStep[rcvCount] << endl;
			        		rcvCount++;
			        	}while(rcvCount < 3);
			        	if (isQuit) break;
			        	cout << sendStep[0] + " " << sendStep[1] + " " << sendStep[2] << endl;

			        	cout << sendStep[3];
			        	rcvMessage(&info, ISMESSAGE, sendStep);
			            
			            break;
			        }
			        case DEL: {
			            printf("DELETE mail\n");
			            break;
			        }
			        case QUIT: {
			        	printf("User quit his/her session. Waiting for new User.\n");
			        	break;
			        }
			        default: {
			            printf("No valid Input detected!\ntry:\n  READ\n  LIST\n  SEND\n  DEL\n");
			            break;
			        }
				}
			}
		}while(strncasecmp(buffer,"quit",4) !=0);
	}

	close(info.newS);
	close(info.createS);
	return 0;
}

vector<string> listDir(DIR *target) {
    struct dirent *mFile;
    vector<string> entries;

    while ((mFile=readdir(target))){// if dp is null, there's no more content to read
        if(!strncasecmp(mFile->d_name,".",1) || !strncasecmp(mFile->d_name,"..",2)) continue;
        printf("%s\n", mFile->d_name);
        entries.push_back(string(mFile->d_name));
    }
    return entries;
}

DIR *searchDir(Information *info, string name){
    string dirName(info->path);
    dirName += "/";
    dirName += name;

    struct dirent *userDir;
    while ((userDir=readdir(info->mStorageDir)) != NULL){// if dp is null, there's no more content to read
        if(!strncasecmp(userDir->d_name,".",1) || !strncasecmp(userDir->d_name,"..",2) || userDir->d_type != DT_DIR){
        	continue;
        }
        if((string)userDir->d_name == name){
        	cout << "Found DIR: " << dirName << endl;
    		if (closedir(info->mStorageDir)) cout << "closdir not successful!!!";
			else cout << "closed mailStorage" << endl;
			DIR *tmpDir = opendir(dirName.c_str());
			if(tmpDir == NULL) cout << "WHYTHEFUCK!!!!"<<endl;
			return tmpDir;
        }
    	usleep(10);
    }
    mkdir(dirName.c_str(), 777);
    cout << "Created DIR: " << name << endl;
    if (closedir(info->mStorageDir)) cout << "closdir not successful!!!";
	cout << "closed mailStorage after made dir" << endl;
	return opendir(dirName.c_str());
}

DIR *inOrOut(Information *info, string user, int option){
	struct dirent *dest;
	/*string currentDir(get_current_dir_name());
	currentDir += ("/mailStorage/" + user + "/" + (option == OUTBOX ? "outbox" : "inbox"));
	cout << "currentDir: " << currentDir << endl;
	//closedir(info->mStorageDir);
	DIR *userDir = opendir(currentDir.c_str());*/

	string currentDir = (string(info->path) + "/" + user + (option == OUTBOX ? "/outbox" : "/inbox"));

	cout << currentDir << " | in inorout" << endl;

	DIR *userDir = searchDir(info, user);
	while ((dest=readdir(userDir)) != NULL){// if dp is null, there's no more content to read
        if(!strncasecmp(dest->d_name,".",1) || !strncasecmp(dest->d_name,"..",2) || dest->d_type != DT_DIR){
        	continue;
        }
        printf("WHAT CASE? %s\n", dest->d_name);
        cout << currentDir << endl;
        switch(option){
			case INBOX: {
				if(strncmp(dest->d_name, "inbox", 5) == 0){
					printf("WASCASEINBOX\n" );
					return opendir(currentDir.c_str());
				}
				break;
			}
			case OUTBOX: {
				if(strncmp(dest->d_name, "outbox", 6) == 0){
					printf("WASCASEOUTBOX\n" );
					return opendir(currentDir.c_str());
				}
				break;
			}
		}
    }
    return NULL;

}

int chooseMode(char buffer[]){
	if(strncasecmp(buffer, "READ", 4) == 0) return 0;
	if(strncasecmp(buffer, "LIST", 4) == 0) return 1;
	if(strncasecmp(buffer, "SEND", 4) == 0) return 2;
	if(strncasecmp(buffer, "DEL",  3) == 0) return 3;
	if(strncasecmp(buffer, "QUIT", 4) == 0) return 4;
	return -1;
}

string rcvMessage(Information *info, bool noMessage, string sendInfo[3]){
	char buffer[RCVBUFF] = {};
	int size;
	if(noMessage){
		size = recv(info->newS,buffer,RCVBUFF,0);
		//printf("SIZE: %d\n", size);
    	buffer[size-1] = '\0';

    	//int i = atoi(buffer);
    	//cout << "WHAT??" << i << endl;
    	printf("Received: _%s_\n", buffer);
    	//printf("buffer[0]: %c|\n", buffer[0]);
		//printf("X%sX\n",buffer );
		//cout << "|" << buffer << "|";
    	string tmp(buffer);

		if(tmp == "QUIT") return "QUIT";	
		if(tmp == "9") return "4";
		return tmp;

	}else {
    	do{
            size = recv(info->newS,buffer,RCVBUFF,0);
            buffer[size] = '\0';
            printf("%s", buffer);
            //TODO save message
        }while(strcmp(buffer,".\n") != 0);		
	}
	return "";
}
















