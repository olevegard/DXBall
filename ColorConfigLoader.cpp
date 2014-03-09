#include "ColorConfigLoader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

void ColorConfigLoader::LoadConfig()
{
	std::ifstream configFile( "config/ColorCfg.txt" );
	std::string configLine;

	while ( getline( configFile, configLine ) )
	{
		ApplyConfig( configLine );
	}
}
void ColorConfigLoader::ApplyConfig( std::string str )
{
	std::string parameterName;
	std::stringstream line( str );

	line >> parameterName;

	if ( parameterName == "#" || str.empty() )
		return;
	else if (  parameterName ==  "local_player_color" )
		line >> localPlayerColor;
	else if (  parameterName ==  "remote_player_color" )
		line >> remotePlayerColor;
	else if (  parameterName ==  "background_color" )
		line >> backgroundColor;
	else if (  parameterName ==  "text_color" )
		line >> textColor;
	else if (  parameterName ==  "grey_area" )
		line >> greyAreaColor;
	else if (  parameterName ==  "prticle_fire_count" )
		line >> particleFireCount;
	else if (  parameterName ==  "particle_decay_min" )
		line >> particleDecayMin;
	else if (  parameterName ==  "particle_decay_max" )
		line >>  particleDecayMax;
	else if (  parameterName ==  "particle_speed_min" )
		line >> particleSpeedMin;
	else if (  parameterName ==  "particle_speed_max" )
		line >> particleSpeedMax;
	else if (  parameterName ==  "bonus_extra_life" )
	{
		SDL_Color color;
		line >> color;
		bonusTypeColors.insert( std::make_pair( BonusType::ExtraLife, color ) );
	}
	else if (  parameterName ==  "bonus_death" )
	{
		SDL_Color color;
		line >> color;
		bonusTypeColors.insert( std::make_pair( BonusType::Death, color ) );
	}
	else if (  parameterName ==  "bonus_super_ball" )
	{
		SDL_Color color;
		line >> color;
		bonusTypeColors.insert( std::make_pair( BonusType::SuperBall, color ) );
	}
	else if (  parameterName ==  "bonus_fire_bullets" )
	{
		SDL_Color color;
		line >> color;
		bonusTypeColors.insert( std::make_pair( BonusType::FireBullets, color ) );
	}
	else
	{
		str = RemoveCharacterFromString( str, '|' );

		std::stringstream ss( str );
		TileColor tc;
		ss >> tc;
		colorConfig.push_back( tc );
	}
}
void ColorConfigLoader::PrintColor( const std::string &colorName, const SDL_Color &color )
{
	PrintIndented( colorName );
	std::cout << color;
}
void ColorConfigLoader::PrintIndented( const std::string &colorName )
{
	std::cout << "\n\t" << std::setfill( '_' ) << std::setw( 23 ) << std::left << colorName;
}
SDL_Color ColorConfigLoader::GetTileColor( TileType type_, uint64_t colorIndex)
{
	for ( const auto &p :  colorConfig )
	{
		if ( p.type == type_ )
			return p.colors[colorIndex];

	}
	return SDL_Color{ 255, 255, 255, 255 };
}
SDL_Color ColorConfigLoader::GetLocalPlayerColor() const
{
	return localPlayerColor;
}

SDL_Color ColorConfigLoader::GetRemotePlayerColor() const
{
	return remotePlayerColor;
}

SDL_Color ColorConfigLoader::GetBackgroundColor() const
{
	return backgroundColor;
}

SDL_Color ColorConfigLoader::GetTextColor() const
{
	return textColor;
}

SDL_Color ColorConfigLoader::GetGreyArea() const
{
	return greyAreaColor;
}
std::map< BonusType, SDL_Color > ColorConfigLoader::GetBonusColorMap()
{
	return bonusTypeColors;
}
void ColorConfigLoader::PrintConfig()
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
std::string ColorConfigLoader::RemoveCharacterFromString( std::string str, char ch )
{
	auto newEnd = std::remove_if( str.begin(), str.end(), [&ch]( char c) { return c == ch; } );
	str.erase( newEnd, str.end() );

	return str;
}
