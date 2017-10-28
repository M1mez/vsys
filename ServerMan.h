
#include "ServerUser.h"

/*typedef struct user_t {
	ServerUser *user;
	int socket;
}*/


class Manager {
public:
	Manager(std::string path);
	void removeUser(ServerUser *user);
	ServerUser *addUser(std::string userName, int newSocket);
	~Manager();
private:
	int _conSocket;
	DIR *_mailStorageDir;
	std::string _path;
	std::vector<ServerUser*> _users;
};