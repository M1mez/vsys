#include "ClientUser.h"

using namespace std;



ClientUser::ClientUser(int socket) : _socket(socket){

}

void ClientUser::switchREAD(){
	cout << "READ:" << endl << EDGE;
	
	string str;

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

		cout << "Name a file:  ";
		if(sendMessage(80, NOMESSAGE)) return;

	    rcvVector();
	    cout << EDGE << "MESSAGE END" << endl;
	}
}

void ClientUser::switchLIST(){ //CHECK
	cout << "LIST:" << endl << EDGE;


    cout << "1: List my inbox-entries!"  << endl;
    cout << "2: List my outbox-entries!" << endl;
    cout << "9: Quit to menu!" << endl;

    if(sendMessage(1, ONEORTWO)) return;
    if (rcvLogic() == "QUIT"){
		cout << "No Messages found!" << endl;
	}else{
	    cout << EDGE << "LISTING MESSAGES:" << endl << endl;
	    rcvVector();
	    cout << EDGE;
	}
}

void ClientUser::switchSEND(){ //CHECK
	cout << "SEND:" << endl << EDGE;


	string str;
    string sendInfo[] = {"Empfänger: ", "Betreff: ", "Nachricht:\n"};
    int sendCount = 0;

    cout << sendInfo[sendCount++] << endl << EDGE;
        if(sendMessage(8, NOMESSAGE)) return;
    cout << sendInfo[sendCount++] << endl << EDGE;
        if(sendMessage(80, NOMESSAGE)) return;
    cout << sendInfo[sendCount++] << endl << EDGE;
        sendMessage(BUFFER, ISMESSAGE);
        cout << EDGE << "Message sent!" << endl;


	sendFile();
}

void ClientUser::switchDEL(){
	cout << "DEL:" << endl << EDGE;

	string str;
  
    cout << "1: Delete an inbox  entry!"  << endl;
    cout << "2: Delete an outbox entry!" << endl;
    cout << "9: Quit to menu!" << endl;
    if(sendMessage(1, ONEORTWO)) return;

    cout << "Which Message do you want to delete?" << endl;
    if(sendMessage(80, NOMESSAGE)) return;

    rcvVector();
    /*do{
        size = recv(createSocket,buffer,RCVBUFF,0);
        buffer[size] = '\0';
        if(strncmp(buffer,".\n",2) == 0) break;
        printf("%s\n", buffer);
        send(createSocket,".\n",strlen(".\n"),0);
    }while(strncmp(buffer,".\n",2) != 0);  
        send(createSocket,".\n",strlen(".\n"),0);
	*/
}

void ClientUser::switchQUIT(){
	//cout << "QUIT:" << endl << EDGE;


}


int ClientUser::chooseMode(){
	string str;
	cin >> str;

	locale loc;
	for (string::size_type i=0; i<str.length(); ++i)
    str[i] = toupper(str[i],loc);

	sendLogic(str);


	if(str == "READ") return READ;
	if(str == "LIST") return LIST;
	if(str == "SEND") return SEND;
	if(str == "DEL") return DEL;
	if(str == "QUIT") return QUIT;
	return INVALID;
}



bool ClientUser::sendMessage(int maxInput, int messageType){
    size_t nameSize;
    bool isQuit = false;
    string str;

    switch(messageType){
        case ISMESSAGE: {
            do{
            	getline(cin, str);
                if(str.length() > maxInput+1){
                    cout << "Line too long! Please stay under" << maxInput << "signs!" << endl;
                    continue;
                }
                sendLogic(str);
            }while(str != ".");  
            break;
        }
        case NOMESSAGE: {
            do{
                cin >> str;
                isQuit = stringCompare(str, "QUIT");
            
                if(str.length() > maxInput+1){
                	cout << "Please enter maximum " << maxInput << " letters!" << endl;
                }

            }while(str.length() > maxInput+1);
            
            sendLogic(str);
            break;
        }
        case ONEORTWO: {
            int tempNum;
            do{
            	cin >> tempNum;
            	//cout << "INPUT: " << tempNum << endl;

                if(tempNum == 9) {
                    cout << "Quit to menu..." << endl;
                    isQuit = true;
                }

                if ((tempNum != 1 && tempNum != 2 && tempNum != 9) || cin.fail()){
                    printf("Please enter only 1, 2 or 9\n");
                }
            }while((tempNum != 1 && tempNum != 2 && tempNum != 9) || cin.fail());
            
            sendLogic(to_string(tempNum));
            break;
        }
        /*case NUMERIC: {
            int tempNum;

            cin >> tempNum;

            if (cin.fail() || tempNum <= 0 || tempNum > maxInput){
                printf("Please enter a number greater than 0 or less than %d\n", maxInput+1);
                isQuit = true;
            }
            strcpy(buffer,(isQuit ? "QUIT" : to_string(tempNum).c_str()));
            //buffer = isQuit ? "QUIT" : (char*	)to_string(tempNum).c_str();

            send(_socket,buffer,strlen(buffer),0);
            break;
        }*/
    }
    return isQuit;
}

void ClientUser::sendLogic(string message){
	char del[5];
	send(_socket,message.c_str(),strlen(message.c_str())+1,0);
	recv(_socket,del,BUFFER+1,0);
}

/*void ClientUser::stopSend(){

	send(_socket, DELIMITER, strlen(DELIMITER),0);
}*/

bool ClientUser::stringCompare(string src, string tar){
	locale loc;
	for (string::size_type i=0; i<src.length(); ++i)
    src[i] = toupper(src[i],loc);

	return src == tar;
}

void ClientUser::rcvVector(){
	/*memset(_buffer, 0, BUFFER);
	int size = recv(_socket,_buffer,BUFFER-1,0);

	printf("HIER %s HIER\n",_buffer );*/

	string str = {};
	//cout << EDGE;
	do{
		//cout << "here" << endl;
        str = rcvLogic();
        	if(str == DELIMITER) {
        		//cout << "whybreak?" << endl;
        		break;
        	}
        cout << str;
        //stopSend();
    }while(str != DELIMITER);  
        //stopSend();
    //cout << EDGE;
}

string ClientUser::rcvLogic(){
	//cout << "waiting" << endl;
	memset(_buffer, 0, BUFFER);
	int size = recv(_socket,_buffer,BUFFER-1,0);
	//sleep(1);
	if (size == -1 ) {
		cout << "ERROR IN RECEIVE" << endl;
    	cout << strerror(errno) << endl;
    }
    //_buffer[size] = '\0';
    string str = string(_buffer);
    //cout << "_" << str << "_";
    //cout << "got: " << str << "_" << endl;
    send(_socket, DELIMITER, strlen(DELIMITER),0);

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

	cout << "DRAG'N'DROP your file NOW: "; 
	
	do {
		if (inputCount > 0){
			inputCount++;
			cout << "Enter correct Path or \"QUIT\": ";
		}
		cin >> filePath;
		if (filePath == "QUIT") {
			sendLogic("QUIT");
			return false;
		}

		filePath.erase( remove( filePath.begin(), filePath.end(), '\'' ), filePath.end());
		cout << filePath << "1" << endl;
		ifstream f(filePath.c_str());
		cout << filePath << "2" << endl;
		if (f.good()) break;
	} while (1);

	size_t i = filePath.rfind("/", filePath.length());
	fileName = filePath.substr(i+1, filePath.length() - i - 1);

	cout << fileName << " -> FileName" << endl;


	if (stringCompare(filePath, "QUIT")) return false;
	file = fopen(filePath.c_str(), "rb");

	sendLogic(fileName);

	strcpy(flag, "1");

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