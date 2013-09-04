#!/bin/sh

qmake -o Makefile build/DXBall.pro
if [ make ]; then
 	./DXBall -lPlayer Ole_Vegard -rPlayer Openent
	run
fi
