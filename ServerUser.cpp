#include "ServerUser.h"



using namespace std;

#include <mutex> 

std::mutex writeMutex;

ServerUser::ServerUser(string userName, string path, int socket) :_userName(userName), _socket(socket){
	
	//SET STANDARDPATHS
	_user.mStorage = path;

	//IF USERPATHS DON'T EXIST, CREATE THEM
	initFolders(userName);
	_user.userPath = path + userName + "/"; 
	_user.outbox = path + userName + "/outbox/";
	_user.inbox = path + userName + "/inbox/";

	//SET WORKING DIRECTORY TO USERDIRECTORY
	_currentDIR = changeDir(NULL);

	cout << "User " << _userName << " built up a connection!" << endl;
}


void ServerUser::switchREAD(){
	cout << "User: " << _userName << " chose READ" << endl;

	
	//INBOX OR OUTBOX?
    int option = atoi(rcvMessage(ONEORTWO).c_str());

    //IF QUIT, RETURN TO MENU
	switch(option){
    	case QUIT: {
    		cout << "User chose to quit to menu!" << endl;
    		return;
    	}
    	default: {
    		listDir(option);
    		break;
    	}
    }

    //RECEIVE FILENAME TO READ, IF QUIT, RETURN, ELSE -> READFILE
	string fileName = "";
    if ((fileName = rcvMessage(NOMESSAGE)) != "QUIT") {
    	readFile(fileName, option);
	}
	return;
}

void ServerUser::switchLIST(){
	cout << "User: " << _userName << " chose LIST" << endl;
	
	//INBOX OR OUTBOX?
    int option = atoi(rcvMessage(ONEORTWO).c_str());

    //IF QUIT, RETURN TO MENU
    switch(option){
    	case QUIT: {
    		cout << "User chose to quit to menu!" << endl;
    		break;
    	}
    	default: {
    		listDir(option);
    		break;
    	}
    }
    return;
}

void ServerUser::switchSEND(){
	cout << "User: " << _userName << " chose SEND" << endl;


	string sendInfo[] = {"Receiver: ", "Subject: ", "Message:\n"};
	string sendStep[2];
	int rcvCount = 0;

	//RECEIVER
	if ((sendStep[0] = rcvMessage(NOMESSAGE)) == "QUIT") return;
	cout << sendInfo[0] << sendStep[0] << endl;

	//SUBJECT
	if ((sendStep[1] = rcvMessage(NOMESSAGE)) == "QUIT") return;
	cout << sendInfo[1] << sendStep[1] << endl;

	//SET RECEIVERDATA LIKE PATH AND SUBJECT TO BUILD FILENAME
	setReceiver(sendStep[0], sendStep[1]);

	//RECEIVE THE MESSAGE
	rcvMessage(ISMESSAGE);

	//RECEIVE FILE, IF QUIT IS SENT, RETURN OUT
	receiveFile();

	cout << "INCOMING Message saved in: " << _rec.inbox << endl;
	cout << "OUTGOING Message saved in: " << _user.outbox << endl;
}

void ServerUser::switchDEL(){
	cout << "User: " << _userName << " chose DEL" << endl;

	//INBOX OR OUTBOX?
	int option = stoi(rcvMessage(ONEORTWO));
    
    //RECEIVE FILENAME OR QUIT
    string fileName = rcvMessage(NOMESSAGE);

    if(fileName == "QUIT") option = QUIT;

    switch(option){
    	case QUIT: {
    		cout << "User chose to quit to menu!" << endl;
    		break;
    	}
    	default: {
    		writeMutex.lock();
    		deleteMessage(fileName, option);
    		writeMutex.unlock();
    		break;
    	}
    }
    return;
}

void ServerUser::setReceiver(string name, string subject){

	//SET RECEIVER DATA LIKE PATH, NAME AND SUBJECT
	_rec.name = name;
	_rec.inbox = (_user.mStorage + name + "/inbox/");
	_rec.subject = subject;

	//IF USER DOES NOT YET EXIST, CREATE FOLDERS
	initFolders(name);
}

string ServerUser::genFileName(string counterPart, string subject){
	
	//CREATE RANDOM UUID AND CUT IT TO 8 CHARS
	uuid_t uuid;
    uuid_generate_random(uuid);
    char uuid_str[9];
    uuid_unparse_lower(uuid, uuid_str);
    uuid_str[8] = '\0';
    string uuidStr(uuid_str);

    //TAKE CURRENT TIMESTAMP AND CUT IT TO 4 DIGITS
    string nowTime = to_string(((long int)time(NULL))%1000);

    //RETURN COMBINED FILENAME
    return (counterPart + "-" + subject + "-" + uuidStr + "-" + nowTime + ".txt");
}

