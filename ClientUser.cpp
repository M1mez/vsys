#include "ClientUser.h"

using namespace std;



ClientUser::ClientUser(int socket) : _socket(socket){
	string inputName, response;
	char* PW;
	char del[5];

	//USER IS PROMPTED FOR USERNAME AND PASSWORD MULTIPLE TIMES UNTIL THE CREDENTIALS ARE VALID
	do{
		cout << "Username: ";
		cin >> inputName;
		PW = getpass("Password: ");

		//SEND USERNAME AND PW AND WAIT FOR RESPONSE STATING IF VALID
		sendLogic(inputName);
		send(_socket,PW,strlen(PW)+1,0);
		recv(_socket,del,BUFFER+1,0);

		response = rcvLogic();
		cout << response << endl;

		//IF TOO MANY TIMES WRONG CREDENTIALS WERE PROVIDED, SHUT DOWN CLIENT
		if (response == "BLOCK"){
			 _isValid = false;
			 cout << "NOW YOU ARE BLOCKED!" << endl;
			 free(PW);
			 return;
		} else if (response == "STILLBLOCKED") {
			_isValid = false;
			free(PW);
			cout << "USER STILL IS BLOCKED!" << endl;
			return;
		} else _isValid = (response == "VALID");
	}
	while(!_isValid);
	free(PW);
}

void ClientUser::switchREAD(){
	cout << "READ:" << endl << EDGE;

	//CHOOSE IF FILE TO READ IS FROM INBOX OR OUTBOX
	cout << "1: Read an inbox  file!"  << endl;
	cout << "2: Read an outbox file!" << endl;
	cout << "9: Quit to menu!" << endl;

	if(sendMessage(1, ONEORTWO)) return;
	if (rcvLogic() == "QUIT"){
		cout << "No Messages found!" << endl;
	}else{
		cout << "Listing Messages:" << endl << EDGE;
		rcvVector();
		cout << EDGE;

		//PROVIDE A FILENAME, MIGHT BE INDEX OF TARGET FILE
		cout << "Name a file:  ";
		if(sendMessage(80, NOMESSAGE)) return;

		//RECEIVE CONTENT
	    rcvVector();
	    cout << EDGE << "MESSAGE END" << endl;
	}
}

void ClientUser::switchLIST(){
	cout << "LIST:" << endl << EDGE;

	//CHOOSE WHETHER INBOX OR OUTBOX SHOULD BE LISTED
    cout << "1: List my inbox-entries!"  << endl;
    cout << "2: List my outbox-entries!" << endl;
    cout << "9: Quit to menu!" << endl;

    if(sendMessage(1, ONEORTWO)) return;
    if (rcvLogic() == "QUIT"){
		cout << "No Messages found!" << endl;
	}else{
	    cout << EDGE << "LISTING MESSAGES:" << endl << endl;

	    //RECEIVE BOX' FILES' NAMES
	    rcvVector();
	    cout << EDGE;
	}
}

void ClientUser::switchSEND(){
	cout << "SEND:" << endl << EDGE;

	string str;
    string sendInfo[] = {"Receiver: ", "Subject: ", "Message:\n"};
    int sendCount = 0;

	//PROVIDE SEND INFORMATION AND FINALLY MESSAGE
    cout << sendInfo[sendCount++] << endl << EDGE;
        if(sendMessage(8, NOMESSAGE)) return;
    cout << sendInfo[sendCount++] << endl << EDGE;
        if(sendMessage(80, NOMESSAGE)) return;
    cout << sendInfo[sendCount++] << endl << EDGE;
        sendMessage(BUFFER, ISMESSAGE);
        cout << EDGE << "Message sent!" << endl;

    //SEND FILE, IF NOT WANTED, TYPE "QUIT"
	sendFile();
}

void ClientUser::switchDEL(){
	cout << "DEL:" << endl << EDGE;

	string str;
  	
  	//CHOOSE IF FILE TO DELETE IS IN INBOX OR OUTBOX
    cout << "1: Delete an inbox  entry!"  << endl;
    cout << "2: Delete an outbox entry!" << endl;
    cout << "9: Quit to menu!" << endl;
    if(sendMessage(1, ONEORTWO)) return;

    //PROVIDE FILENAME AND RECEIVE ANSWER IF FILE WAS DELETED OR NOT FOUND
    cout << "Which Message do you want to delete?" << endl;
    if(sendMessage(80, NOMESSAGE)) return;
    rcvVector();
}

int ClientUser::chooseMode(){
	string str;
	cin >> str;

	//INPUT TO UPPER FOR EASIER COMPARISON
	locale loc;
	for (string::size_type i=0; i<str.length(); ++i)
    str[i] = toupper(str[i],loc);

	//SEND OPTION TO SERVER
	sendLogic(str);

	//USE SENT OPTION FOR CLIENT ITSELF
	if(str == "READ") return READ;
	if(str == "LIST") return LIST;
	if(str == "SEND") return SEND;
	if(str == "DEL") return DEL;
	if(str == "QUIT") return QUIT;
	return INVALID;
}

