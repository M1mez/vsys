#include "ServerMan.h"

using namespace std;

Manager::Manager(string path){
	_path = path + "/mailStorage/";

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
		memset(buffer, '\0', size);

		size = recv(clientSocket,buffer,BUFFER-1,0);
		buffer[size-1] = '\0';
		userPass = string(buffer);
		memset(buffer, '\0', size);

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

	if (client == NULL){
		return;
	}

	do{
		option = client->chooseMode();
		cout << "OPTION is " << option << endl;

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
		
	}while(option != QUIT);
	removeUser(client);
}

Manager::~Manager(){
	close(_conSocket);
}