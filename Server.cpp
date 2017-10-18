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
#include <csignal>
#include <ctype.h>
#include <fstream>
#include <ctime>
#include <sstream>
#include <time.h>

#define SNDBUFF 1024
#define RCVBUFF 1023
#define DELIMITER ".\n"

using namespace std;

enum{READ, LIST, SEND, DEL, QUIT};
enum{ISMESSAGE, NOMESSAGE, ONEORTWO};
enum{INBOX=1, OUTBOX};
typedef struct Information {
	DIR* mStorageDir;
	int newS;
	int createS;
	string path;
	int box;
	string user;
	string sender;
	string receiver;
} Information;

void listDir(Information *info, DIR *target);
int chooseMode(char buffer[]);
DIR *searchDir(Information *info, string name);
string rcvMessage(Information *info, int option);
DIR *inOrOut(Information *info);
void sendVector(Information *info, vector<string>);
void readFile(Information *info, DIR *target, string fileName);
void customMessage(Information *info, string message = "");
void saveMessage(Information *info, vector<string> message);
void deleteMessage(Information *info, string user, string fileName, int option);

int main(int argc, char **argv){
	srand(time(NULL));
	//signal(SIGPIPE, SIG_IGN);
	setbuf(stdin, NULL);
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
		                printf("READ mail\n");

		                bool isQuit = false;
			            string user = rcvMessage(&info, NOMESSAGE);
			            	if(strncasecmp(user.c_str(), "QUIT", 4) == 0) isQuit = true;
			            int option = stoi(rcvMessage(&info, ONEORTWO));
			            string fileName = rcvMessage(&info, NOMESSAGE);
			            	if(strncasecmp(fileName.c_str(), "QUIT", 4) == 0) isQuit = true;
							if (isQuit) option = QUIT;

			            switch(option){
			            	case INBOX: {
			            		info.user = user;
			            		info.box = INBOX;
			            		DIR *listDirPtr = inOrOut(&info);
			            		if(listDirPtr == NULL){
			            			string message = "An ERROR happened!";
			            			customMessage(&info, message);
			            			break;
			            		}
			            		readFile(&info, listDirPtr, fileName);
			            		closedir(listDirPtr);
								info.mStorageDir = opendir(info.path.c_str());
								break;
			            	}

			            	case OUTBOX: {
			            		info.user = user;
			            		info.box = OUTBOX;
			            		DIR *listDirPtr = inOrOut(&info);
			            		readFile(&info, listDirPtr, fileName);
			            		closedir(listDirPtr);
								info.mStorageDir = opendir(info.path.c_str());
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
			        case LIST: {
			            printf("LIST mails: \n\n");
			            string user = rcvMessage(&info, NOMESSAGE);
			            //cout << "USER: " << user << endl;
			            int option = stoi(rcvMessage(&info, ONEORTWO));
			            //cout << "OPTION: " << option << endl;

			            switch(option){
			            	case INBOX: {
			            		info.user = user;
			            		info.box = INBOX;
			            		DIR *listDirPtr = inOrOut(&info);
			            		listDir(&info, listDirPtr);
			            		closedir(listDirPtr);
								info.mStorageDir = opendir(info.path.c_str());
								break;
			            	}

			            	case OUTBOX: {
			            		info.user = user;
			            		info.box = OUTBOX;
			            		DIR *listDirPtr = inOrOut(&info);
			            		listDir(&info, listDirPtr);
			            		closedir(listDirPtr);
								info.mStorageDir = opendir(info.path.c_str());
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
			        		sendStep[rcvCount] = rcvMessage(&info, NOMESSAGE);
			        		if (sendStep[rcvCount].empty()) {
			        			isQuit = true;
			        			break;
			        		}
			        		cout << sendInfo[rcvCount] << sendStep[rcvCount] << endl;
			        		rcvCount++;
			        	}while(rcvCount < 3);
			        	if (isQuit) break;
			        	cout << sendStep[0] + " " << sendStep[1] + " " << sendStep[2] << endl;
			        	info.sender = sendStep[0];
			        	info.receiver = sendStep[1];

			        	searchDir(&info, sendStep[0]);
			        	searchDir(&info, sendStep[1]);
			        	rcvMessage(&info, ISMESSAGE);

			        	cout << "Message saved in: " << "PLACEHOLDER" << endl;
			            
			            break;
			        }
			        case DEL: {
			            printf("DELETE mail\n");
			            string user = rcvMessage(&info, NOMESSAGE);
			            int option = stoi(rcvMessage(&info, ONEORTWO));
			            string fileName = rcvMessage(&info, NOMESSAGE);

			            deleteMessage(&info, user, fileName, option);
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

void listDir(Information *info, DIR *target) {
    struct dirent *mFile;
    vector<string> entries;

    while ((mFile=readdir(target))){
        if(!strncasecmp(mFile->d_name,".",1) || !strncasecmp(mFile->d_name,"..",2)) continue;
        entries.push_back((string)mFile->d_name);
    }
    if (entries.empty()){
    	customMessage(info, "NO files where found!");
    }
    sendVector(info, entries);
}

DIR *searchDir(Information *info, string name){
    string dirName(info->path);
    dirName += "/";
    dirName += name;

    struct dirent *userDir;
    while ((userDir=readdir(info->mStorageDir)) != NULL){
        if(!strncasecmp(userDir->d_name,".",1) || !strncasecmp(userDir->d_name,"..",2) || userDir->d_type != DT_DIR){
        	continue;
        }
        if((string)userDir->d_name == name){
        	//cout << "Found DIR: " << dirName << endl;
    		//if (closedir(info->mStorageDir)) cout << "closedir not successful!!!";
			//else cout << "closed mailStorage" << endl;
			DIR *tmpDir = opendir(dirName.c_str());
			return tmpDir;
        }
    	usleep(10);
    }

    mkdir(dirName.c_str(), 777);
    mkdir((dirName+"/inbox").c_str(), 777);
    mkdir((dirName+"/outbox").c_str(), 777);

    //cout << "Created DIR: " << info->user << endl;
    closedir(info->mStorageDir);
	//else cout << "closed mailStorage after made dir" << endl;
	return opendir(dirName.c_str());
}

//string searchFile(Information *info, string fileName)

void readFile(Information *info, DIR *target, string fileName) {
	struct dirent *box;
	if(!(fileName.substr( fileName.length() - 4 ) == ".txt")) fileName += ".txt";
	bool fileFound = false;

	while ((box=readdir(target)) != NULL){
        if(!strncasecmp(box->d_name,".",1) || !strncasecmp(box->d_name,"..",2)){
        	continue;
        }

        if((string)box->d_name == fileName){
        	fileFound = true;
        	//cout << "Found File: " << fileName << endl;
        	string path = (string(info->path) + "/" + info->user + (info->box == OUTBOX ? "/outbox/" : "/inbox/") + fileName);
        	//cout << "PATH TO FILE: " << path << endl;
        	vector<string> message;
        	ifstream file;
			file.open(path);

			string doneRead;

			while(getline(file,doneRead))
			{		
				message.push_back(doneRead);
				cout << doneRead << endl;
			}
			//for (string i : message) cout << i << endl;
			file.close();

			if(message.empty()) {
				send(info->newS, "File was empty!", strlen("File was empty!"), 0);
				rcvMessage(info, NOMESSAGE);
				send(info->newS, DELIMITER, strlen(DELIMITER), 0);
			} else {
				sendVector(info, message);
			}
        }
    }
    if (!fileFound) customMessage(info, "That file was NOT found!");
}

DIR *inOrOut(Information *info){
	struct dirent *dest;
	string currentDir = (string(info->path) + "/" + info->user + (info->box == OUTBOX ? "/outbox" : "/inbox"));

	DIR *userDir = searchDir(info, info->user);
	while ((dest=readdir(userDir)) != NULL){
        if(!strncasecmp(dest->d_name,".",1) || !strncasecmp(dest->d_name,"..",2) || dest->d_type != DT_DIR){
        	continue;
        }
        //cout << currentDir << endl;
        switch(info->box){
			case INBOX: {
				if(strncmp(dest->d_name, "inbox", 5) == 0){
					if(closedir(userDir)) cout << "could not close dir in inOrOut" << endl; 
					return opendir(currentDir.c_str());
				}
				break;
			}
			case OUTBOX: {
				if(strncmp(dest->d_name, "outbox", 6) == 0){
					if(closedir(userDir)) cout << "could not close dir in inOrOut" << endl; 
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

string rcvMessage(Information *info, int option){
	char buffer[RCVBUFF] = {};
	int size = recv(info->newS,buffer,RCVBUFF,0);
	buffer[size-1] = '\0';
	string tmp(buffer);
	memset(buffer, '\0', size);

	switch(option){
		case NOMESSAGE: {
			if((strncasecmp(tmp.c_str(), "QUIT", 4) == 0) || tmp == ".\n") {
				return "QUIT";	
			}
			return tmp;
		}
		case ISMESSAGE: {
			vector<string> newMessage;
	    	do{
	            size = recv(info->newS,buffer,RCVBUFF,0);
	            buffer[size] = '\0';
	            printf("%s", buffer);
	            newMessage.push_back((string)buffer);
	        }while(strcmp(buffer,DELIMITER) != 0);	
	        saveMessage(info, newMessage);
	        break;
		}
		case ONEORTWO: {
			if(tmp == "9") return "4";
			return tmp;
		}
		default: {
			return "";
		}
	}
	return "";
}


void sendVector(Information *info, vector<string> entries){
	string edges = "~~~~~~~~~~~~~~~\n";
	send(info->newS, edges.c_str(), strlen(edges.c_str()),0);
	for (string i : entries){
		//i += '\n';
		send(info->newS, i.c_str(), strlen(i.c_str()),0);
		rcvMessage(info, NOMESSAGE);
	}
	send(info->newS, edges.c_str(), strlen(edges.c_str()),0);
	rcvMessage(info, NOMESSAGE);
	send(info->newS, DELIMITER, strlen(DELIMITER),0);
}

void customMessage(Information *info, string message){
	vector<string> vectorError;
	vectorError.push_back(message);
	sendVector(info, vectorError);
}

void saveMessage(Information *info, vector<string> message){
	
/*
	string senderUID = info->receiver;
	string receiverUID = info->sender;
	cout << senderUID;

	string senderDir =   (string(info->path) + "/" + info->sender + "/outbox");
	string receiverDir = (string(info->path) + "/" + info->receiver + "/inbox");
	cout << senderDir;

	FILE *senderFile = fopen(senderDir.c_str() ,"a");
	FILE *receiverFile = fopen(receiverDir.c_str() ,"a");

	for (string i : message){
		fprintf(senderFile,   "%s\n", i.c_str());
		fprintf(receiverFile, "%s\n", i.c_str());
	}

	fclose(senderFile);
	fclose(receiverFile);*/
}


void deleteMessage(Information *info, string user, string fileName, int option){
	if(!(fileName.substr( fileName.length() - 4 ) == ".txt")) fileName += ".txt";
	string folder = (string(info->path) + "/" + user + (option == INBOX ? "/inbox/" : "/outbox/"));
	string filePath = folder + fileName;

	if(remove(filePath.c_str()) != 0){
		string noFileFound = "No File found with name: \"" + fileName + "\"";
		customMessage(info, noFileFound);
	}else {
		string fileDeleted = "File: \"" + fileName + "\" was successfully deleted!";
		customMessage(info, fileDeleted);
	}

}






