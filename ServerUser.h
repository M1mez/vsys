#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <dirent.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <vector>
#include <fstream>
#include <cerrno>
#include <locale>

#define DELIMITER ".\n"
#define EDGE "~~~~~~~~~~~~~~~~~~~~~~~~~~"
#define BUFFER 1024

enum{INVALID = -1, READ = 0, LIST, SEND, DEL, QUIT};
enum{ISMESSAGE, NOMESSAGE, ONEORTWO, NUMERIC};
enum{INBOX=1, OUTBOX};


typedef struct Receiver{
	std::string inbox;
	std::string name;
	std::string subject;
} Rec_t;

typedef struct Paths {
	std::string inbox;
	std::string outbox;
	std::string mStorage;
	std::string userPath;
} Path_t;

class ServerUser {
public:
	ServerUser(std::string userName, std::string path, int socket);
	void switchREAD();
	void switchLIST();
	void switchSEND();
	void switchDEL();
	void switchQUIT();
	int chooseMode();
	~ServerUser();
	std::string _userName;
private:
	DIR *_currentDIR;
	DIR *_userDIR;
	DIR *changeDir(DIR *oldDIR, std::string path = "IchKluk");
	DIR *searchDir();
	int _socket;
	char _buffer[BUFFER];
	Path_t _user;
	Rec_t _rec;
	std::string genFileName(std::string counterPart, std::string subject);
	std::string rcvMessage(int option);
	void customMessage(std::string message);
	void deleteMessage(std::string fileName, int option);
	void initFolders(std::string userName);
	void listDir(int option);
	void readFile(std::string fileName, int option);
	void saveMessage(std::vector<std::string> message);
	void sendVector(std::vector<std::string> entries);
	void setReceiver(std::string name, std::string subject);
	void stopSend();
	void sendLogic(std::string message);
	std::string rcvLogic();
};