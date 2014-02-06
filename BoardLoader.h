#pragma once

#include "structs/board/Board.h"

#include <string>

struct SDL_Rect;
struct TilePosition;
struct BoardLoader
{
	BoardLoader();

	void BuildLevelList();

	Board LoadLevel( const std::string &textFile );

	Board GenerateBoard( const SDL_Rect &rect );

	bool IsLastLevel() const;

	void Reset( )
	{
		currentLevel = 0;
	}

	void SetIsServer( bool server )
	{
		isServer = server;
	}
	private:
	bool DoesFileExist( const std::string &fileName ) const;
	std::string FindNextExistingBoard();

		size_t currentLevel;
		std::vector< std::string > levelTextFiles;
		std::vector< Board > levels;
		bool isServer;
};
