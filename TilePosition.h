#pragma once

#include "Tile.h"

#include <iomanip>
#include <iostream>

// The location and type of a single tile
struct TilePosition
{
	TilePosition();
	TilePosition( short xPos_, short yPos_, const TileTypes &tt );

	short xPos;
	short yPos;

	TileTypes type;
};

inline std::istream& operator>>( std::istream &is, TilePosition &pos )
{
	int type = 0;
	is >> pos.xPos >> pos.yPos >> type;
	pos.type = static_cast< TileTypes > ( type );

	if ( type < 0 || type > 5 )
	{
		is.setstate( std::ios_base::failbit );
		std::cout << "Error! " << type << " is not a valid tyle type!" << std::endl;
	}

	return is;
}
inline std::ostream& operator<<( std::ostream &os, const TilePosition &pos )
{
	os << std::setw(4) << pos.xPos << " " << std::setw(4) << pos.yPos << " ";

	switch ( pos.type )
	{
		case TileTypes::Regular:
			os << "Regular";
			break;
		case TileTypes::Hard:
			os << "Hard";
			break;
		case TileTypes::Unbreakable:
			os << "Unbreakable";
			break;
		case TileTypes::Explosive:
			os << "Explosive";
			break;
		default:
			os << "Unknown";
			break;

	}

	os << std::endl;

	return os;
}
