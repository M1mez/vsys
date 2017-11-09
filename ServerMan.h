#include <thread>
#include <map>

#include "ServerUser.h"


class Manager {
public:
	Manager(int port, std::string path);
	int _conSocket;
	ServerUser *addUser(int clientSocket);
	socklen_t addrlen;
	struct sockaddr_in address;
	struct sockaddr_in clientAddress;
	void removeUser(ServerUser *user);
	void switchLogic(int createSocket);
	~Manager();
private:
	char _buffer[BUFFER] = {};
	std::string _path;
	std::string getIP(struct sockaddr_in clientAddr);
	void blockUser(std::string IP);
	std::map<std::string, long int> blockedIPs;
};