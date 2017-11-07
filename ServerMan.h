#include <thread>

#include "ServerUser.h"

/*typedef struct user_t {
	ServerUser *user;
	int socket;
}*/


class Manager {
public:
	Manager(int port, std::string path);
	ServerUser *addUser(int clientSocket);
	void removeUser(ServerUser *user);
	void switchLogic(int createSocket);
	~Manager();
	int _conSocket;
	struct sockaddr_in address;
	struct sockaddr_in clientAddress;
	socklen_t addrlen;
private:
	//std::vector<ServerUser*> _users;
	std::string _path;
	std::string getIP(struct sockaddr_in clientAddr);
	
	char _buffer[BUFFER] = {};
};