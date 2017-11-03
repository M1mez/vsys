#include "ServerUser.h"

using namespace std;

ServerUser::ServerUser(string userName, string path, int socket) :_userName(userName), _socket(socket){
	
	_user.mStorage = path;
	_user.userPath = path + userName + "/"; 
	_user.outbox = path + userName + "/outbox/";
	_user.inbox = path + userName + "/inbox/";
	initFolders(userName);

	cout << EDGE;
	cout << _user.inbox << endl << _user.outbox << endl << _user.mStorage << endl << _user.userPath << endl;
	cout << EDGE;

	_userDIR = changeDir(NULL);
	cout << "User " << _userName << " built up a connection!" << endl;

	string i = "Welcome. Please enter your command:\n";
	send(_socket, i.c_str(), strlen(i.c_str()),0);
}

void ServerUser::switchREAD(){
	cout << "User: " << _userName << " chose READ" << endl;

	string fileName;
	int option;
    bool isQuit = false;

    if ((option = stoi(rcvMessage(ONEORTWO))) == QUIT) isQuit = true;
    else if ((fileName = rcvMessage(NOMESSAGE)) == "QUIT") isQuit = true;
	else {
		readFile(fileName, option);
		return;
	}

    switch(option){
    	case QUIT: {
    		cout << "User chose to quit to menu!" << endl;
    		break;
    	}
    	default: {
    		cout << "ERROR IN SERVER LIST" << endl;
    		break;
    	}
    }
}

void ServerUser::switchLIST(){
	cout << "User: " << _userName << " chose LIST" << endl;
	
    int option;
    if ((option = stoi(rcvMessage(ONEORTWO))) != QUIT){
    	listDir(option);
		return;
	}

    switch(option){
    	case QUIT: {
    		cout << "User chose to quit to menu!" << endl;
    		break;
    	}
    	default: {
    		cout << "ERROR IN SERVER LIST" << endl;
    		break;
    	}
    }
}

void ServerUser::switchSEND(){
	cout << "User: " << _userName << " chose SEND" << endl;

	string sendInfo[] = {"Empfänger: ", "Betreff: ", "Nachricht:\n"};
	string sendStep[2];
	int rcvCount = 0;

	do{
		if ((sendStep[rcvCount] = rcvMessage(NOMESSAGE)) == "QUIT"){
			return;
		}
		cout << sendInfo[rcvCount] << sendStep[rcvCount] << endl;
		rcvCount++;
	}while(rcvCount < 2);

	cout << sendInfo[0] << sendStep[0] + " " << sendInfo[1] << sendStep[1];

	setReceiver(sendStep[0], sendStep[1]);

	rcvMessage(ISMESSAGE);

	cout << "IN Message saved in: " << _rec.inbox << endl;
	cout << "OUT  Message saved in: " << _user.outbox << endl;
}

void ServerUser::switchDEL(){
	cout << "User: " << _userName << " chose DEL" << endl;

	int option;
	string fileName;

    if(option = stoi(rcvMessage(ONEORTWO)) == QUIT) return;
    if((fileName = rcvMessage(NOMESSAGE)) == "QUIT") return;

    deleteMessage(fileName, option);
}

void ServerUser::setReceiver(string name, string subject){
	_rec.name = name;
	_rec.inbox = (_user.mStorage + name + "/inbox/");
	_rec.subject = subject;
	initFolders(name);
}

string ServerUser::genFileName(string counterPart, string subject){
	uuid_t uuid;

    uuid_generate_random(uuid);
    char uuid_str[9];
    uuid_unparse_lower(uuid, uuid_str);
    uuid_str[8] = '\0';

    string nowTime = to_string(((long int)time(NULL))%1000);
    string uuidStr(uuid_str);

    return (counterPart + "-" + subject + "-" + uuidStr + "-" + nowTime + ".txt");
}

void ServerUser::deleteMessage(string fileName, int option){
	if(!(fileName.substr( fileName.length() - 4 ) == ".txt")) fileName += ".txt";
	string filePath = ((option == INBOX) ? _user.inbox : _user.outbox) + fileName;

	if(remove(filePath.c_str()) != 0){
		string noFileFound = "No File found with name: \"" + fileName + "\"";
		customMessage(noFileFound);
	}else {
		string fileDeleted = "File: \"" + fileName + "\" was successfully deleted!";
		customMessage(fileDeleted);
	}
}

void ServerUser::customMessage(string message){
	vector<string> v;
	v.push_back(message);
	sendVector(v);
}

void ServerUser::sendVector(vector<string> entries){
	string edges = "~~~~~~~~~~~~~~~\n";
	entries.insert(entries.begin(),edges);
	entries.push_back(edges);

	for (string i : entries){
		send(_socket, i.c_str(), strlen(i.c_str()),0);
		rcvMessage(NOMESSAGE);
	}
	stopSend();
}

void ServerUser::listDir(int option) {
    struct dirent *mFile;
    vector<string> entries;
    int incr = 0;
	
	DIR *toList = changeDir(_userDIR, ((option == INBOX) ? _user.inbox : _user.outbox));
	cout << ((option == INBOX) ? _user.inbox : _user.outbox) << endl;

    while ((mFile=readdir(toList))){
        if(!strncasecmp(mFile->d_name,".",1) || !strncasecmp(mFile->d_name,"..",2)) continue;
        entries.push_back(to_string(++incr) + ". " +((string)mFile->d_name));
    }
    if (entries.empty()){
    	customMessage("NO files where found!");
    }
    sendVector(entries);
    changeDir(toList);
}

