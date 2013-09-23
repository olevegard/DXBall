#pragma once

#include <vector>
#include <algorithm>

#include <ostream>
#include <iostream>

namespace RectHelpers
{
	void PrintPosition( const Rect &pos, const std::string &tilename );
	bool CheckTileIntersection( const std::vector< Rect > &rectVec, const Rect &explosion );
	bool CheckMouseIntersection( int x, int y, SDL_Rect rect );

	inline std::ostream& operator<<(std::ostream& stream, const Rect &rect)
	{
		stream << "Pos ( " << rect.x << " , " << rect.y << " ) Size ( " << rect.w << " , " << rect.h << ")";
		return stream;
	}

	inline void PrintPosition( const Rect &pos, const std::string &tileName )
	{
		std::cout << "\t" << tileName << " position tl : " << pos.x              << " , " << pos.y             << std::endl;
		std::cout << "\t" << tileName << " position br : " << ( pos.x + pos.w )  << " , " << ( pos.y + pos.h ) << std::endl << std::endl;
	}
	inline bool CheckTileIntersection( const std::vector< Rect > &rectVec, const Rect &explosion )
	{
		auto p = [ explosion ]( const Rect &rect )
		{
			return ( explosion.CheckTileIntersection( rect ) );
		};

		return (  std::any_of( rectVec.begin(), rectVec.end(), p ) );
	}

	inline bool CheckMouseIntersection( int x, int y, SDL_Rect rect )
	{
		if ( x < rect.x )
			return false;

		if ( x > ( rect.x + rect.w ) )
			return false;

		if ( y < rect.y )
			return false;

		if ( y > ( rect.y + rect.h ) )
			return false;

		return true;
	}
}
