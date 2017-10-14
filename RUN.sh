#prepare parameters and commands
ip=`ifconfig ens33 | grep 'inet Adresse:' | cut -d: -f2 | awk '{ print $1}'`
path=`pwd`
serverCmd="bash -c \"Server/bin/Debug/Server 5001 ./mailStorage\""
clientCmd="bash -c \"Client/bin/Debug/Client 5001 $ip\""

#compile both files
gcc -Wall main.c -o runServer
gcc -Wall client.c -o runClient

#print information
echo "ip = $ip"
echo "path = $path"
echo "start server: \"$serverCmd\""
echo "start client: \"$clientCmd\""

#run commands
gnome-terminal -e "$serverCmd"
gnome-terminal -e "$clientCmd"
