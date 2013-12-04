#include "ConfigLoader.h"

void ConfigLoader::LoadConfig()
{
	std::ifstream configFile( "config/Config.txt" );
	std::string configLine;
	std::cout << "ConfigLoader.cpp@" << __LINE__ << " Loading config...";

	while ( getline( configFile, configLine ) )
	{
		if ( configLine[0] == '#' || configLine.empty() )
			continue;

		std::stringstream ss(configLine);
		std::string str;
		ss >> str;
		std::cout << "\n\t" << std::setfill( '_' ) << std::setw( 23 ) << std::left << str;

		if (  configLine.find( "ball_speed_normal" ) != std::string::npos )
		{
			ss >> ballSpeed;;
			std::cout << ballSpeed;
			continue;
		}
		if (  configLine.find( "ball_speed_fastmode" ) != std::string::npos )
		{
			ss >> ballSpeedFastMode;;
			std::cout << ballSpeedFastMode;
			continue;
		}
		if (  configLine.find( "bullet_speed" ) != std::string::npos )
		{
			ss >> bulletSpeed;;
			std::cout << bulletSpeed;
			continue;
		}
		else if (  configLine.find( "bonus_box_speed" ) != std::string::npos )
		{
			ss >> bonusBoxSpeed;;
			std::cout << bonusBoxSpeed;
			continue;
		}
		else if (  configLine.find( "is_fast_mode" ) != std::string::npos )
		{
			std::string fastMode;
			ss >> fastMode;
			isFastMode = ( fastMode.find( "true" ) != std::string::npos );
			std::cout << fastMode << " : " << std::boolalpha << isFastMode;
			continue;
		}
		else if (  configLine.find( "bonus_box_chance" ) != std::string::npos )
		{
			ss >> bonusBoxChance;
			std::cout << bonusBoxChance;
			continue;
		}

		else


		{
			std::cout << "Unknown config\n";
		}

		//configLine = RemoveCharacterFromString( configLine, '|' );

	}
	std::cout << std::endl;

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
