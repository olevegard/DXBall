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
		if ( line[0] == '#' || line.empty() || !DoesFileExist( "boards/" + line ))
			continue;

		std::cout << "Added file : " << line << std::endl;

		levelTextFiles.push_back( line );
	}
}
Board BoardLoader::LoadLevel( const std::string &textFile )
{
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
bool BoardLoader::IsLastLevel()
{
	return currentLevel == levelTextFiles.size();
}
Board BoardLoader::GenerateBoard( const SDL_Rect &rect )
{
	std::string level = "boards/" + levelTextFiles[ currentLevel++ ];
	Board b = LoadLevel( level );
	levels.push_back( b );

	b.CenterAndFlip( rect );
	b.CalcMaxScale( rect );

	return b;
}
bool BoardLoader::DoesFileExist( const std::string &fileName ) const
{
	std::ifstream file( fileName );

	bool exists = file.good();

	if ( !exists )
		std::cout << "BoardLoader@" << __LINE__ << " : " << fileName << " doesn't exist\n";
	else
		std::cout << "BoardLoader@" << __LINE__ << " : " << fileName << " exists\n";

	file.close();

	return exists;
}
