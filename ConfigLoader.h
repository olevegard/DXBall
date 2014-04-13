#pragma once

#include <string>

#include <vector>
#include <map>

#include "enums/TileType.h"
#include "enums/PlussMin.h"
#include "enums/ConfigValueType.h"

#include <SDL2/SDL.h>

class ConfigLoader
{
	public:
	ConfigLoader()
	{

	}
	void LoadConfig();

	bool IsFastMode() const
	{
		return isFastMode;
	}
	double GetTilePoints( const TileType &tt )
	{
		return points[tt];
	}
	std::map< TileType, int32_t > GetPoints() const
	{
		return points;
	}
	void ApplyChange( ConfigValueType config, double value, PlussMin plussMin )
	{
		if ( plussMin == PlussMin::Pluss )
			configValues[config] += value;
		else if ( plussMin == PlussMin::Minus )
			configValues[config] -= value;
	}
	double Get( ConfigValueType config ) const
	{
		return configValues.at( config );
	}
	void Set( double value, ConfigValueType config )
	{
		configValues[config] = value;
	}

	bool GetFastMode( ) const
	{
		return isFastMode;
	}

	private:
	void PrintColor( const std::string &colorName, const SDL_Color &color );
	void PrintIndented( const std::string &colorName );
	void PrintConfig();
	std::string RemoveCharacterFromString( std::string str, char ch );

	std::map< ConfigValueType, double > configValues;
	std::map< TileType, int32_t > points;

	bool isFastMode;

};

