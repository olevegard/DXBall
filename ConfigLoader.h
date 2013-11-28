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

			// local_player_color
			if (  configLine.find( "local_player_color" ) != std::string::npos )
			{
				std::stringstream ss(configLine);
				std::string str;
				ss >> str;
				ss >> localPlayerColor;
				PrintColor( "local player color", localPlayerColor);
				continue;
			}
			else if (  configLine.find( "remote_player_color" ) != std::string::npos )
			{
				std::stringstream ss(configLine);
				std::string str;
				ss >> str;
				ss >> remotePlayerColor;
				PrintColor( "remote player color", remotePlayerColor);
				continue;
			}
			else if (  configLine.find( "background_color" ) != std::string::npos )
			{
				std::stringstream ss(configLine);
				std::string str;
				ss >> str;
				ss >> backgroundColor;
				PrintColor( "background color", backgroundColor);
				continue;
			}
			else if (  configLine.find( "text_color" ) != std::string::npos )
			{
				std::stringstream ss(configLine);
				std::string str;
				ss >> str;
				ss >> textColor;
				PrintColor( "text color", textColor);
				continue;
			}
			else if (  configLine.find( "grey_area " ) != std::string::npos )
			{
				std::stringstream ss(configLine);
				std::string str;
				ss >> str;
				ss >> greayAreaColor;;
				PrintColor( "grey area color", greayAreaColor);
				continue;
			}
			else if (  configLine.find( "bonus_extra_life " ) != std::string::npos )
			{
				std::stringstream ss(configLine);
				std::string str;
				SDL_Color color;
				ss >> str;
				ss >> color;;
				PrintColor( "bonus extra life", color );
				bonusTypeColors.insert( std::make_pair( BonusType::ExtraLife, color ) );
				continue;
			}
			else if (  configLine.find( "bonus_death" ) != std::string::npos )
			{
				std::stringstream ss(configLine);
				std::string str;
				SDL_Color color;
				ss >> str;
				ss >> color;;
				PrintColor( "bonus death", color );
				bonusTypeColors.insert( std::make_pair( BonusType::Death, color ) );
				continue;
			}
			else if ( configLine[0] == '#' || configLine.empty() )
				continue;

			configLine = RemoveCharacterFromString( configLine, '|' );

			std::stringstream ss(configLine);
			TileColor tc;
			ss >> tc;
			colorConfig.push_back( tc );
		}

		PrintConfig();
	}
	void PrintColor( const std::string &colorName, const SDL_Color &color )
	{
		PrintIndented( colorName );
		std::cout << color;
	}
	void PrintIndented( const std::string &colorName )
	{
		std::cout << "\n\t" << std::setfill( '_' ) << std::setw( 23 ) << std::left << colorName;
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
	SDL_Color GetLocalPlayerColor() const
	{
		return localPlayerColor;
	}

	SDL_Color GetRemotePlayerColor() const
	{
		return remotePlayerColor;
	}

	SDL_Color GetBackgroundColor() const
	{
		return backgroundColor;
	}

	SDL_Color GetTextColor() const
	{
		return textColor;
	}

	SDL_Color GetGreyArea() const
	{
		return greayAreaColor;
	}
	std::map< BonusType, SDL_Color > GetBonusColorMap()
	{
		return bonusTypeColors;
	}
	private:
	void PrintConfig()
	{
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
	}
	std::string RemoveCharacterFromString( std::string str, char ch )
	{
		auto newEnd = std::remove_if( str.begin(), str.end(), [&ch]( char c) { return c == ch; } );
		str.erase( newEnd, str.end() );

		return str;
	}
	std::vector< TileColor > colorConfig;

	SDL_Color localPlayerColor;
	SDL_Color remotePlayerColor;
	SDL_Color backgroundColor;
	SDL_Color textColor;
	SDL_Color greayAreaColor;

	std::map< BonusType, SDL_Color > bonusTypeColors;
};

