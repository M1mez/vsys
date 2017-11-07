#include "ClientMan.h"

using namespace std;


Manager::Manager(int port, string ip): _port(port), _ip(ip){
	int socketInt;

	if((socketInt = socket(AF_INET,SOCK_STREAM,0)) == -1){
		printf("Error\n");
		exit(EXIT_FAILURE);
	}

	memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_port = htons(port);
	inet_aton(ip.c_str(), &_address.sin_addr);


	if(connect (socketInt, (struct sockaddr *) &_address,sizeof(_address)) == 0){
		printf("Connected to server.\n");
		/*size = recv(createSocket,buffer,RCVBUFF,0);
		if(size>0){
			buffer[size]='\0';
			printf("%s", buffer);
		}*/
	}else {
		perror("Could not connect to server");
		exit(EXIT_FAILURE);
	}
	_user = addUser(socketInt);
}

bool Manager::validUser(){
	return _user;
}


void Manager::switchLogic(){
	string str;
	int failCounter = 0;
	int option;
	do {
    	cout << "##########################" << endl
	         << "Type one of the following:" << endl
    		 << "READ" << endl
    		 << "LIST" << endl
    		 << "SEND" << endl
    		 << "DEL"  << endl
    		 << "QUIT" << endl
    		 << "########" << endl;
        
        option = _user->chooseMode();
        failCounter = (option == INVALID) ? failCounter + 1 : 0;

        clearScreen();

		switch(option) {
            case READ: {
                _user->switchREAD();
                break;
            }
            case LIST: {
                _user->switchLIST();
                break;
            }
            case SEND: {
                _user->switchSEND();
                break;
            }
            case DEL: {
                _user->switchDEL();
                break;
            }
            case QUIT: { 
                break;
            }
            default: {
            	cout << EDGE << "No valid Input detected!" << endl; 
                break;  
            }
		}

		//fgets(buffer,SNDBUFF,stdin);
		//send(createSocket,buffer,strlen(buffer),0);
	}while(option != QUIT && failCounter < 5);
	cout << "User quit his session" 
		 <<	((failCounter == 5) ? 
		 	" by 5 times invalid input!" :
		 	", see you next time!") << endl;
}

ClientUser* Manager::addUser(int socket)
{
	ClientUser *newUser = new ClientUser(socket);

	return newUser->_isValid ? newUser : NULL;
	
}


void Manager::clearScreen(){
		// Assume WINDOWS
	#ifdef WINDOWS
		system("cls");
	#else
		// Assume POSIX
		system ("clear");
	#endif
}

Manager::~Manager(){
	delete _user;
}