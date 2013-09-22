#pragma once

#include "Board.h"

#include <string>

struct TilePosition;
struct BoardLoader
{
	BoardLoader();

	void BuildLevelList();

	Board LoadLevel( const std::string &textFile );

	std::vector< TilePosition > GenerateBoard( );

	private:
		size_t currentLevel;
		std::vector< std::string > levelTextFiles;
		std::vector< Board > levels;
};
