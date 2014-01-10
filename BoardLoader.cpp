#include "BoardLoader.h"

#include "structs/game_objects/Tile.h"
#include "structs/board/TilePosition.h"
#include "tools/TilePositionHelpers.h"

#include <algorithm>

#include <iostream>
#include <fstream>
#include <sstream>


#include <SDL2/SDL.h>

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

	std::cout << "BoardLoader@" << __LINE__ << " Adding board files : \n";
	while ( getline( boardFile, line ) )
	{
		if ( line[0] == '#' || line.empty() )
		{
			std::cout << "Skipping : " << line << std::endl;
			continue;
		}

		std::cout << "\tAdding : " << line << std::endl;
		levelTextFiles.push_back( line );
	}
}

Board BoardLoader::LoadLevel( const std::string &textFile )
{
	std::string filePath = "boards/";
	filePath.append( textFile );
	std::cout << "BoardLoader@" << __LINE__ << " Loading level : " << filePath << std::endl;

	std::ifstream boardFile( filePath );
	TilePosition pos;
	std::string line;
	Board board;
	while ( getline( boardFile, line ) )
	{
		std::stringstream ss( line );

		if ( line[0] == '#' || line.empty() )
			continue;

		ss >> pos;
		board.AddTile( pos );
	}
	return board;
}

bool BoardLoader::IsLastLevel() const
{
	return ! ( currentLevel >= levelTextFiles.size() );
}
Board BoardLoader::GenerateBoard( const SDL_Rect &rect )
{
	std::string level = levelTextFiles[ currentLevel ];

	Board b = LoadLevel( level  );
	levels.push_back( b );

	b.CenterAndFlip( rect, isServer );
	b.CalcMaxScale( rect );

	++currentLevel;

	return b;
}
