/*
	Client file
	Stefan Mühlbacher & Johannes Fessler
	uid: if16b011 &  if16b042
*/

#include "ClientMan.h"

using namespace std;

int main(int argc, char **argv){

	if(argc < 2 ){
		printf("Please enter IP address and Port number\n");
		exit(EXIT_FAILURE);
	}

	printf("Client started\n");

	int port = atoi(argv[1]);
	string ip = string(argv[2]);

	Manager man(port, ip);
	man.switchLogic();

    return 0;
}




















