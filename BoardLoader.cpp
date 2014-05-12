#include "BoardLoader.h"

#include "structs/game_objects/Tile.h"
#include "structs/board/TilePosition.h"
#include "tools/TilePositionHelpers.h"

#include <algorithm>

#include <fstream>
#include <sstream>

#include "Logger.h"

#include <SDL2/SDL.h>

BoardLoader::BoardLoader()
	:	currentLevel( 0 )
	,	levelTextFiles( 0 )
	,	levels(  )
{
	logger = Logger::Instance();
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

		logger->Log( __FILE__, __LINE__, "Added file : ", line );

		levelTextFiles.push_back( line );
	}
}
Board BoardLoader::LoadLevel( const std::string &textFile )
{
	Board board;
	board.levelName = textFile;

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
		logger->Log( __FILE__, __LINE__, "File not found : ", fileName );

	file.close();

	return exists;
}
