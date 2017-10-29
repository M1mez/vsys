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

	printf("%s %s\n", argv[1],argv[2]);

	int size;
	char buffer[BUFFER];
	struct sockaddr_in address;
	struct sockaddr_in clientAddress;
	socklen_t addrlen;

    Manager *man = new Manager(string(argv[2]));

    memset(&address,0,sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(atoi(argv[1]));

	man->_conSocket = socket(AF_INET,SOCK_STREAM,0);

	if(bind (man->_conSocket, (struct sockaddr *) &address, sizeof(address)) !=0){
		perror("bind_error");
		exit(EXIT_FAILURE);
	}


	

	
	listen(man->_conSocket, 5);
	addrlen = sizeof(struct sockaddr_in);

	cout << "OPTIONS ARE: " << endl << " INVALID: " << INVALID << endl << " READ: " << READ << endl << " LIST " << LIST
		 << endl << " SEND: " << SEND << endl << " DEL: " << DEL << endl << " QUIT: " << QUIT << endl;
	
	vector<thread*> th;

	while(1){
		printf("Waiting for connections. \n");

		int clientSocket = accept(man->_conSocket, (struct sockaddr *)&clientAddress, &addrlen);


		man->switchLogic(clientSocket);

		//thread hans(man->switchLogic, clientSocket);
		
	}
	delete man;
	return 0;
}