void ServerUser::deleteMessage(string fileName, int option){

	//CHECK IF ENDING WAS PROVIDED
	if(!(fileName.substr( fileName.length() - 4 ) == ".txt")) fileName += ".txt";

	//SET FILEPATH FROM INFO PROVIDED
	string filePath = ((option == INBOX) ? _user.inbox : _user.outbox) + fileName;

	//CHECK IF REMOVING IS POSSIBLE AND INFORM CLIENT
	if(remove(filePath.c_str()) != 0){
		customMessage("No File found with name: \"" + fileName + "\"");
	}else {
		customMessage("File: \"" + fileName + "\" was successfully deleted!");
	}
}

void ServerUser::customMessage(string message){

	//SEND VECTOR WITH ONLY ONE LINE, COMMODITY SINCE CLIENT IS WAITING FOR SEVERAL LINES
	vector<string> v;
	v.push_back(message);
	sendVector(v);
}

void ServerUser::listDir(int option) {
    struct dirent *mFile;
    vector<string> entries;
    int incr = 1;
	
	//CHANGE WORKING DIR TEMPORARILY TO CHOSEN DIRECTORY	
	DIR *toList = changeDir(_currentDIR, ((option == INBOX) ? _user.inbox : _user.outbox));

	//ITERATE UNTIL NO MORE FILE IS FOUND AND PUSH ENTRIES TO VECTOR, INCREMENTAL INDEX PRECEDING
    while ((mFile=readdir(toList))){
        if(!strcmp(mFile->d_name,".") || !strcmp(mFile->d_name,"..")) continue;
        entries.push_back(to_string(incr++) + ". " +((string)mFile->d_name) + '\n');
    }

    //IF NO ENTRY WAS FOUND, INFORM CLIENT
    if (entries.empty()){
    	sendLogic("QUIT");
    }
    
    //IF ENTRIES WERE FOUND, INFORM CLIENT THEN SEND ENTRIES
    else {
    	sendLogic("NOQUIT");
    	sendVector(entries);
    }

    //CHANGE WORKING DIR BACK TO PREVIOUS DIR, TRY AGAIN IF NECESSARY
    _currentDIR = changeDir(toList);
    while (_currentDIR == NULL) {
    	cout << "Error happened while changing DIR" << endl;
    	_currentDIR = changeDir(NULL);
    }
}

void ServerUser::readFile(string fileName, int option) {
	struct dirent *entry = NULL;
	DIR *target = NULL;
	int dirIndex = 0, dirPos = 1;

	//CHECK IF FILENAME PROVIDED IS ACTUALLY INDEX
	bool isIndex = (fileName.length() < 4 && isdigit(fileName[0]));
	if (isIndex) dirIndex = stoi(fileName, nullptr);

	//IF INPUT IS VALID, PRECEED, ELSE CHANGE BACK TO PREVIOUS DIR, IF AT ALL IT WAS CHANGED
	if (fileName.length() > 4 || isIndex){

		//CHECK IF FILETYPE WAS PROVIDED IF NOT ADD IT
		if(!isIndex && !(fileName.substr( fileName.length() - 4 ) == ".txt")) fileName += ".txt";

		//CHOOSE TARGET PATH AND CHANGE WORKING DIR ACCORDINGLY
		string targetPath = (option == INBOX) ? _user.inbox : _user.outbox;
		target = changeDir(_currentDIR, targetPath);

		//ITERATE THROUGH DIR UNTIL CORRECT FILE WAS FOUND
		while ((entry=readdir(target)) != NULL){
	        if(!strncasecmp(entry->d_name,".",1) || !strncasecmp(entry->d_name,"..",2)) continue;

	        //IF FILE WAS FOUND SAVE ITS FILES TO STRING VECTOR
	        if((string)entry->d_name == fileName || dirIndex == dirPos++){
	        	vector<string> message;
	        	string filePath = targetPath + (isIndex ? (string)entry->d_name : fileName);
	        	message.push_back("Filename: \"" + string(entry->d_name) + "\"\n" + EDGE + '\n');
	        	ifstream file;
	        	string doneRead;

				file.open(filePath);
				while(getline(file,doneRead)) {
					doneRead += '\n';
					cout << doneRead;
					message.push_back(doneRead);
				}

				//ALL ACTIONS DONE, FILE CLOSE
				file.close();

				//IF FILE WAS FOUND EMPTY, ADD AT LEAST THAT TO THE VECTOR, THEN SEND IT AND CHANGE DIR BACK
				if(message.empty()) message.push_back("File was empty!");
				sendVector(message);
	    		_currentDIR = changeDir(target);

				return;
	        }
	    }
	}
    customMessage("That file was NOT found!\n");
    _currentDIR = changeDir(target);
}

