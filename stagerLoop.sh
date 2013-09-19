#!/bin/bash
read filename

for item in $(cat $filename)
do
	if [[ "$1" == "-q" ]] #find out whats up with the item
	then
		stager_qry -M $item
	fi

	if [[ "$1" == "-r" ]] #request the item be moved to hard drive from the (slooooooooooow) magnetic tape
	then
		stager_get -M $item
	fi

	if [[ "$1" == "-g" ]] #pull the item from castor storage to local. This is almost never a good idea!
	then
		mkdir $(basename "$item" ".scmp" )
		xrdcp xroot://castorpublic.cern.ch/$item $(basename "$item" ".scmp")/file.scmp 
	fi
done

