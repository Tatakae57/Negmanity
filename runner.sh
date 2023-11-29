#!/bin/bash

#	Parámetros
path=$1
type=$2
extension="${path##*.}"
name="${path%.*}"
realpath=$(dirname "$path")
echo $realpath

#	Build
if [ "$type" = "build" ]; then
	if [ "$extension" = "c" ]; then
		gcc -o $name $path -lncurses -lncursesw
	elif [ "$extension" = "cpp" ]; then
		g++ -o $name $path -lncurses -lncursesw
	fi

#	Run
elif [ "$type" = "run" ]; then
	if [ "$extension" = "py" ]; then
		python3 $path
	elif [ "$extension" = "sh" ]; then
		sh $path
	else
		if [ "$realpath" = "." ]; then
			$name
		else 
			$realpath$name
		fi
	fi
	
else 
	echo "Unknown command $type"
fi
