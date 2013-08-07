#include <iostream>
#include <string>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "Renderer.h"
#include "GamePiece.h"
#include "GameManager.h"
#include "Timer.h"
#include "Ball.h"
#include "Paddle.h"

	int main( int argc, char* args[] )
{
	GameManager gameMan;

	std::cout << "Args : " << argc << " \nArg[0] : " << args[0] << std::endl;
	std::cout << "C++ version : " << __cplusplus << std::endl;
	gameMan.Init();

	gameMan.Run();
}
