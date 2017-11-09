#prepare parameters and commands
ip=`ifconfig wlo1 | grep 'inet addr:' | cut -d: -f2 | awk '{ print $1}'`
path=`pwd`
port=$(shuf -i 5000-9999 -n 1)
valgrind="valgrind --leak-check=yes --track-origins=yes"

if [ $1 -eq 0 ]
 then
  echo "no valgrind"
  valgrind=""
elif [ $1 -eq 1 ]
 then
  echo "normal check"
else
  echo "full check"
  valgrind="valgrind --leak-check=full --show-leak-kinds=all"
fi

serverCmd="bash -c \"$valgrind $path/runServer $port $path\""
clientCmd="bash -c \"$valgrind $path/runClient $port $ip\""


#multiscreen: if no arguments set, then single screen
term1=0
term2=650

if [ $# -lt 2 ]
 then
  echo "No Arguments given instead of valgrind switch"
elif [ $# -lt 3 ]
 then
  echo "Multiscreen mode"
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
sleep 1
gnome-terminal --geometry 70x30+"$term2"+19 -e "$clientCmd"	
