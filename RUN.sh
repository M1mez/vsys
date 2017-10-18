#prepare parameters and commands
ip=127.0.0.1 #`ifconfig wlo1 | grep 'inet addr:' | cut -d: -f2 | awk '{ print $1}'`
path=`pwd`
port=$(shuf -i 5000-9999 -n 1)
serverCmd="bash -c \"$path/runServer $port $path/mailStorage\""
clientCmd="bash -c \"$path/runClient $port $ip\""

#compile both files
g++ -std=c++11 -g -Wall ./Server.cpp -o ./runServer
g++ -std=c++11 -g -Wall ./Client.cpp -o ./runClient

#print information
echo "ip = $ip"
echo "path = $path"
echo "Port = $port"
echo "start server: \"$serverCmd\""
echo "start client: \"$clientCmd\""

#run commands
gnome-terminal --geometry 70x30+-10+19 -e "$serverCmd"
gnome-terminal --geometry 70x30+645+19 -e "$clientCmd"
