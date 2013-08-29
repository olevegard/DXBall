#include <iostream>

#include "GameManager.h"

std::string ReplaceUnderscores( std::string str );

int main( int argc, char* args[] )
{
	GameManager gameMan;

	std::string localPlayerName  = "Player 1";
	std::string remotePlayerName = "Player 2";

	std::cout << "C++ version : " << __cplusplus << std::endl;
	std::cout << "Args : \n";

	for ( int i = 1; i < argc ; i+=2 )
	{
		if ( argc > ( i + 1 ) )
		{
			std::cout << "\t" << i << " : " <<  args[i] << " = " << args[ i + 1 ]  << std::endl;

			std::string str = args[i];
			if ( str == "-lplayer" && argc > ( i + 1 )  )
				localPlayerName = args[i + 1 ];
			else if ( str == "-rplayer" && argc > ( i + 1 )  )
				remotePlayerName = args[i + 1 ];
		}
	}

	localPlayerName  = ReplaceUnderscores( localPlayerName );
	remotePlayerName = ReplaceUnderscores( remotePlayerName );

	gameMan.Init( localPlayerName, remotePlayerName );

	gameMan.Run();
}

std::string ReplaceUnderscores( std::string str )
{
	for ( size_t i = 0 ; i < str.size() ; ++i )
		if ( str[i] == '_' )
			str[i] = ' ';

	return str;
}
