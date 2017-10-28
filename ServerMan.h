
#include "ServerUser.h"

/*typedef struct user_t {
	ServerUser *user;
	int socket;
}*/


class Manager {
public:
	Manager(std::string path);
	ServerUser *addUser(std::string userName, int newSocket);
	void removeUser(ServerUser *user);
	~Manager();
private:
	DIR *_mailStorageDir;
	int _conSocket;
	std::string _path;
	std::vector<ServerUser*> _users;
};