#include "enums/TileType.h"

#include <iomanip>
#include <iostream>
#include <array>

#include <SDL2/SDL.h>

#include "tools/SDLColorHelper.h"

struct TileColor
{
	TileType type;
	std::array< int32_t, 5 > r;
	std::array< int32_t, 5 > g;
	std::array< int32_t, 5 > b;
	std::array< int32_t, 5 > a;
	std::array< SDL_Color, 5 > colors;
	TileColor()
		:	r({ 0, 0, 0, 0, 0 })
		,	g({ 0, 0, 0, 0, 0 })
		,	b({ 0, 0, 0, 0, 0 })
		,	a({ 0, 0, 0, 0, 0 })
#ifdef linux //  The line below causes the error C1001 : An internal error has occured in the compiler
		,	colors({ { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 },  { 0, 0, 0, 0 } })
#endif
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
	bool IsMultiColored() const
	{
		return isMultiColored;
	}
	void SetIsMultiColored( bool multiColored )
	{
		isMultiColored = multiColored;
	}
	private:
		bool isMultiColored;
};

inline std::istream& operator>>( std::istream &is, TileColor &tileColor )
{
	int32_t tileType;
	is >> tileType;
	tileColor.type = static_cast< TileType > ( tileType );

	if ( tileColor.type != TileType::Hard )
	{
		is >> tileColor.colors[0];
		tileColor.SetIsMultiColored( false );
	}
	else
	{
		for ( int i = 0 ; i < 5 ; ++i )
		{
			is >> tileColor.colors[i];
		}
		tileColor.SetIsMultiColored( true );
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
