
#include "ClientUser.h"

class Manager {
public:
	Manager(int port, std::string ip);
	void switchLogic();
	ClientUser* addUser(int socket);
	~Manager();
private:
	ClientUser *_user;
	int _port;
	std::string _ip;
	struct sockaddr_in _address;
	void clearScreen();

};