#prepare parameters and commands
ip=`ifconfig wlo1 | grep 'inet addr:' | cut -d: -f2 | awk '{ print $1}'`
path=`pwd`
serverCmd="bash -c \"$path/runServer 5001 ./mailStorage\""
clientCmd="bash -c \"$path/runClient 5001 $ip\""

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