void ServerUser::readFile(string fileName, int option) {
	struct dirent *box;
	if(!(fileName.substr( fileName.length() - 4 ) == ".txt")) fileName += ".txt";

	string targetPath = (option == INBOX) ? _user.inbox : _user.outbox;
	DIR *target = changeDir(_userDIR, targetPath);
	//DIR *target = opendir(targetPath.c_str());

	while ((box=readdir(target)) != NULL){
        if(!strncasecmp(box->d_name,".",1) || !strncasecmp(box->d_name,"..",2)) continue;
        
        if((string)box->d_name == fileName){
        	string filePath = targetPath + fileName;
        	vector<string> message;
        	ifstream file;
        	string doneRead;

			file.open(filePath);
			while(getline(file,doneRead)) {
				doneRead += '\n';
				cout << doneRead;
				message.push_back(doneRead);
			}
			//getline(file,doneRead);
			//message.push_back(doneRead);

			file.close();

			if(message.empty()) message.push_back("File was empty!");

			sendVector(message);
    		_userDIR = changeDir(target);
			return;
        }
    }
    customMessage("That file was NOT found!");
    _userDIR = changeDir(target);
}

string ServerUser::rcvMessage(int option){
	string str = rcvLogic();
	
	switch(option){
		case NOMESSAGE: {
			if((strncasecmp(str.c_str(), "QUIT", 4) == 0)) {
				return "QUIT";	
			}
			return str;
		}
		case ISMESSAGE: {
			vector<string> newMessage;
	        newMessage.push_back(str);

	    	do{
	            str = rcvLogic();
	            newMessage.push_back(str);
	        }while(str != ".");	
	        newMessage.erase(newMessage.begin());
	        saveMessage(newMessage);
	        break;
		}
		case ONEORTWO: {
			if(str == "9") return "4";
			return str;
		}
		case NUMERIC: {
			if (strncasecmp(str.c_str(), "QUIT", 4) == 0) return "QUIT";
			return str;
		}
		default: {
			return "";
		}
	}
	return "";
}

string ServerUser::rcvLogic(){
	int size = recv(_socket,_buffer,BUFFER-1,0);
	_buffer[size] = '\0';	
	string str(_buffer);
	//str += '\n';
	//cout << str;
	stopSend();

	return str;
}

void ServerUser::sendLogic(string message){
	send(_socket,message.c_str(),message.length()+1,0);
	recv(_socket,_buffer,BUFFER-1,0);
}

int ServerUser::chooseMode(){
	string str = rcvMessage(NOMESSAGE);

	locale loc;
	for (string::size_type i=0; i<str.length(); ++i)
    str[i] = toupper(str[i],loc);

	if(str == "READ") return READ;
	if(str == "LIST") return LIST;
	if(str == "SEND") return SEND;
	if(str == "DEL") return DEL;
	if(str == "QUIT") return QUIT;
	return INVALID;
}

void ServerUser::saveMessage(vector<string> message){

	transform(_rec.subject.begin(), _rec.subject.end(), _rec.subject.begin(), [](char ch) {
    	return ch == ' ' ? '_' : ch;
	});

	string senderUID = genFileName(_rec.name, _rec.subject);
	string receiverUID = genFileName(_userName, _rec.subject);

	string senderFilePath = _user.outbox + senderUID;
	string receiverFilePath = _rec.inbox + receiverUID;

	FILE *senderFile = fopen(senderFilePath.c_str() ,"a");
	FILE *receiverFile = fopen(receiverFilePath.c_str() ,"a");

	message.pop_back();

	for (string i : message){
		fprintf(senderFile,   "%s\n", i.c_str());
		fprintf(receiverFile, "%s\n", i.c_str());
	}

	fclose(senderFile);
	fclose(receiverFile);
}

void ServerUser::stopSend(){

	send(_socket, DELIMITER, strlen(DELIMITER),0);
}

DIR *ServerUser::changeDir(DIR *oldDIR, string path){
	if (oldDIR) closedir(oldDIR);

	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	cout << "Old working dir: " << string(cwd) << endl;

	if (path == "IchKluk"){
		path = _user.userPath;	
	} 

	DIR *newDIR;
	if((newDIR = opendir(path.c_str())) == NULL) {
		printf("Error happened in changeDir: %d\n", errno);
		return NULL;
	}else {
		getcwd(cwd, sizeof(cwd));
		cout << "Current working dir: " << string(cwd) << endl;
		return newDIR;
	}
}

void ServerUser::initFolders(string userName){
	string target = _user.mStorage + userName + "/";
	if (mkdir(target.c_str(), 777) == 0){
		mkdir((target + "/inbox").c_str(), 777);
		mkdir((target + "/outbox").c_str(), 777);
	}
}

ServerUser::~ServerUser(){
	close(_socket);
	cout << "User " << _userName << " closed his/her connection!" << endl;
}
