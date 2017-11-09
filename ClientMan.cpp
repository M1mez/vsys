#include "ClientMan.h"

using namespace std;

Manager::Manager(int port, string ip): _port(port), _ip(ip){
	
	//CONNECTION IS ESTABLISHED AND CONNECTION INFORMATION SET ACCORDINGLY
	int socketInt;
	if((socketInt = socket(AF_INET,SOCK_STREAM,0)) == -1){
		printf("Error\n");
		exit(EXIT_FAILURE);
	}

	memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_port = htons(port);
	inet_aton(ip.c_str(), &_address.sin_addr);

	//EXIT IF CONNECTION CANNOT BE ESTABLISHED
	if(connect (socketInt, (struct sockaddr *) &_address,sizeof(_address)) != 0){
		perror("Could not connect to server");
		exit(EXIT_FAILURE);
	}
		
	//AFTER CONNECTION WAS ESTABLISHED, CREATE USER
	printf("Connected to server.\n");
	_user = addUser(socketInt);
}

bool Manager::validUser(){

	//CHECK WHETHER A USER WAS CREATED OR NOT
	return _user;
}


void Manager::switchLogic(){
	string str;
	int failCounter = 0;
	int option = -1;
	do {
    	cout << "##########################" << endl
	         << "Type one of the following:" << endl
    		 << "READ" << endl
    		 << "LIST" << endl
    		 << "SEND" << endl
    		 << "DEL"  << endl
    		 << "QUIT" << endl
    		 << "########" << endl;
        
        //INPUT MENU CHOICE AND CLEAR SCREEN AFTERWARDS FOR BETTER USER EXPERIENCE
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
	usleep(1);
	}while(option != QUIT && failCounter < 5);
	cout << endl << endl << endl << endl 
		 << "User quit his session" 
		 <<	((failCounter == 5) ? 
		 	" by 5 times invalid input!" :
		 	", see you next time!") << endl
		 << endl << endl << endl << endl;
}

ClientUser* Manager::addUser(int socket){

	//CREATE NEW USER, BUT DELETE HIM RIGHT AFTER, IF HE ENTERS INVALID CREDENTIALS
	ClientUser *newUser = new ClientUser(socket);
	if (!newUser->_isValid){
		delete newUser;
		return NULL;
	}
	return newUser;
}


void Manager::clearScreen(){

	//CLEAR SCREEN FOR BETTER USER EXPERIENCE

		// Assume WINDOWS
	#ifdef WINDOWS
		system("cls");
	#else
		// Assume POSIX
		system ("clear");
	#endif
}

Manager::~Manager(){
	
	//WHEN MANAGER IS SHUT DOWN, USER IS DELETED AS WELL
	delete _user;
}