#!/bin/bash

# Let qmake create a makefile
qmake -o Makefile build/DXBall.pro

# Clear console window so it's easier to see make output
clear

# Try to build the project
if make; then

	# Build succesfull, no need to see make output
	clear
	echo 'Compilation succesfull'

	# Check arguments
	while getopts ":r" opt; do
		case $opt in
			r)
				echo 'Running program`'
				./DXBall -lPlayer Ole_Vegard -rPlayer Openent
				;;
			\?)
				echo "Invalid option: -$OPTARG" >&2
				;;
		esac
	done
	exit 0
fi
