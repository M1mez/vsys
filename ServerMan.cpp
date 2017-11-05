#include "ServerMan.h"

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
	bool validCred;
	char buffer[BUFFER-1] = {};
	int size;
	string userName;
	string userPass;
	int checkCount = 0;
	
	if(clientSocket > 0) printf("CONNECTED TO NEW CLIENT!\n");
	else {
		printf("ERROR IN CONNECTING TO CLIENT\n");
		return NULL;	
	}



	do{
		if (checkCount == 3) return NULL;
		checkCount++;

		size = recv(clientSocket,buffer,BUFFER-1,0);
		buffer[size-1] = '\0';	
		userName = string(buffer);
		send(clientSocket, DELIMITER, strlen(DELIMITER),0);

		size = recv(clientSocket,buffer,BUFFER-1,0);
		buffer[size-1] = '\0';	
		userPass = string(buffer);
		send(clientSocket, DELIMITER, strlen(DELIMITER),0);

		cout << "userName: " << userName << ", userPass: " << userPass << endl;
		validCred = true; 
	}while(!validCred);


	ServerUser *newUser = new ServerUser(userName, _path, clientSocket);
	_users.push_back(newUser);
	return newUser;	
}

void Manager::removeUser(ServerUser *user){
	delete user;
}

void Manager::switchLogic(int createSocket){
	ServerUser *client = addUser(createSocket);
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