string ServerUser::rcvMessage(int option){

	//RECEIVE FIRST MESSAGE, ACCORDING TO OPTION, MORE MIGHT FOLLOW
	string str = rcvLogic();
	
	switch(option){

		//NO MESSAGE WILL FOLLOW, MEANING EXACTLY ONE LINE, IF CASE INSENSITIVE QUIT IS PROVIDED, RETURN CAPS QUIT
		case NOMESSAGE: {
			if((strncasecmp(str.c_str(), "QUIT", 4) == 0)) {
				return "QUIT";	
			}
			//ELSE SIMPLY RETURN MESSAGE AS IS
			return str;
		}

		//MESSAGE FOLLOWS WITH NO LIMIT OF LINES, FIRST LINE IS ADDED IMMEDIATELY DUE TO INITIAL RECEIVE
		case ISMESSAGE: {
			vector<string> newMessage;
	        newMessage.push_back(str);

	        //NOW WAIT FOR REST UNTIL DELIMITER IS RECEIVED: ".\n"
	    	do{
	            str = rcvLogic();
	            newMessage.push_back(str);
	        }while(str != ".");	
	        newMessage.erase(newMessage.begin());
	        saveMessage(newMessage);
	        break;
		}

		//ONLY 1 DIGIT WILL BE PROVIDED, 1, 2 OR 9 FOR QUIT, IN WHICH CASE "4" WILL BE RETURNED SINCE ENUM QUIT IS 4
		case ONEORTWO: {
			if(str == "9") return "4";
			return str;
		}

		default: {
			return "QUIT";
		}
	}
}

void ServerUser::sendVector(vector<string> entries){

	//SEND EVERY ENTRY OF VECTOR WITH A TRAILING DELIMITER
	for (string i : entries) sendLogic(i);
	sendLogic(DELIMITER);
}

string ServerUser::rcvLogic(){

	//FIRST RECEIVE THE MESSAGE, CAST TO STRING, ANSWER WITH DELIMITER
	int size = recv(_socket,_buffer,BUFFER-1,0);
	_buffer[size-1] = '\0';	
	string str(_buffer);
	stopSend();
	return str;
}

void ServerUser::sendLogic(string message){

	//FIRST SEND MESSAGE WHICH NEEDS TO BE PARSED FIRST, THROW AWAY ANSWER, WHICH WOULD BE DELIMITER
	char delim[5];
	memset(_buffer, 0, BUFFER);
	if (send(_socket,message.c_str(),strlen(message.c_str())+1,0) == -1) cout << strerror(errno);
	recv(_socket,delim,5,0);
}

int ServerUser::chooseMode(){

	//RECEIVE MESSAGE TO CHOOSE MENU ENTRY
	string str = rcvMessage(NOMESSAGE);

	//TRANSFORM TO CAPS FIRST FOR EASIER COMPARISON
	locale loc;
	for (string::size_type i=0; i<str.length(); ++i) 
		str[i] = toupper(str[i],loc);

	//RETURN ENUM ENTRIES AFTER COMPARISON
	if(str == "READ") return READ;
	if(str == "LIST") return LIST;
	if(str == "SEND") return SEND;
	if(str == "DEL") return DEL;
	if(str == "QUIT") return QUIT;
	return INVALID;
}

