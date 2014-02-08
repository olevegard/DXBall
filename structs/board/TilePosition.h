#pragma once

#include "structs/game_objects/Tile.h"


// The location and type of a single tile
struct TilePosition
{
	TilePosition();
	TilePosition( double xPos_, double yPos_, const TileType &tt );

	Vector2f tilePos;

	TileType type;
};


