#include <arpa/inet.h>
#include "ServerMan.h"

#ifndef LDAP_CPP
#include "ldap.cpp"
#endif

using namespace std;

Manager::Manager(int port, string path){
	_path = path + "/mailStorage/";
	memset(&address,0,sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	addrlen = sizeof(struct sockaddr_in);

	_conSocket = socket(AF_INET,SOCK_STREAM,0);
}

ServerUser* Manager::addUser(int clientSocket){
	bool isValid;
	int size;
	int checkCount = 2;
	char del[5];
	char *userName;
	char *PW;
	string IP = getIP(clientAddress);
	char response[10];
	
	if(clientSocket > 0) printf("CONNECTED TO NEW CLIENT!\n");
	else {
		printf("ERROR IN CONNECTING TO CLIENT\n");
		return NULL;	
	}



	do{

		size = recv(clientSocket,_buffer,BUFFER-1,0);
		send(clientSocket, DELIMITER, strlen(DELIMITER),0);
		userName = (char *) malloc(sizeof(char)*size);
		strncpy(userName, _buffer, size);
		memset(_buffer, 0, size);


		size = recv(clientSocket,_buffer,BUFFER-1,0);
		send(clientSocket, DELIMITER, strlen(DELIMITER),0);
		PW = (char *) malloc(sizeof(char)*size);
		strncpy(PW, _buffer, size);
		memset(_buffer, 0, size);


		printf("userName: %s\n", userName);
		//printf("PW:       %s\n", PW);

		isValid = (loginLDAP(userName, PW) == EXIT_SUCCESS);

		if (isValid){
			strcpy(response, "VALID");
			send(clientSocket, response, strlen(response),0);
			recv(clientSocket, del, BUFFER-1,0);
			memset(response, 0, strlen(response));
			break;
		} else if (checkCount == 0) {
			strcpy(response, "BLOCK");
			send(clientSocket, response, strlen(response),0);
			recv(clientSocket, del, BUFFER-1,0);
			memset(response, 0, strlen(response));
			free(userName);
			free(PW);
			blockUser(IP);
			return NULL;
		} else {
			strcpy(response, "INVALID");
			send(clientSocket, response, strlen(response),0);
			recv(clientSocket, del, BUFFER-1,0);
			memset(response, 0, strlen(response));
			checkCount--;
		}


	}while(!isValid);
	string user(userName);
	free(userName);
	free(PW);
	return new ServerUser(user, _path, clientSocket);
}

void Manager::blockUser(string IP){
	cout << "User with IP: " << IP << " is now definitely blocked." << endl 
		 << "Please don't try again, because it really works." << endl; 
}

string Manager::getIP(struct sockaddr_in clientAddr){
	struct in_addr addr = clientAddr.sin_addr;
	char IP[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr, IP, INET_ADDRSTRLEN);

	return string(IP);
}

void Manager::removeUser(ServerUser *user){
	delete user;
}

void Manager::switchLogic(int createSocket){
	ServerUser *client;
	if ((client = addUser(createSocket)) == NULL) {
		return;
	}
	int option; 
	int failCounter = 0;

	if (client == NULL){
		return;
	}

	do{
		option = client->chooseMode();
		failCounter = (option == INVALID) ? failCounter + 1 : 0;

		switch(option) {
	        case READ: {
                client->switchREAD();
                break;
    		}
	        case LIST: {
	            client->switchLIST();
                break;
	        }
	        case SEND: {
	        	client->switchSEND();
                break;
	        }
	        case DEL: {
	            client->switchDEL();
                break;
	        }
	        case QUIT: {
	        	cout << "User: " << client->_userName << " quit his/her session!" << endl;
	        	break;
	        }
	        default: {
	            break;
	        }
		}
	}while(option != QUIT && failCounter < 5);
	removeUser(client);
}

Manager::~Manager(){
	close(_conSocket);
}
