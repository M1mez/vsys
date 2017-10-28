#include "ServerMan.h"

using namespace std;

Manager::Manager(string path) : _path(path + "/mailStorage/"){
	_mailStorageDir = opendir(_path.c_str());
	_conSocket = socket(AF_INET,SOCK_STREAM,0);
}

ServerUser* Manager::addUser(string userName, int newSocket){
	ServerUser *newUser = new ServerUser(userName, _path, newSocket);
	_users.push_back(newUser);
	return newUser;
}

void Manager::removeUser(ServerUser *user){
	delete user;
}

Manager::~Manager(){
	close(_conSocket);
}