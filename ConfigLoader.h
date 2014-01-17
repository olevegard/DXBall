#pragma once

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <map>

#include "enums/TileType.h"
#include "enums/BonusType.h"

#include <SDL2/SDL.h>

#include "tools/SDLColorHelper.h"

class ConfigLoader
{
	public:
	ConfigLoader()
		:	ballSpeed( 0.2 )
		,	bonusBoxSpeed( 0.2 )
	{

	}
	void LoadConfig();

	double GetBallSpeed() const
	{
		return ballSpeed;
	}
	double GetBallSpeedFastMode() const
	{
		return ballSpeedFastMode;
	}
	double GetBulletSpeed() const
	{
		return bulletSpeed;
	}
	double GetBonusBoxSpeed() const
	{
		return bonusBoxSpeed;
	}
	bool IsFastMode() const
	{
		return isFastMode;
	}
	int32_t GetBonusBoxChance() const
	{
		return bonusBoxChance;
	}
	int32_t GetTilePoints( const TileType &tt )
	{
		return points[tt];
	}
	std::map< TileType, int32_t > GetPoints() const
	{
		return points;
	}
	private:
	void PrintColor( const std::string &colorName, const SDL_Color &color );
	void PrintIndented( const std::string &colorName );
	void PrintConfig();
	std::string RemoveCharacterFromString( std::string str, char ch );

	double ballSpeed;
	double ballSpeedFastMode;
	double bulletSpeed;
	double bonusBoxSpeed;

	bool isFastMode;

	int32_t bonusBoxChance;

	std::map< TileType, int32_t > points;
};

