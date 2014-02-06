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
	std::cout << "Loading level : " << textFile << std::endl;
	Board board;
	TilePosition pos;
	std::string line;

	std::ifstream boardFile( textFile );

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
	Board b = LoadLevel( FindNextExistingBoard() );
	levels.push_back( b );

	b.CenterAndFlip( rect, isServer );
	b.CalcMaxScale( rect );

	++currentLevel;

	return b;
}

std::string BoardLoader::FindNextExistingBoard()
{
	std::string level;
	do
	{
		level = "boards/" + levelTextFiles[ currentLevel ];
		++currentLevel;
	}
	while( !DoesFileExist( level ) && currentLevel < levelTextFiles.size()  );

	return level;
}
bool BoardLoader::DoesFileExist( const std::string &fileName ) const
{
	std::ifstream file( fileName );

	bool exists = file.good();
	if ( !exists )
		std::cout << "BoardLoader@" << __LINE__ << " : " << fileName << " doesn't exist\n";
	file.close();

	return exists;
}
