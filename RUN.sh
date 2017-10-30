#prepare parameters and commands
ip=127.0.0.1 #`ifconfig wlo1 | grep 'inet addr:' | cut -d: -f2 | awk '{ print $1}'`
path=`pwd`
port=$(shuf -i 5000-9999 -n 1)
serverCmd="bash -c \"$path/runServer $port $path\""
clientCmd="bash -c \"$path/runClient $port $ip\""

#multiscreen: if no arguments set, then single screen
term1=0
term2=650
if [ $# -eq 1 ]
 then
  term1=1800
  term2=2570
fi

#compile both files
make

#print information
echo "ip = $ip"
echo "path = $path"
echo "Port = $port"
echo "start server: \"$serverCmd\""
echo "start client: \"$clientCmd\""

#run commands
gnome-terminal --geometry 70x30+"$term1"+19 -e "$serverCmd"
gnome-terminal --geometry 70x30+"$term2"+19 -e "$clientCmd"	