bool ClientUser::sendMessage(int maxInput, int messageType){
    bool isQuit = false;
    string str;

    switch(messageType){

    	//IN CASE OF MESSAGE, SEND AS LONG AS DELIMITER ".\n" IS NOT PROVIDED 
        case ISMESSAGE: {
            do{
            	getline(cin, str);
                if(str.length() > maxInput+1){
                    cout << "Line too long! Please stay under" << maxInput << "signs!" << endl;
                    continue;
                }

                //SEND EACH LINE
                sendLogic(str);
            }while(str != ".");  
            break;
        }

        //A MAXIMUM ON INPUT CHARACTERS COULD BE DEFINED THIS WAS FOR LIMITING FOLLOWING INPUT
        case NOMESSAGE: {
            do{
                cin >> str;
                isQuit = stringCompare(str, "QUIT");
            
                if(str.length() > maxInput+1) cout << "Please enter maximum " << maxInput << " letters!" << endl;
            }while(str.length() > maxInput+1);

            //FINALLY SEND INPUT
            sendLogic(str);
            break;
        }

        //SEND ONLY DIGIT 1, 2 OR 9, 9 MEANS QUIT
        case ONEORTWO: {
            int tempNum;
            do{
            	cin >> tempNum;
                if(tempNum == 9) {
                    cout << "Quit to menu..." << endl;
                    isQuit = true;
                }

                //IF WRONG INPUT, REPEAT AND FLUSH CIN BUFFER IN CASE MANY CHARS WERE ENTERED
                cin.clear();
                cin.ignore(INT_MAX, '\n');
                if ((tempNum != 1 && tempNum != 2 && tempNum != 9) || cin.fail()){
                    printf("Please enter only 1, 2 or 9\n");
                }
            }while((tempNum != 1 && tempNum != 2 && tempNum != 9) || cin.fail());
            
            //FINALLY SEND INPUT
            sendLogic(to_string(tempNum));
            break;
        }
    }

    //IF QUIT OR AN EQUIVALENT WERE ENTERED, RETURN TRUE, IF CORRECT INPUT WAS ENTERED RETURN FALSE
    return isQuit;
}

void ClientUser::sendLogic(string message){

	//SEND A TO CHAR* PARSED STRING AND WAIT FOR AN ANSWER, WHICH IS THROWN AWAY
	char del[5] = {};
	send(_socket,message.c_str(),strlen(message.c_str())+1,0);
	recv(_socket,del,BUFFER+1,0);
}


bool ClientUser::stringCompare(string src, string tar){
	
	//FIRST PARAMETER - PROVIDED BY USER - TOUPPER AND COMPARED TO THE SECOND - PROVIDED BY PROGRAM - ALWAYS UPPERCASE 
	locale loc;
	for (string::size_type i=0; i<src.length(); ++i)
    src[i] = toupper(src[i],loc);

	//RETURNS IF THEY ARE THE SAME
	return src == tar;
}

void ClientUser::rcvVector(){

	//VECTOR IS RECEIVED AS STRINGS AND IMMEDIATELY PRINTED
	string str = {};
	do{
        str = rcvLogic();
        if(str == DELIMITER) break;
        cout << str;
    }while(str != DELIMITER);
}

string ClientUser::rcvLogic(){

	//BUFFER RECEIVED IS RESPONDED TO BY DELIMITER AND CONVERTED TO STRING, THEN RETURNED
	int size = recv(_socket,_buffer,BUFFER-1,0);
    send(_socket, DELIMITER, strlen(DELIMITER),0);

    string str(_buffer);
    memset(_buffer, 0, size);
    return str;
}

bool ClientUser::sendFile(){
	memset(_buffer, 0, BUFFER);

	int readSize;
	char flag[] = "1";
	char del[5];
	FILE *file;
	string filePath, fileName;  
	int inputCount = 0;

	cout << "DRAG'N'DROP your file now or type \"QUIT\": "; 
	
	do {
		if (inputCount > 0){
			inputCount++;
			cout << "Enter correct Path or \"QUIT\": ";
		}
		cin >> filePath;

		//IF "QUIT" WAS PROVIDED RETURN FALSE
		if (filePath == "QUIT") {
			sendLogic("QUIT");
			return false;
		}

		//ERASE PRECEDING AND TRAILING QUOTES TO ENABLE DRAG AND DROPPING
		filePath.erase( remove( filePath.begin(), filePath.end(), '\'' ), filePath.end());

		//CHECK IF FILE EXISTS
		ifstream f(filePath.c_str());
		if (f.good()) break;
	} while (1);

	//CUT OUT FILENAME
	size_t i = filePath.rfind("/", filePath.length());
	fileName = filePath.substr(i+1, filePath.length() - i - 1);
	cout << fileName << " -> FileName" << endl;


	file = fopen(filePath.c_str(), "rb");
	sendLogic(fileName);

	do{

		//HOW MUCH DATA WILL BE SENT?
		readSize = fread(_buffer, sizeof(char), BUFFER, file);
		sendLogic(to_string(readSize));

		// WILL MORE DATA FOLLOW? IF YES, SEND ALL
		if(readSize > 0){
			sendLogic(flag);
			
			//string is delimited, so couldn't use sendLogic
			send(_socket,_buffer,readSize,0);
			recv(_socket,del,BUFFER+1,0);
		}

	}while(readSize > 0);

	// NO MORE DATA FOLLOWS
	strcpy(flag, "0");
	sendLogic(flag);

	fclose(file);
	cout << "File was successfully sent!" << endl;
	
	memset(_buffer, 0, BUFFER);
	return true;

}

ClientUser::~ClientUser(){
	close(_socket);
}