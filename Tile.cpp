#include "Tile.h"

Tile::Tile(TileTypes type_ )
	:	type( type_ )
	,	hitsLeft( 1 )
{
	type = type_;

	if ( type == TileTypes::Regular || type == TileTypes::Explosive  )
	{
		hitsLeft = 1;
	} else if ( type == TileTypes::Hard )
	{
		hitsLeft = 5;
	}
}
Tile::~Tile()
{

}
