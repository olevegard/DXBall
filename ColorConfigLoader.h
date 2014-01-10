#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

#include "enums/TileType.h"
#include "enums/BonusType.h"

#include "structs/board/TileColor.h"

class ColorConfigLoader
{
	public:
	void LoadConfig();
	void PrintColor( const std::string &colorName, const SDL_Color &color );
	void PrintIndented( const std::string &colorName );
	SDL_Color GetTileColor( TileType type_, int32_t colorIndex = 0);
	SDL_Color GetLocalPlayerColor() const;

	SDL_Color GetRemotePlayerColor() const;

	SDL_Color GetBackgroundColor() const;

	SDL_Color GetTextColor() const;

	SDL_Color GetGreyArea() const;
	std::map< BonusType, SDL_Color > GetBonusColorMap();
	private:
	void PrintConfig();
	std::string RemoveCharacterFromString( std::string str, char ch );

	std::vector< TileColor > colorConfig;

	SDL_Color localPlayerColor;
	SDL_Color remotePlayerColor;
	SDL_Color backgroundColor;
	SDL_Color textColor;
	SDL_Color greayAreaColor;

	std::map< BonusType, SDL_Color > bonusTypeColors;
};

