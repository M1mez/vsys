#include <arpa/inet.h>
#include "ServerMan.h"


#ifndef LDAP_CPP
#include "ldap.cpp"
#endif

#define BLOCKTIME 30*60

using namespace std;

Manager::Manager(int port, string path){

	//STANDARD MAILPATH IS SET AND CONNECTION INFORMATION IS SAVED
	_path = path + "/mailStorage/";
	mkdir((path + "/mailStorage").c_str(), 0777);
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

		printf("userName entered: %s\n", userName);

		//CHECKED WITH LDAP IF LOGIN IS VALID
		isValid = (loginLDAP(userName, PW) == EXIT_SUCCESS);

		//IF LOGIN IS VALID, RESPOND ACCORDINGLY
		if (isValid){
			strcpy(response, "VALID");
			send(clientSocket, response, strlen(response),0);
			recv(clientSocket, del, BUFFER-1,0);
			memset(response, 0, 10);
			break;

		//IF LOGIN WAS INVALID 3 TIMES, USER IS BLOCKED AND (REALLY, I SWEAR!) BLOCKED FOR SOME TIME
		} else if (checkCount == 0) {
			strcpy(response, "BLOCK");
			send(clientSocket, response, strlen(response),0);
			cout << "SENT STUFF" << endl;
			recv(clientSocket, del, BUFFER-1,0);
			memset(response, 0, 10);
			free(userName);
			free(PW);
			blockUser(IP);
			return NULL;

		//IF LOGIN WAS INVALID < 3 TIMES, USER HAS TO ENTER AGAIN
		} else if (blockedIPs[IP] > (long int)time(NULL)) {
			strcpy(response, "STILLBLOCKED");
			send(clientSocket, response, strlen(response),0);
			recv(clientSocket, del, BUFFER-1,0);
			memset(response, 0, 10);
			free(userName);
			free(PW);
			return NULL;
		} else {
			strcpy(response, "INVALID");
			send(clientSocket, response, strlen(response),0);
			recv(clientSocket, del, BUFFER-1,0);
			memset(response, 0, 10);
			checkCount--;
		}
	}while(!isValid);

	//ALLOCATED SPACE IS FREED AND USER IS CREATED
	string user(userName);
	free(userName);
	free(PW);
	return new ServerUser(user, _path, clientSocket);
}

void Manager::blockUser(string IP){

	//USER REALLY GETS BLOCKED
	cout << "User with IP: " << IP << " is now definitely blocked." << endl 
		 << "Please don't try again, because it really works." << endl; 

	long int nowTime = (long int)time(NULL) + BLOCKTIME;

	blockedIPs[IP] = nowTime;
}

string Manager::getIP(struct sockaddr_in clientAddr){

	//IP IS DERIVED FROM CLIENT CONNECTION INFORMATION
	struct in_addr addr = clientAddr.sin_addr;
	char IP[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr, IP, INET_ADDRSTRLEN);
	return string(IP);
}

void Manager::removeUser(ServerUser *user){

	//WHEN USER CHOOSES QUIT, HIS SESSION ENDS WITH HIM BEING TERMINATED
	delete user;
}

void Manager::switchLogic(int createSocket){
	ServerUser *client;

	//IF USER DID NOT ENTER VALID CREDENTIALS, NULL IS RETURNED AND THREAD ENDS
	if ((client = addUser(createSocket)) == NULL) return;
	
	int failCounter = 0;
	int option = -1; 

	do{
	//CHOOSE OPTIONS ACCORDING TO USER INPUT
		option = client->chooseMode();

		//IF USER INPUTS WRONG INFORMATION 5 TIMES IN A ROW, THREAD ENDS AFTER DELETING HIM
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
	usleep(1);
	}while(option != QUIT && failCounter < 5);
	removeUser(client);
}

Manager::~Manager(){

	//MANAGER IS DELETED AFTER CUTTING CONNECTION
	close(_conSocket);
}
