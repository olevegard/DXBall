#include <iostream>
#include <algorithm>
#include <string>

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
	std::string localPlayerName  = "Player 1";
	unsigned short fpsLimit = 100;
	SDL_Rect resolution;
	resolution.x = 0;
	resolution.y = 0;
	resolution.w = 1920 / 2;
	resolution.h = 1080 / 2;
	std::string ip = "127.0.0.1";
	unsigned short port = 2002;

	bool startFS = false;
	bool startTwoPlayer = false;
	bool isServer = false;
	bool isAIControlled = false;

	std::cout << "Args : \n";

	for ( int i = 1; i < argc ; i+=2 )
	{
		if ( argc >  i )
		{
			std::string str = ToLower( args[i] );

			std::cout << "\t" << i << " : " <<  str << " = " << args[ i + 1 ] << std::endl;

			if ( str == "-lplayer" && argc > ( i + 1 )  )
				localPlayerName = args[i + 1 ];
			else if ( str == "-fpslimit" && argc > ( i + 1 )  )
				fpsLimit = static_cast< unsigned short >( std::stoi( args[i + 1] ) );
			else if ( str == "-resolution" && argc > ( i + 1 )  )
				resolution = SetResolution( args[i + 1 ] );
			else if ( str == "-startfs" && argc > ( i + 1 ) )
				startFS = StrToBool( args[ i + 1 ]);
			else if ( str == "-twoplayer" && argc > ( i + 1 ) )
				startTwoPlayer = StrToBool( args[ i + 1 ]);
			else if ( str == "-server" && argc > ( i + 1 ) )
				isServer = StrToBool( args[ i + 1 ]);
			else if ( str == "-ip" && argc > ( i + 1 ) )
				ip = args[ i + 1 ];
			else if ( str == "-port" && argc > ( i + 1 ) )
				port = static_cast<unsigned short >( std::stoi( args[ i + 1 ] ) );
			else if ( str == "-aicontrolled" && argc > ( i + 1 ) )
				isAIControlled = StrToBool( args[ i + 1 ]);
		}
	}

	localPlayerName = ReplaceUnderscores( localPlayerName );

	std::cout << "========== CONFIG ==========\n";
	std::cout << "Local player     : " << "\'" << localPlayerName << "\'"  << std::endl;
	std::cout << "Frame rate limit : " << fpsLimit << std::endl;
	std::cout << "Resolution       : " << resolution.w << "x" << resolution.h << std::endl;
	std::cout << "Fullscreen       : " << std::boolalpha << startFS << std::endl;
	std::cout << "2 player mode    : " << std::boolalpha << startTwoPlayer << std::endl;
	std::cout << "Is server        : " << std::boolalpha << isServer << std::endl;
	std::cout << "IP               : " << ip << std::endl;
	std::cout << "Port             : " << port << std::endl;
	std::cout << "AI Controlled    : " << isAIControlled << std::endl;
	std::cout << "============================\n";

	GameManager gameMan;
	if ( !gameMan.Init( localPlayerName, resolution, startFS  ) )
		return 1;

	gameMan.SetFPSLimit( fpsLimit );
	gameMan.SetAIControlled( isAIControlled );
	gameMan.InitNetManager( isServer, ip, port );
	gameMan.Run();
	return 0;
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
