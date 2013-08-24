#include "Tile.h"

Tile::Tile(TileTypes type_, unsigned int tileID_ )
	:	type( type_ )
	,	hitsLeft( 1 )
	,	tileID( tileID_ )
{
	type = type_;
	tileID = tileID_;

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
