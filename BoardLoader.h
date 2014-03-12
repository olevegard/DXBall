#pragma once

#include "structs/board/Board.h"

struct SDL_Rect;
struct TilePosition;
struct BoardLoader
{
	BoardLoader();

	void BuildLevelList();

	Board LoadLevel( const std::string &textFile );

	Board GenerateBoard( const SDL_Rect &rect );

	bool IsLastLevel();

	void Reset( )
	{
		currentLevel = 0;
	}
	private:
	bool DoesFileExist( const std::string &fileName ) const;

	size_t currentLevel;
	std::vector< std::string > levelTextFiles;
	std::vector< Board > levels;
};
