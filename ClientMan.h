#include "ClientUser.h"

class Manager {
public:
	Manager(int port, std::string ip);
	bool validUser();
	ClientUser* addUser(int socket);
	void switchLogic();
	~Manager();
private:
	ClientUser *_user;
	int _port;
	std::string _ip;
	struct sockaddr_in _address;
	void clearScreen();
};