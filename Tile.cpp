#include "Tile.h"

#include "math/Vector2f.h"
#include <iostream>
#include <ostream>

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
	} else if ( type == TileTypes::Unbreakable )
	{
		hitsLeft = 821;
	}

	// int bonus = rand() % 1000;
	// std::cout << "Bonus type " << bonus << std::endl;
	bonusType = BonusTypes::Extra_Life;
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

void Tile::ExpandRect( Rect &explodingTile ) const
{
	/*
	double expTileLeft     = explodingTile.x;
	double expTileTop      = explodingTile.y;
	double expTileRight    = explodingTile.x + explodingTile.w;
	double expTileBottom   = explodingTile.y + explodingTile.h;

	double tileLeft     = explodingTile.x;
	double tileTop      = explodingTile.y;
	double tileRight    = explodingTile.x + explodingTile.w;
	double tileBottom   = explodingTile.y + explodingTile.h;
	*/

	Vector2f expTile;
	expTile.x = explodingTile.x / ( explodingTile.w / 2.0 );
	expTile.y = explodingTile.y / ( explodingTile.h / 2.0 );

	Vector2f localTile;
	localTile.x = rect.x / ( rect.w / 2.0 );
	localTile.y = rect.y / ( rect.h / 2.0 );

	Vector2f expDir( localTile - expTile );
	std::cout << "Local tile : " << localTile << std::endl;
	std::cout << "Exp tile : " << expTile << std::endl;
	std::cout << "Exp dir : " << expDir << std::endl;
	//std::cin.ignore();
}
