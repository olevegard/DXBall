#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include "enums/TileType.h"
#include "TileColor.h"

class ConfigLoader
{
	public:
	void LoadConfig()
	{
		std::ifstream configFile( "media/ColorCfg.txt" );
		std::string configLine;
		std::cout << "ConfigLoader.cpp@" << __LINE__ << " Loading config...";

		while ( getline( configFile, configLine ) )
		{

			if ( configLine[0] == '#' || configLine.empty() )
				continue;

			configLine = RemoveCharacterFromString( configLine, '|' );

			std::stringstream ss(configLine);
			TileColor tc;
			ss >> tc;
			colorConfig.push_back( tc );
		}

		PrintConfig();
	}
	SDL_Color GetTileColor( TileType type_, int32_t colorIndex = 0)
	{
		for ( const auto &p :  colorConfig )
		{
			if ( p.type == type_ )
				return p.colors[colorIndex];

		}
		return SDL_Color{ 255, 255, 255, 255 };
	}
	private:
	void PrintConfig()
	{
		for ( const auto &p : colorConfig )
			std::cout << "\n\t" << p;
		std::cout << std::endl;
	}
	std::string RemoveCharacterFromString( std::string str, char ch )
	{
		auto newEnd = std::remove_if( str.begin(), str.end(), [&ch]( char c) { return c == ch; } );
		str.erase( newEnd, str.end() );

		return str;
	}
	std::vector< TileColor > colorConfig;
};

