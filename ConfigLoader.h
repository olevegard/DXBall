#pragma once

#include <string>

#include <vector>
#include <map>

#include "enums/TileType.h"

#include <SDL2/SDL.h>

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
	int32_t GetPointsHit()
	{
		return pointsHit;
	}

	double ballSpeed;
	private:
	void PrintColor( const std::string &colorName, const SDL_Color &color );
	void PrintIndented( const std::string &colorName );
	void PrintConfig();
	std::string RemoveCharacterFromString( std::string str, char ch );

	double ballSpeedFastMode;
	double bulletSpeed;
	double bonusBoxSpeed;

	bool isFastMode;

	int32_t bonusBoxChance;

	std::map< TileType, int32_t > points;
	int32_t pointsHit;
};

