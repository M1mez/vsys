#include <algorithm>
#include <algorithm>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <locale>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>

#define DELIMITER ".\n"
#define EDGE "~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
#define BUFFER 1024

enum{INVALID = -1, READ = 0, LIST, SEND, DEL, QUIT};
enum{ISMESSAGE, NOMESSAGE, ONEORTWO, NUMERIC};
enum{INBOX=1, OUTBOX};


class ClientUser {
public:
	ClientUser(int socket);
	void switchREAD();
	void switchLIST();
	void switchSEND();
	void switchDEL();
	void switchQUIT();
	int chooseMode();
	void rcvVector();
	bool stringCompare(std::string src, std::string tar);
	~ClientUser();
private:
	void sendLogic(std::string message);
	//void sendLogic(char *message);
	bool sendFile();
	char _buffer[BUFFER] = {};
	void stopSend();
	int _socket;
	bool sendMessage(int maxInput, int messageType);
	std::string rcvLogic();
	std::string _name;
};