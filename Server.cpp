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

	if(argc < 3){
		printf("Please enter Port number and path!\n");
		exit(EXIT_FAILURE);
	}

	//CHECK IF FIRST ARGUMENT WAS PORT OR PATH
	bool portFirst = isdigit(argv[1][0]);
	int port = portFirst ? atoi(argv[1]) : atoi(argv[2]);
	string path = portFirst ? string(argv[2]) : string(argv[1]);

	cout << "PORT: " << port << endl
		 << "PATH: " << path << endl;


    Manager *man = new Manager(port, path);

    //IF BIND FAILS, EXIT
    if(bind (man->_conSocket, (struct sockaddr *) &man->address, sizeof(man->address)) !=0){
		perror("bind_error");
		exit(EXIT_FAILURE);
	}
	
	//IF LISTENING FAILS, EXIT
	if (listen(man->_conSocket, 5) < 0) {
		perror("listen error");
		exit(EXIT_FAILURE);
	}

	
	//vector<thread> th;
	do{
		int clientSocket = accept(man->_conSocket, (struct sockaddr *)&man->clientAddress, &man->addrlen);
		//th.push_back(thread(&Manager::switchLogic, man, clientSocket));
		thread newT(&Manager::switchLogic, man, clientSocket);
		newT.detach();
		printf("Waiting for connections. \n");
	}while(1);
	delete man;
	return 0;
}