/*
	Server file
	Stefan Mühlbacher & Johannes Fessler
	uid: if16b011 &  if16b042
*/

#include "ServerMan.h"

using namespace std;


int main(int argc, char **argv){
	srand(time(NULL));
	setbuf(stdin, NULL);

	if(argc < 2){
		printf("Please enter Port number and path!\n");
		exit(EXIT_FAILURE);
	}

	int port = atoi(argv[1]);
	string path = string(argv[2]);
    Manager *man = new Manager(port, path);

	cout << "PORT: " << port << endl
		 << "PATH: " << path << endl;

    if(bind (man->_conSocket, (struct sockaddr *) &man->address, sizeof(man->address)) !=0){
		perror("bind_error");
		exit(EXIT_FAILURE);
	}
	
	listen(man->_conSocket, 5);

	cout << "OPTIONS ARE: " << endl 
		 << " INVALID: " << INVALID << endl 
		 <<  " READ: " << READ << endl 
		 << " LIST " << LIST << endl 
		 << " SEND: " << SEND << endl 
		 << " DEL: " << DEL << endl 
		 << " QUIT: " << QUIT << endl;
	
	vector<thread*> th;

	while(1){
		printf("Waiting for connections. \n");

		int clientSocket = accept(man->_conSocket, (struct sockaddr *)&man->clientAddress, &man->addrlen);


		man->switchLogic(clientSocket);

		//thread hans(man->switchLogic, clientSocket);
		
	}
	delete man;
	return 0;
}