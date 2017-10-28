all: runServer runClient


#Server ----------------------------------------
runServer: Server.o ServerMan.o ServerUser.o
	g++ -std=c++11 -g -Wall Server.o ServerMan.o ServerUser.o -luuid -o runServer

Server.o: Server.cpp
	g++ -std=c++11 -c Server.cpp

ServerUser.o: ServerUser.cpp ServerUser.h
	g++ -std=c++11 -c ServerUser.cpp

ServerMan.o: ServerMan.cpp ServerMan.h
	g++ -std=c++11 -c ServerMan.cpp


#Client ----------------------------------------
runClient: Client.o
	g++ -std=c++11 -g -Wall Client.o -o runClient

Client.o: Client.cpp
	g++ -std=c++11 -c Client.cpp


#clean -----------------------------------------
clean:
	rm *.o runServer
	rm *.o runClient
