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
	
	while(1){
		printf("Waiting for connections. \n");

		int clientSocket = accept(man->_conSocket, (struct sockaddr *)&clientAddress, &addrlen);
		ServerUser *client = man->addUser(clientSocket);
		cout << "hierDRAUSSEN";
		
		do{
			int option = client->chooseMode();
			
			if(option != INVALID){
				buffer[size] = '\0';

				switch(option) {
			        case READ: {
		                client->switchREAD();
            		}
			        case LIST: {
			            client->switchLIST();
			        }
			        case SEND: {
			        	client->switchSEND();
			        }
			        case DEL: {
			            client->switchDEL();
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
		man->removeUser(client);
	}
	delete man;
	return 0;
}