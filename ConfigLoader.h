#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

#include "enums/TileType.h"
#include "enums/BonusType.h"

#include "TileColor.h"


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

	double GetBonusBoxSpeed() const
	{
		return bonusBoxSpeed;
	}
	private:
	void PrintColor( const std::string &colorName, const SDL_Color &color );
	void PrintIndented( const std::string &colorName );
	void PrintConfig();
	std::string RemoveCharacterFromString( std::string str, char ch );

	double ballSpeed;
	double bonusBoxSpeed;
};

