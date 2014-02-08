#include "TilePosition.h"

	TilePosition::TilePosition()
	:	TilePosition( 0, 0, TileType::Regular )
{
}
	TilePosition::TilePosition( double xPos, double yPos, const TileType &tt )
	:	tilePos( xPos, yPos)
	,	type( tt )
{

}


