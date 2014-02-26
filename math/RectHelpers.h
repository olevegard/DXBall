#pragma once

#include <vector>
#include <algorithm>

#include <ostream>
#include <iostream>
#include <string>

#include "Vector2f.h"

namespace RectHelpers
{
	void PrintPosition( const Rect &pos, const std::string &tilename );
	bool CheckTileIntersection( const std::vector< Rect > &rectVec, const Rect &explosion );
	bool CheckMouseIntersection( int x, int y, SDL_Rect rect );
	Vector2f GetCenter( const Rect &r );
	Vector2f CenterInRect( const Rect &mainRect, const Rect &toCenter  );

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

	inline Vector2f GetCenter( const Rect &r )
	{
		return Vector2f( r.x + ( r.w * 0.5 ), r.y + ( r.h * 0.5 ));
	}

	inline Vector2f CenterInRect( const Rect &mainRect, const Rect &toCenter  )
	{
		Vector2f center = GetCenter( mainRect );
		center.x -= toCenter.w * 0.5;
		center.y -= toCenter.h * 0.5;
		return center;
	}
}
inline std::ostream& operator<<(std::ostream& stream, const Rect &rect)
{
	stream << "Pos ( " << rect.x << " , " << rect.y << " ) Size ( " << rect.w << " , " << rect.h << ")";
	return stream;
}
