#!/bin/bash

if [ $# -eq 1 ];
then
    grep -E "ERROR|WARN" $1 >bug.txt

else
    case $2 in
    "--latest")
        tail -n 5 $1        

    ;;
    "--find")
        grep $3 $1 >${3}.txt

    ;;
    "--diff")
        diff $1 $3 >nowhere
	if [ $? -eq 0 ]
	then
		echo "same"
	else
		echo "different"
	fi

    ;;
    esac
fi
