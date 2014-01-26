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

	while ( getline( boardFile, line ) )
	{
		if ( line[0] == '#' || line.empty() )
			continue;

		levelTextFiles.push_back( line );
	}
}

Board BoardLoader::LoadLevel( const std::string &textFile )
{
	std::string filePath = "boards/";
	filePath.append( textFile );

	Board board;
	TilePosition pos;
	std::string line;

	std::ifstream boardFile( filePath );

	if ( !boardFile.is_open() )
		return board;

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
