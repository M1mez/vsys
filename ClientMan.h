

#include "ClientUser.h"

enum{READ, LIST, SEND, DEL, QUIT};
enum{ISMESSAGE, NOMESSAGE, ONEORTWO, NUMERIC};

class Manager {
public:
	Manager();
	ClientUser *createUser(int socket);
	~Manager();
private:
	int _conSocket;
};