#!/bin/bash
stty -F /dev/ttyACM0 115200 raw -echo -ignpar -cstopb eol 255 eof 255 

while true
do
	oldY=0
	oldX=0
	data="$(echo -e "\r\nHTTP/1.0 404 File not found\r\nServer: KaputtServer/0.0.1 Kaputt/1.0\r\nContent-Type:text/html\r\nConnection:close\r\n\r\n" | nc.traditional -l -p 8081 -s 127.0.0.1 | grep -v ':' | grep -v ^$)"
	echo $data
	stack=""
	for i in $data
	do
		if [ ! "$i" = "" ]
		then
			X="$(echo $i | awk -F ',' '{print $1}')"
			Y="$(echo $i | awk -F ',' '{print $2}')"
			PEN="$(echo $i | awk -F ',' '{print $3}')"
			if [ "$X" > "$oldX" ]
			then
				for i in $(seq $oldX $X)
				do
					stack=$stack"4"
				done
			fi
			if [ "$X" < "$oldX" ]
			then
				for i in $(seq $X $oldX)
				do
					stack=$stack"3"
				done
			fi
			if [ "$Y" > "$oldY" ]
			then
				for i in $(seq $oldY $Y)
				do
					stack=$stack"1";
				done
			fi
			if [ "$Y < $oldY" ]
			then
				for i in $(seq $Y $oldY)
				do
					stack=$stack"2"
				done
			fi
			if [ "$PEN" = "0" ]
			then
				stack=$stack"5"
			fi
			if [ "$PEN" = "1" ]
			then
				stack=$stack"6"
			fi
			oldX=$X
			oldY=$Y
		fi
	done
	php controlshit.php "$stack"
done
