#include "Server.h"

#if defined(_WIN32)
int wmain( int argc, char* args[] )
#else
int main( int argc, char* args[] )
#endif
{
	Server server;

	if ( !server.Init() )
	{
		std::cin.ignore();
		return 1;
	}

	server.Run();
}