void ServerUser::saveMessage(vector<string> message){

	//FIRST REPLACE WHITESPACES WITH UNDERSCORES
	transform(_rec.subject.begin(), _rec.subject.end(), _rec.subject.begin(), [](char ch) {
    	return ch == ' ' ? '_' : ch;
	});

	//CREATE FILENAME TO SAVE TO EITHER RECEIVER AND SENDER PATH
	string senderUID = genFileName(_rec.name, _rec.subject);
	string receiverUID = genFileName(_userName, _rec.subject);

	//CONCATENATE USERPATH WITH FILENAME FOR FINAL FILEPATH
	string senderFilePath = _user.outbox + senderUID;
	string receiverFilePath = _rec.inbox + receiverUID;

	writeMutex.lock();

	//OPEN BOTH FILES, IF NOT EXISTS, CREATE FIRST
	FILE *senderFile = fopen(senderFilePath.c_str() ,"a");
	FILE *receiverFile = fopen(receiverFilePath.c_str() ,"a");

	//DELIMITER WAS RECEIVED, NEEDS TO BE ELIMINATED FIRST
	message.pop_back();

	//SAVE EVERY SINGLE LINE IN BOTH FILES
	for (string i : message){
		fprintf(senderFile,   "%s\n", i.c_str());
		fprintf(receiverFile, "%s\n", i.c_str());
	}

	//FINALLY CLOSE BOTH FILES
	fclose(senderFile);
	fclose(receiverFile);

    writeMutex.unlock();
}

void ServerUser::stopSend(){

	//SIMPLY SEND DELIMITER, NO BIGGIE
	send(_socket, DELIMITER, strlen(DELIMITER),0);
}

DIR *ServerUser::changeDir(DIR *oldDIR, string path){

	//IF NO PATH WAS PROVIDED, CHOOSE STANDARD USERPATH
	if (path == "IchKluk") path = _user.userPath;	
	
	//FIND OUT CURRENT WORKING DIR ACCORDING TO OPEN DIR
	char cwd[1024] = {};
	getcwd(cwd, sizeof(cwd));

	//IF CWD ALREADY IS TARGET DIR, NO CHANGE AND RETURN OLD DIR
	if (string(cwd) == path) return oldDIR;

	//IF OLDDIR IS NOT NULL, CLOSE IT FIRST
	if (oldDIR) closedir(oldDIR);

	//NOW OPEN TARGET DIR
	DIR *newDIR;

	//IF ERROR HAPPENED, RETURN NULL, ELSE OPENED TARGET DIR
	return ((newDIR = opendir(path.c_str())) == NULL) ? NULL : newDIR;
}

void ServerUser::initFolders(string userName){

	writeMutex.lock();
	//CHECK IF USERFOLDERS ALREADY EXIST, OTHERWISE CREATE THEM
	string target = _user.mStorage + userName + "/";
	if (mkdir(target.c_str(), 0777) == 0){
		mkdir((target + "/inbox").c_str(), 0777);
		mkdir((target + "/outbox").c_str(), 0777);
	}
    writeMutex.unlock();
}

void ServerUser::receiveFile(){

	int size, receivedSize;
	FILE *file;
	bool dataEnd = true;
	char flag[5];

	//FIRST RECEIVE FILENAME, AND SET FILEPATH ACCORDINGLY
	string fileName = rcvLogic();
	if (fileName == "QUIT") return;
	string newPath = _rec.inbox + fileName;

	//OPEN FILE
	file = fopen(newPath.c_str(), "wb");

	//AFTER INITIAL MEMSET SINCE DATA WILL BE BINARY, START RECEIVING DATA
	memset(_buffer, 0, BUFFER);
	do{
		
		//HOW MUCH DATA TO EXPECT
		while ((size = recv(_socket,_buffer,5,0)) == -1) cout << "Receive failed, trying again!" << endl;
		stopSend();
		receivedSize = atoi(_buffer);
		

		//MORE DATA FOLLOWING FLAG, IF NO MORE DATA FOLLOWS, BREAK
		while ((size = recv(_socket,flag, 5, 0)) == -1) cout << "Receive failed, trying again!" << endl;
		stopSend();
		dataEnd = (strncmp(flag, "0", 1) == 0);
		if(dataEnd) break;


		//RECEIVE AND WRITE TO FILE
		while ((size = recv(_socket,_buffer,receivedSize,0)) == -1) cout << "Receive failed, trying again!" << endl;
		stopSend();
		fwrite(_buffer, sizeof(char), receivedSize, file);

		//AGAIN MEMSET SINCE ITS BINARY DATA
		memset(_buffer, 0, BUFFER);
	}while (!dataEnd);

	//FINALLY CLOSE FILE
	fclose(file);
}

ServerUser::~ServerUser(){

	//CLOSE SOCKET AFTER USER CHOSE QUIT
	close(_socket);
	cout << "User " << _userName << " closed his/her connection!" << endl;
}
