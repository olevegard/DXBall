#include "Server.h"

int main()
{
	Server server;

	if ( !server.Init() )
	{
		std::cin.ignore();
		return 1;
	}

	server.Run();
	std::cin.ignore();
}

