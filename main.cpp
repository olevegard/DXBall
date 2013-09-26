#include <iostream>
#include <algorithm>
#include <string>
#include <future>

#include "GameManager.h"
#include "math/Rect.h"
#include "NetManager.h"

std::string Replace( const std::string &str, char replace, char replaceWith );
std::string ReplaceUnderscores( const std::string &str );

std::string ToLower( const std::string &str );

SDL_Rect SetResolution( const std::string &str );

bool StrToBool( const std::string &str );

int main( int argc, char* args[] )
{
	NetManager netMan;
	std::cout << "Init" << std::endl;
	netMan.Init();
	netMan.Sent( "this is a message" );
	netMan.Close();

	//netMan.AcceptConnection();
	GameManager gameMan;


	std::string localPlayerName  = "Player 1";
	std::string remotePlayerName = "Player 2";
	unsigned short fpsLimit = 100;
	SDL_Rect resolution;
	resolution.x = 0;
	resolution.y = 0;
	resolution.w = 1920 / 2;
	resolution.h = 1080 / 2;

	bool startFS = false;
	bool startTwoPlayer = false;

	std::cout << "Args : \n";

	for ( int i = 1; i < argc ; i+=2 )
	{
		if ( argc >  i )
		{
			std::string str = ToLower( args[i] );

			std::cout << "\t" << i << " : " <<  str << " = " << args[ i + 1 ]  << std::endl;

			if ( str == "-lplayer" && argc > ( i + 1 )  )
				localPlayerName = args[i + 1 ];
			else if ( str == "-rplayer" && argc > ( i + 1 )  )
				remotePlayerName = args[i + 1 ];
			else if ( str == "-fpslimit" && argc > ( i + 1 )  )
				fpsLimit = static_cast< unsigned short > ( std::stoi( args[i + 1] ) );
			else if ( str == "-resolution" && argc > ( i + 1 )  )
				resolution = SetResolution( args[i + 1 ] );
			else if ( str == "-startfs" && argc > ( i + 1 ) )
				startFS = StrToBool( args[ i + 1 ]);
			else if ( str == "-twoplayer" && argc > ( i + 1 ) )
				startTwoPlayer = StrToBool( args[ i + 1 ]);
		}
	}
	localPlayerName = ReplaceUnderscores( localPlayerName );
	remotePlayerName =  ReplaceUnderscores( remotePlayerName );

	std::cout << "========== CONFIG ==========\n";
	std::cout << "Local player     : " << "\'" << localPlayerName << "\'"  << std::endl;
	std::cout << "Rmote player     : " << "\'" << remotePlayerName << "\'"  << std::endl;
	std::cout << "Frame rate limit : " << fpsLimit << std::endl;
	std::cout << "Resolution       : " << resolution.w << "x" << resolution.h  << std::endl;
	std::cout << "Fullscreen       : " << std::boolalpha << startFS << std::endl;
	std::cout << "2 player mode    : " << std::boolalpha << startTwoPlayer << std::endl;
	std::cout << "============================\n";

	gameMan.SetTwoPlayerMode( startTwoPlayer );
	if ( !gameMan.Init( localPlayerName, remotePlayerName , resolution, startFS  ) )
		return 1;

	gameMan.SetFPSLimit( fpsLimit );

	gameMan.Run();
}

bool StrToBool( const std::string &str )
{
	std::string toBool = ToLower(str);

	if ( toBool == "yes" || toBool == "true" || str == "yup" )
		return true;
	else
		return false;
}
SDL_Rect SetResolution( const std::string &str )
{
	size_t xPos = str.find_first_of( 'x' );
	SDL_Rect r;

	ToLower( str );

	std::string widht = str.substr( 0, xPos );
	std::string height = str.substr( xPos + 1, str.length()  );

	r.x = 0;
	r.y = 0;
	r.w = static_cast< unsigned short > ( std::stoi( widht  ) );
	r.h = static_cast< unsigned short > ( std::stoi( height ) );

	return r;
}

std::string Replace( const std::string &str, char replace, char replaceWith )
{
	std::string retStr = str;

	auto func = [ replace, replaceWith ] ( char c )
	{
		// If ( c == '_' ) c = ' ';

		return ( c != replace ) ?  c : replaceWith;
	
	};
	std::transform( retStr.begin(), retStr.end(), retStr.begin(), func);
	return retStr;
}

std::string ReplaceUnderscores( const std::string &str )
{
	return Replace( str, '_', ' ' );
}

std::string ToLower( const std::string &str )
{
	std::string retStr = str;
	std::transform( retStr.begin(), retStr.end(), retStr.begin(), ::tolower );

	return retStr;

}
