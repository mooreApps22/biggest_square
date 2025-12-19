#!/bin/bash

mkdir -p "files"

if (( $# != 1 ))
then
	echo "Usage: $0 <iteration count>"
	exit
fi

make

echo "====== Generating $# BSQ Maps  ========="
echo

i=0
while (( i < $1 ))
do
	echo
	echo  "----------------- Map $(( i + 1 )) -----------------"
	echo

	rand1=0
	rand2=0
	until (( rand1 != 0 && rand2 != 0 ))
	do
		rand1=$(( RANDOM % 20))
		rand2=$(( RANDOM % 20))
	done
	./file_builder.sh $rand1 $rand2 "$i"
	(( i++ ))
done
echo
