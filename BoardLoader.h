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
	double GetResolutionX() const
	{
		return currentResX;
	}
	double GetResolutionY() const
	{
		return currentResY;
	}
	double GetScale()
	{
		return scale;
	}
	private:
		double scale;
		double currentResX;
		double currentResY;
		size_t currentLevel;
		std::vector< std::string > levelTextFiles;
		std::vector< Board > levels;
		bool isServer;
};
