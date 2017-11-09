#include <algorithm>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <locale>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <climits>



#define DELIMITER ".\n"
#define EDGE "~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
#define BUFFER 1024

enum{INVALID = -1, READ = 0, LIST, SEND, DEL, QUIT};
enum{ISMESSAGE, NOMESSAGE, ONEORTWO, NUMERIC};
enum{INBOX=1, OUTBOX};


class ClientUser {
public:
	ClientUser(int socket);
	bool _isValid;
	bool stringCompare(std::string src, std::string tar);
	int chooseMode();
	void rcvVector();
	void switchDEL();
	void switchLIST();
	void switchQUIT();
	void switchREAD();
	void switchSEND();
	~ClientUser();
private:
	bool sendFile();
	bool sendMessage(int maxInput, int messageType);
	char _buffer[BUFFER] = {};
	int _socket;
	std::string _name;
	std::string rcvLogic();
	void sendLogic(std::string message);
};