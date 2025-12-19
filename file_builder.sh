#!/bin/bash

height=$1
width=$2
id=$3
path="files/file"

touch "files/file$3"
# Em Ob Fu
echo "$height.ox" > "$path$id"
#rand=$(shuf -i 0-9 -n 1)
j=0
while (( j < $height ))
do
	rand=$(( RANDOM % $width ))
	i=0
	while (( i < $width )) 
	do
		if (( i == rand ))
		then
			echo -n "o" >> "$path$id"
		else
			echo -n "." >> "$path$id"
		fi
		(( i++ ))
	done
	echo >> "$path$id"
	(( j++ ))
done

./bsq "$path$id" | cat -e
