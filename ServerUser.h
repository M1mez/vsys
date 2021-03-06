#include <algorithm>
#include <cctype>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <locale>
#include <netinet/in.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <vector>

#define DELIMITER ".\n"
#define EDGE "~~~~~~~~~~~~~~~~~~~~~~~~~~"
#define BUFFER 1024


enum{INVALID = -1, READ = 0, LIST, SEND, DEL, QUIT};
enum{ISMESSAGE, NOMESSAGE, ONEORTWO, NUMERIC};
enum{INBOX=1, OUTBOX};

//RECEIVER FILES, SET WHEN USER CHOSE SEND AND PROVIDED INFORMATION
typedef struct Receiver{
	std::string inbox;
	std::string name;
	std::string subject;
} Rec_t;

//USERPATHS ARE SET AFTER USER LOGGED IN SUCCESSFULLY
typedef struct Paths {
	std::string inbox;
	std::string outbox;
	std::string mStorage;
	std::string userPath;
} Path_t;

class ServerUser {
public:
	ServerUser(std::string userName, std::string path, int socket);
	int chooseMode();
	void switchDEL();
	void switchLIST();
	void switchQUIT();
	void switchREAD();
	void switchSEND();
	std::string _userName;
	~ServerUser();
private:
	char _buffer[BUFFER];
	DIR *_currentDIR;
	DIR *_userDIR;
	DIR *changeDir(DIR *oldDIR, std::string path = "IchKluk");
	DIR *searchDir();
	int _socket;
	Path_t _user;
	Rec_t _rec;
	std::string genFileName(std::string counterPart, std::string subject);
	std::string rcvLogic();
	std::string rcvMessage(int option);
	struct sockaddr_in _clientAddr;
	void customMessage(std::string message);
	void deleteMessage(std::string fileName, int option);
	void initFolders(std::string userName);
	void listDir(int option);
	void readFile(std::string fileName, int option);
	void receiveFile();
	void saveMessage(std::vector<std::string> message);
	void sendLogic(std::string message);
	void sendVector(std::vector<std::string> entries);
	void setReceiver(std::string name, std::string subject);
	void stopSend();
};