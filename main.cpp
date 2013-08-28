#include <iostream>

#include "GameManager.h"

std::string ReplaceUnderscores( std::string str )
{
	for ( size_t i = 0 ; i < str.size() ; ++i )
		if ( str[i] == '_' )
			str[i] = ' ';

	return str;
}
int main( int argc, char* args[] )
{
	GameManager gameMan;

	std::string name = "Ole Vegard";

	std::cout << "C++ version : " << __cplusplus << std::endl;
	std::cout << "Args : \n";

	for ( int i = 0; i < argc ; ++i )
	{
		std::cout << "\t" << args[i] << std::endl;

		std::string str = args[i];

		if ( str == "-name" && argc > ( i + 1 )  )
			name = args[i + 1 ];
	}

	name = ReplaceUnderscores( name );

	gameMan.Init( name, "Someone Else");

	gameMan.Run();
}
