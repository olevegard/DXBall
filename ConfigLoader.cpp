#include "ConfigLoader.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include <algorithm>

#include "tools/SDLColorHelper.h"

void ConfigLoader::LoadConfig()
{
	std::ifstream configFile( "config/Config.txt" );
	std::string configLine;

	while ( getline( configFile, configLine ) )
	{
		if ( configLine[0] == '#' || configLine.empty() )
			continue;

		std::stringstream ss(configLine);
		std::string str;
		ss >> str;

		if (  configLine.find( "is_fast_mode" ) != std::string::npos )
		{
			std::string fastMode;
			ss >> fastMode;
			isFastMode = ( fastMode.find( "true" ) != std::string::npos );
		}
		else if (  configLine.find( "ball_speed_normal" ) != std::string::npos )
			ss >> configValues[ ConfigValueType::BallSpeed ];
		else if (  configLine.find( "ball_speed_fastmode" ) != std::string::npos )
			ss >> configValues[ ConfigValueType::BallSpeed_FM ];
		else if (  configLine.find( "bullet_speed" ) != std::string::npos )
			ss >> configValues[ ConfigValueType::BulletSpeed ];
		else if (  configLine.find( "bonus_box_speed" ) != std::string::npos )
			ss >> configValues[ ConfigValueType::BonusBoxSpeed ];
		else if (  configLine.find( "bonus_box_chance" ) != std::string::npos )
			ss >> configValues[ ConfigValueType::BonusBoxChance ];
		else if (  configLine.find( "points_regular" ) != std::string::npos )
			ss >> points[TileType::Regular];
		else if (  configLine.find( "points_hard" ) != std::string::npos )
			ss >> points[TileType::Hard];
		else if (  configLine.find( "points_explosive" ) != std::string::npos )
			ss >> points[TileType::Explosive];
		else if (  configLine.find( "points_unbreakable" ) != std::string::npos )
			ss >> points[TileType::Unbreakable];
		else if (  configLine.find( "points_hit" ) != std::string::npos )
			ss >> configValues[ ConfigValueType::PointsHit ];
	}

	PrintConfig();
}
void ConfigLoader::PrintColor( const std::string &colorName, const SDL_Color &color )
{
	PrintIndented( colorName );
	std::cout << color;
}
void ConfigLoader::PrintIndented( const std::string &colorName )
{
	std::cout << "\n\t" << std::setfill( '_' ) << std::setw( 23 ) << std::left << colorName;
}

void ConfigLoader::PrintConfig()
{
	/*
	for ( const auto &p : colorConfig )
	{
		if ( p.IsMultiColored()  )
		{
			PrintIndented( p.GetTileTypeAsString() );
			for ( int i = 0; i < 5; ++i )
			{
				std::stringstream ss;
				ss << "\tColor[" << ( i + 1 ) << "]";
				std::string str = "\t";
				PrintColor( ss.str(), p.colors[i] );
			}
		}
		else
		{
			PrintColor( p.GetTileTypeAsString(), p.colors[0] );
		}
	}
	//std::cout << "\n\t" << p;
	std::cout << std::endl;
	*/
}
std::string ConfigLoader::RemoveCharacterFromString( std::string str, char ch )
{
	auto newEnd = std::remove_if( str.begin(), str.end(), [&ch]( char c) { return c == ch; } );
	str.erase( newEnd, str.end() );

	return str;
}
