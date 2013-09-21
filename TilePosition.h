#pragma once

#include "Tile.h"

#include <iomanip>
#include <iostream>

// The location and type of a single tile
struct TilePosition
{
	TilePosition();
	TilePosition( short xPos_, short yPos_, const TileType &tt );

	short xPos;
	short yPos;

	TileType type;
};

inline std::istream& operator>>( std::istream &is, TilePosition &pos )
{
	int type = 0;
	is >> pos.xPos >> pos.yPos >> type;
	pos.type = static_cast< TileType > ( type );

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
		case TileType::Regular:
			os << "Regular";
			break;
		case TileType::Hard:
			os << "Hard";
			break;
		case TileType::Unbreakable:
			os << "Unbreakable";
			break;
		case TileType::Explosive:
			os << "Explosive";
			break;
		default:
			os << "Unknown";
			break;

	}

	os << std::endl;

	return os;
}
