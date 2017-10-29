
#include "ServerUser.h"

/*typedef struct user_t {
	ServerUser *user;
	int socket;
}*/


class Manager {
public:
	Manager(std::string path);
	ServerUser *addUser(int clientSocket);
	void removeUser(ServerUser *user);
	~Manager();
	int _conSocket;
private:
	std::vector<ServerUser*> _users;
	std::string _path;
	
	char _buffer[BUFFER] = {};
};