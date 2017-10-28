#include "ServerUser.h"

using namespace std;

ServerUser::ServerUser(string userName, string path, int socket) :_userName(userName), _path(path), _socket(socket){
	_outbox = path + userName + "/outbox/";
	_inbox = path + userName + "/inbox/";
	cout << "User " << userName << " built up a connection!";
}

void ServerUser::setReceiver(string name){
	_rec.name = name;
	_rec.inbox = (_path + name + "/inbox/");
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
	string filePath = ((option == INBOX) ? _inbox : _outbox) + fileName;

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

string ServerUser::rcvMessage(int option){
	char buffer[BUFFER-1] = {};
	int size = recv(_socket,buffer,BUFFER-1,0);
	buffer[size-1] = '\0';
	string tmp(buffer);
	memset(buffer, '\0', size);

	switch(option){
		case NOMESSAGE: {
			if((strncasecmp(tmp.c_str(), "QUIT", 4) == 0) || tmp == ".\n") {
				return "QUIT";	
			}
			return tmp;
		}
		case ISMESSAGE: {
			vector<string> newMessage;
	    	do{
	            size = recv(_socket,buffer,BUFFER-1,0);
	            buffer[size] = '\0';
	            printf("%s", buffer);
	            newMessage.push_back((string)buffer);
	        }while(strcmp(buffer,DELIMITER) != 0);	
	        saveMessage(newMessage);
	        break;
		}
		case ONEORTWO: {
			if(tmp == "9") return "4";
			return tmp;
		}
		case NUMERIC: {
			if (strncasecmp(tmp.c_str(), "QUIT", 4) == 0) return "QUIT";
			return tmp;
		}
		default: {
			return "";
		}
	}
	return "";
}

int ServerUser::chooseMode(string str){
	std::transform(str.begin(), str.end(),str.begin(), ::toupper);
	if(str == "READ") return READ;
	if(str == "LIST") return LIST;
	if(str == "SEND") return SEND;
	if(str == "DEL") return DEL;
	if(str == "QUIT") return QUIT;
	return INVALID;
}

void ServerUser::saveMessage(vector<string> message){

	string senderUID = genFileName(_rec.name, _rec.subject);
	string receiverUID = genFileName(_userName, _rec.subject);

	string senderFilePath = _outbox + senderUID;
	string receiverFilePath = _rec.inbox + receiverUID;

	FILE *senderFile = fopen(senderFilePath.c_str() ,"a");
	FILE *receiverFile = fopen(receiverFilePath.c_str() ,"a");

	for (string i : message){
		fprintf(senderFile,   "%s", i.c_str());
		fprintf(receiverFile, "%s", i.c_str());
	}

	fclose(senderFile);
	fclose(receiverFile);
}

void ServerUser::stopSend(){
	send(_socket, DELIMITER, strlen(DELIMITER),0);
}

ServerUser::~ServerUser(){
	close(_socket);
	cout << "User " << _userName << " closed his/her connection!" << endl;
}

