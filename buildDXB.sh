#!/bin/bash

# Let qmake create a makefile
qmake -o Makefile build/DXBall.pro

RunGame=false
RunGDB=false
RunValgrind=false

RunString="./DXBall -lPlayer Me -rPlayer You -fpsLimit 0 "
GDBString="gdb -ex run --args $RunString"
ValgindString="valgrind \
	--suppressions=valgrind/ignore \
	--tool=memcheck \
	--leak-check=full \
	--leak-resolution=high \
	--error-limit=no \
	--track-origins=yes \
	--undef-value-errors=yes \
	--read-var-info=yes \
	$RunString"

clear

# Try to build the project
if make; then

	# Build succesfull, no need to see make output
	clear
	echo "Compilation succesfull"

	# Check arguments
	while getopts ":rvg" opt; do
		case $opt in
			r)
				RunGame=true
				RunGDB=false
				RunValgrind=false
				;;
			v)
				RunGame=false
				RunValgrind=true
				RunGDB=false
				;;
			g)
				RunGame=false
				RunGDB=true
				RunValgrind=false
				;;
			\?)
				echo "Invalid option: -$OPTARG" >&2
				;;
		esac

	done

	if $RunGame ; then
		echo -e "\tNormal mode"
		echo -e "\tCommand : " $RunString
		echo "=============================== DX Balll ==============================="
		$RunString
	fi

	if $RunGDB ; then
		echo -e "\tDebug mode"
		echo -e "\tCommand : " $GDBString
		echo "=============================== DX Balll ==============================="
		$GDBString
	fi

	if $RunValgrind ; then
		echo -e "\tValgrind mode"
		echo -e "\tCommand : " $ValgindString
		echo "=============================== DX Balll ==============================="
		$ValgindString
	fi
fi # if make
exit 0
#gdb -ex run ./DXBall
#fi
#./DXBall -lPlayer Ole_Vegard -rPlayer Openent -fpslimit 0
