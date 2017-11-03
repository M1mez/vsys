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
	g++ -std=c++11 -g -Wall Client.o ClientMan.o ClientUser.o -o runClient

Client.o: Client.cpp
	g++ -std=c++11 -c Client.cpp

ClientUser.o: ClientUser.cpp ClientUser.h
	g++ -std=c++11 -c ClientUser.cpp

ClientMan.o: ClientMan.cpp ClientMan.h
	g++ -std=c++11 -c ClientMan.cpp


#clean -----------------------------------------
clean:
	rm -f *.o runServer
	rm -f *.o runClient
