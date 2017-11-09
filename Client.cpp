/*
	Client file
	Stefan Mühlbacher & Johannes Fessler
	uid: if16b011 &  if16b042
*/

#include "ClientMan.h"

using namespace std;

int main(int argc, char **argv){

	//USER HAS TO PROVIDE 2 ARGUMENTS
	if(argc < 2 ){
		printf("Please enter IP address and Port number\n");
		exit(EXIT_FAILURE);
	}

	int port = atoi(argv[1]);
	string ip = string(argv[2]);

	//AFTER USER ENTERED VALID INFORMATION, JUMP TO MENU
	Manager man(port, ip);
	if(man.validUser()){
		man.switchLogic();
	}

    return 0;
}




















