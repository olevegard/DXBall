#!/bin/bash



RunGame=false
RunGDB=false
RunValgrind=false
RunProfiler=false
RunTwoInstances=false

ClearCompileOutput=false
ForceFullCompile=false

CompileString="clang++ \
	main.cpp \
	Timer.cpp \
	GamePiece.cpp \
	Tile.cpp \
	Ball.cpp \
	Paddle.cpp \
	TilePosition.cpp \
	BoardLoader.cpp \
	Renderer.cpp \
	BonusBox.cpp \
	GameManager.cpp \
	MenuManager.cpp \
	math/Vector2f.cpp \
	math/Rect.cpp  \

	-std=c++11 \

	-Weverything  -Wall \
	-Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded \
	-Wno-switch-enum -Wno-float-equal -Werror \

	-fsanitize=address-full
	-fsanitize=undefined
	-fsanitize=null

	-c"

	#-fsanitize=memory
	#-fsanitize=behavior
	#-o DXBall
	#-O3 \
	#-lSDL2 \
	#-lSDL2_ttf \
	#-lSDL2_image \

CompileServer="clang++
	Server/main.cpp
	Server/TCPConnectionServer.cpp
	TCPMessage.cpp
	-o Server/Server
	-std=c++11
	-lSDL2
	-lSDL2_ttf
	-lSDL2_net
	-g"



#RunString="./DXBall -lPlayer client -rPlayer server -fpsLimit 0 -resolution 1280x720 -twoPlayer false"
RunString="./DXBall -lPlayer client -rPlayer server -fpsLimit 0 -resolution 1920x1080 -twoPlayer false -startfs true"
RunStringClient="./DXBall -lPlayer client -rPlayer server -fpsLimit 0 -resolution 960x540 "
RunStringServer="./DXBall -lPlayer server -rPlayer client -fpsLimit 0 -resolution 960x540 -server true -AIControlled true "
#RunStringServer="./DXBall -lPlayer server -rPlayer client -fpsLimit 0 -resolution 1920x1080 -twoPlayer true -server true"
RunStringGameServer="cd Server && ./Server"

GDBString="gdb -ex run --args $RunString"
GDBStringServer="gdb -ex run --args $RunStringServer"
GDBStringClient="gdb -ex run --args $RunStringClient"
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

ProfilerString="valgrind --tool=callgrind --dump-instr=yes --simulate-cache=yes --collect-jumps=yes $RunString"

# Completely clean console window
reset

# Let qmake create a makefile
qmake -o Makefile build/DXBall.pro

clear

echo "Building..."

# Try to build the project
if make; then

	# Check arguments
	while getopts ":rvgcfpdD" opt; do
		case $opt in
			r)
				RunGame=true
				RunGDB=false
				RunValgrind=false
				RunProfiler=false
				RunTwoInstances=false
				;;
			d)
				RunGame=false
				RunGDB=false
				RunValgrind=false
				RunProfiler=false
				RunTwoInstances=true
				;;
			D)#Rum two instances in debug
				RunGame=false
				RunGDB=true
				RunValgrind=false
				RunProfiler=false
				RunTwoInstances=true
				;;
			v)
				RunGame=false
				RunValgrind=true
				RunGDB=false
				RunProfiler=false
				RunTwoInstances=false
				;;
			g)
				RunGame=false
				RunGDB=true
				RunValgrind=false
				RunProfiler=false
				RunTwoInstances=false
				;;
			p)
				RunGame=false
				RunGDB=false
				RunValgrind=false
				RunProfiler=true
				RunTwoInstances=false
				;;
			c)
				ClearCompileOutput=true
				;;
			f)
				ForceFullCompile=true
				;;
			\?)
				echo "Invalid option: -$OPTARG" >&2
				;;
		esac

	done

	$CompileServer

	echo "Build succesfull"

	if $ForceFullCompile ; then
		echo "========================================================================"
		echo $CompileString
		$CompileString && echo 'Compile success!!!!'
	fi

	if $ClearCompileOutput ; then
		clear
	fi

	if $RunGame ; then
		echo -e "\tNormal mode"
		echo -e "\tCommand : " $RunString
		echo "=============================== DX Balll ==============================="
		$RunString
	fi

	if $RunTwoInstances ; then
		if $RunGDB ; then
			echo -e "\tDebug mode"
			echo -e "\tCommand : " $GDBString
			echo "=============================== DX Balll ==============================="
			#gnome-terminal -e "$GDBString"&
			#gnome-terminal -e "$GDBStringServer"&

			#Run in terminal ( explanation below )

			xterm -fa default -fs 12 -geometry 95x15+1000+600 -e "$RunStringGameServer" &
			sleep 1
			xterm -fa default -fs 12 -geometry 95x15+0+600 -e "$GDBStringServer"&
			sleep 1
			xterm -fa default -fs 12 -geometry 95x15+1000+600   -e "$GDBStringClient"&
		else
			echo -e "\tNormal mode"
			echo -e "\tCommand : " $RunString
			echo "=============================== DX Balll ==============================="

			gnome-terminal -e "$RunStringClient"&   # Run without blocking
			gnome-terminal -e "$RunStringServer"&   # Run without blocking

			# Run in a new terminal window with :
				# font      = default
				# font size = 13
				# size      = 95x15 ( font size )
				# position  = 95x15 ( pixels? )
				# program args
				# non-blocking
			xterm -fa default -fs 12 -geometry 95x15+1000+600 -e "$RunStringGameServer" &
			xterm -fa default -fs 12 -geometry 95x15+0000+600 -e "$RunStringServer" &
			xterm -fa default -fs 12 -geometry 95x15+1000+600 -e "$RunStringClient" &

			#xterm -fa default -fs 12 -geometry 95x15+0+600    -e "$RunStringServer" &
		fi
	else
		if $RunGDB ; then
			echo -e "\tDebug mode"
			echo -e "\tCommand : " $GDBString
			echo "=============================== DX Balll ==============================="
			$GDBString
		fi
	fi

	if $RunValgrind ; then
		echo -e "\tValgrind mode"
		echo -e "\tCommand : " $ValgindString
		echo "=============================== DX Balll ==============================="
		$ValgindString
	fi

	if $RunProfiler ; then
		echo -e "\tProilfer mode"
		echo -e "\tCommand : " $ProfilerString
		echo "=============================== DX Balll ==============================="
		$ProfilerString
	fi

fi # if make
exit 0
#gdb -ex run ./DXBall
#fi
#./DXBall -lPlayer Ole_Vegard -rPlayer Openent -fpslimit 0
