all: runServer runClient


#Server ----------------------------------------
runServer: Server.o ServerMan.o ServerUser.o
	g++ -std=c++11 -g -Wall Server.o ServerMan.o ServerUser.o -luuid -pthread -o runServer

Server.o: Server.cpp
	g++ -std=c++11 -c Server.cpp

ServerUser.o: ServerUser.cpp ServerUser.h
	g++ -std=c++11 -c ServerUser.cpp

ServerMan.o: ServerMan.cpp ServerMan.h
	g++ -std=c++11 -c ServerMan.cpp


#Client ----------------------------------------
runClient: Client.o ClientMan.o ClientUser.o
	g++ -std=c++11 -g -Wall Client.o ClientMan.o ClientUser.o -o runClient -lldap -DLDAP_DEPRECATED

Client.o: Client.cpp
	g++ -std=c++11 -c Client.cpp -lldap -DLDAP_DEPRECATED

ClientUser.o: ClientUser.cpp ClientUser.h 
	g++ -std=c++11 -c ClientUser.cpp -lldap -DLDAP_DEPRECATED

ClientMan.o: ClientMan.cpp ClientMan.h
	g++ -std=c++11 -c ClientMan.cpp -lldap -DLDAP_DEPRECATED


#clean -----------------------------------------
clean:
	rm -f runServer *.o 
	rm -f runClient *.o 
