#pragma once

#include "Board.h"

#include <string>

struct SDL_Rect;
struct TilePosition;
struct BoardLoader
{
	BoardLoader();

	void BuildLevelList();

	Board LoadLevel( const std::string &textFile );

	std::vector< TilePosition > GenerateBoard( const SDL_Rect &rect );

	void Reset( )
	{
		currentLevel = 0;
	}

	void SetIsServer( bool server )
	{
		isServer = server;
	}

	private:
		size_t currentLevel;
		std::vector< std::string > levelTextFiles;
		std::vector< Board > levels;
		bool isServer;
};
