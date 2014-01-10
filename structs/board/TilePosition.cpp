#include "TilePosition.h"

	TilePosition::TilePosition()
	:	TilePosition( 0, 0, TileType::Regular )
{
}
	TilePosition::TilePosition( short xPos_, short yPos_, const TileType &tt )
	:	xPos(xPos_)
	,	yPos(yPos_)
	,	type( tt )
{

}


