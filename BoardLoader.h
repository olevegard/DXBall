#pragma once

#include "TilePosition.h"
#include "Board.h"

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
