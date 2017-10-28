#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>
#include <uuid/uuid.h>
#include <vector>
#include <algorithm>

#define DELIMITER ".\n"
#define BUFFER 1024

enum{INVALID = -1, READ, LIST, SEND, DEL, QUIT};
enum{ISMESSAGE, NOMESSAGE, ONEORTWO, NUMERIC};
enum{INBOX=1, OUTBOX};


typedef struct Receiver{
	std::string name;
	std::string inbox;
	std::string subject;
} Rec_t;

class ServerUser {
public:
	ServerUser(std::string userName, std::string path, int socket);
	void switchREAD();
	void switchLIST();
	void switchSEND();
	void switchDEL();
	void switchQUIT();
	int chooseMode(std::string str);
	~ServerUser();
private:
	int _socket;
	std::string _userName;
	std::string _path;
	std::string _inbox;
	std::string _outbox;
	Rec_t _rec;
	void setReceiver(std::string name);
	std::string genFileName(std::string counterPart, std::string subject);
	void deleteMessage(std::string fileName, int option);
	void customMessage(std::string message);
	void sendVector(std::vector<std::string> entries);
	void stopSend();
	void saveMessage(std::vector<std::string> message);
	std::string rcvMessage(int option);
};