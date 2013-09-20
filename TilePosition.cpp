#include "TilePosition.h"

	TilePosition::TilePosition()
	:	TilePosition( 0, 0, TileTypes::Regular )
{
}
	TilePosition::TilePosition( short xPos_, short yPos_, const TileTypes &tt )
	:	xPos(xPos_)
	,	yPos(yPos_)
	,	type( tt )
{

}


