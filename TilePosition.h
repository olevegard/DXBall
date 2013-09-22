#pragma once

#include "Tile.h"


// The location and type of a single tile
struct TilePosition
{
	TilePosition();
	TilePosition( short xPos_, short yPos_, const TileType &tt );

	short xPos;
	short yPos;

	TileType type;
};


