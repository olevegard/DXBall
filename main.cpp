#include <iostream>
#include <algorithm>
#include <string>

#include "GameManager.h"

std::string Replace( std::string str, char replace, char replaceWith );
std::string ReplaceUnderscores( std::string str );

std::string ToLower( std::string str );

int main( int argc, char* args[] )
{
	GameManager gameMan;

	std::string localPlayerName  = "Player 1";
	std::string remotePlayerName = "Player 2";
	unsigned short fpsLimit = 100;

	std::cout << "C++ version : " << __cplusplus << std::endl;
	std::cout << "Args : \n";

	for ( int i = 1; i < argc ; i+=2 )
	{
		if ( argc > ( i + 1 ) )
		{
			std::string str = ToLower( args[i] );

			std::cout << "\t" << i << " : " <<  str << " = " << args[ i + 1 ]  << std::endl;

			if ( str == "-lplayer" && argc > ( i + 1 )  )
				localPlayerName = args[i + 1 ];
			else if ( str == "-rplayer" && argc > ( i + 1 )  )
				remotePlayerName = args[i + 1 ];
			else if ( str == "-fpslimit" && argc > ( i + 1 )  )
				fpsLimit = static_cast< unsigned short > ( std::stoi( args[i + 1] ) );
		}
	}

	localPlayerName  = ReplaceUnderscores( localPlayerName );
	remotePlayerName = ReplaceUnderscores( remotePlayerName );

	gameMan.Init( localPlayerName, remotePlayerName );
	gameMan.SetFPSLimit( fpsLimit );

	gameMan.Run();
}

std::string Replace( std::string str, char replace, char replaceWith )
{
	auto func = [ replace, replaceWith ] ( char c )
	{
		// If ( c == '_' ) c = ' ';

		return ( c != replace ) ?  c : replaceWith;
	
	};

	std::transform( str.begin(), str.end(), str.begin(), func);

	return str;
}

std::string ReplaceUnderscores( std::string str )
{
	return Replace( str, '_', ' ' );
}
std::string ToLower( std::string str )
{
	std::transform( str.begin(), str.end(), str.begin(), ::tolower );
	return str;
}
