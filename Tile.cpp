#include "Tile.h"

#include "math/Vector2f.h"
#include "math/VectorHelpers.h"

#include <iostream>
#include <ostream>

Tile::Tile(TileType type_, unsigned int tileID_ )
	:	type( type_ )
	,	hitsLeft( 1 )
{
	type = type_;
	SetObjectID( tileID_ );

	if ( type == TileType::Regular || type == TileType::Explosive  )
	{
		hitsLeft = 1;
	} else if ( type == TileType::Hard )
	{
		hitsLeft = 5;
	} else if ( type == TileType::Unbreakable )
	{
		hitsLeft = 821;
	}
}
Tile::~Tile()
{

}
void Tile::Hit()
{
	// Decrease hits left
	--hitsLeft;
}
bool Tile::CheckExplosion( const Rect &explodingTile )
{
	return explodingTile.x > 2.0;
}
