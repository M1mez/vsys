
#include "ClientUser.h"

class Manager {
public:
	Manager(int port, std::string ip);
	ClientUser *createUser();
	void switchLogic();
	~Manager();
private:
	ClientUser *_user;
	int _conSocket;
	int _port;
	std::string _ip;
	struct sockaddr_in _address;
	void clearScreen();

};