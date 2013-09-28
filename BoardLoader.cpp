#include "BoardLoader.h"

#include "Tile.h"
#include "Board.h"
#include "TilePosition.h"
#include "TilePositionHelpers.h"

#include <algorithm>

#include <iostream>
#include <fstream>
#include <sstream>

BoardLoader::BoardLoader()
	:	currentLevel( 0 )
	,	levelTextFiles( 0 )
	,	levels(  )
{
	BuildLevelList();
}

void BoardLoader::BuildLevelList()
{
	std::string line;
	std::ifstream boardFile( "boards/boardlist.txt" );

	while ( getline( boardFile, line ) )
	{
		if ( line[0] == '#' || line.empty() )
		{
			std::cout << "Skipping : " << line << std::endl;
			std::cout << "===========================================================================\n";
			continue;
		} else
		{
			std::cout << "Adding : " << line << std::endl;
		}

		levelTextFiles.push_back( line );
	}
}

Board BoardLoader::LoadLevel( const std::string &textFile )
{
	std::string filePath = "boards/";
	filePath.append( textFile );
	std::cout << "Loading level : " << filePath << std::endl;

	std::ifstream boardFile( filePath );
	TilePosition pos;
	std::string line;
	Board board;
	while ( getline( boardFile, line ) )
	{
		if ( line[0] == '#' || line.empty() )
			continue;

		std::stringstream( line ) >> pos;
		std::cout << "\tAddiing tile...\n";
		board.AddTile( pos );
	}
	return board;
}
std::vector< TilePosition > BoardLoader::GenerateBoard( )
{
	if ( currentLevel >= levelTextFiles.size() )
	{
		std::vector< TilePosition > emptyVec;
		return emptyVec;
	}

	std::string level = levelTextFiles[ currentLevel ];

	Board b = LoadLevel( level  );
	levels.push_back( b );
	++currentLevel;

	return b.GetTiles();
}
