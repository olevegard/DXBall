#include "enums/TileType.h"

#include <iomanip>
#include <iostream>

#include <SDL2/SDL.h>

struct TileColor
{
	TileType type;
	int32_t r[5];
	int32_t g[5];
	int32_t b[5];
	int32_t a[5];
	SDL_Color colors[5];
	TileColor()
		:	r{ 0, 0, 0, 0, 0 }
		,	g{ 0, 0, 0, 0, 0 }
		,	b{ 0, 0, 0, 0, 0 }
		,	a{ 0, 0, 0, 0, 0 }
		,	colors{{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }
	{

	}
	std::string GetTileTypeAsString() const
	{
		switch ( type )
		{
			case TileType::Regular:
				return "Regular";
			case TileType::Hard:
				return "Hard";
			case TileType::Explosive:
				return "Explosive";
			case TileType::Unbreakable:
				return "Unbreakable";
			default:
				return "Unknown";
		}
	}
};
inline std::istream& operator>>( std::istream &is, SDL_Color &color )
{
	int32_t r = 0;
	is >> r;
	color.r = static_cast<uint8_t> ( r );

	int32_t g = 0;
	is >> g;
	color.g = static_cast<uint8_t> ( g );

	int32_t b = 0;
	is >> b;
	color.b = static_cast<uint8_t> ( b );

	int32_t a = 0;
	is >> a;
	color.a = static_cast<uint8_t> ( a );

	return is;
}
inline std::ostream& operator<<( std::ostream &os, const SDL_Color &color )
{
	os
		<< static_cast < int32_t> ( color.r ) << ", "
		<< static_cast < int32_t> ( color.g ) << ", "
		<< static_cast < int32_t> ( color.b ) << ", "
		<< static_cast < int32_t> ( color.a );
	return os;
}
inline std::istream& operator>>( std::istream &is, TileColor &tileColor )
{
	int32_t tileType;
	is >> tileType;
	tileColor.type = static_cast< TileType > ( tileType );

	if ( tileColor.type != TileType::Hard )
	{
		is >> tileColor.colors[0];
	}
	else
	{
		for ( int i = 0 ; i < 5 ; ++i )
		{
			is >> tileColor.colors[i];
		}
	}
	return is;
}
inline std::ostream& operator<<( std::ostream &os, const TileColor &tileColor )
{
	if ( tileColor.type != TileType::Hard )
	{
		os << tileColor.GetTileTypeAsString() << " : " << tileColor.colors[0];
	}
	else
	{
		os << tileColor.GetTileTypeAsString() << " : ";

		for ( int i = 0 ; i < 5 ; ++i )
		{
			os << tileColor.colors[i];

			if ( i != 4 )
				os << " | ";
		}
	}

	return os;
}

