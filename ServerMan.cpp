#include "ServerMan.h"

using namespace std;

Manager::Manager(string path){
	_path = path + "/mailStorage/";

}

ServerUser* Manager::addUser(int clientSocket){
	if(clientSocket > 0) printf("CONNECTED TO NEW CLIENT!\n");
	else printf("ERROR IN CONNECTING TO CLIENT\n");

	// TODO: VIELLEICHT statischer Username??

	ServerUser *newUser = new ServerUser(to_string(clientSocket), _path, clientSocket);
	_users.push_back(newUser);
	return newUser;
}

void Manager::removeUser(ServerUser *user){
	delete user;
}

Manager::~Manager(){
	close(_conSocket);
}