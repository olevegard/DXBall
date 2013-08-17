#include <iostream>

#include "GameManager.h"

int main( int argc, char* args[] )
{
	GameManager gameMan;

	std::cout << "Args : " << argc << " \nArg[0] : " << args[0] << std::endl;
	std::cout << "C++ version : " << __cplusplus << std::endl;

	gameMan.Init();

	gameMan.Run();
}
