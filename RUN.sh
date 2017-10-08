x=`pwd`
gcc -Wall main.c -o runServer
gcc -Wall client.c -o runClient
serverCmd='bash -c \"$x/runServer 5001 ./mailStorage\"'
echo $serverCmd
gnome-terminal -e "$serverCmd"
clientCmd='bash -c \"$x/runClient 5001 192.168.0.178\"'
echo $clientCmd
gnome-terminal -e "$clientCmd"
