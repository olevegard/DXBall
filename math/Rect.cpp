#include "Rect.h"

#include "Vector2f.h"

#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

#include <SDL2/SDL.h>

bool Rect::CheckTileIntersection( const Rect &other ) const
{
	double thisLeft =   x;
	double thisTop =    y;
	double thisRight =  x + w;
	double thisBottom = y + h;

	double otherLeft =   other.x;
	double otherTop =    other.y;
	double otherRight =  other.x + other.w;
	double otherBottom = other.y + other.h;

	// Intersection test
	return !(
			otherTop    > thisBottom
			|| otherLeft   > thisRight
			|| otherRight  < thisLeft
			|| otherBottom < thisTop
		);
}


bool Rect::CheckTileIntersection( const std::vector< Rect > &rectVec, const Rect &explosion )
{
	auto p = [ explosion ]( const Rect &rect )
	{
		return ( explosion.CheckTileIntersection( rect ) );
	};

	return (  std::any_of( rectVec.begin(), rectVec.end(), p ) );
}
double Rect::FindDistanceBetweenTiles( const Rect &other ) const
{
	Vector2f otherCenter( other.x + ( other.w / 2.0 ) , other.y + ( other.h / 2.0 ) );

	Vector2f thisCenter( x + ( w / 2.0 ) , y + ( h / 2.0 ) );

	Vector2f dist( otherCenter - thisCenter );

	return sqrt( ( dist.x * dist.x ) + ( dist.y * dist.y ) );
}

void Rect::CombineRects( const Rect &other )
{
	double thisLeft =   x;
	double thisTop =    y;
	double thisRight =  x + w;
	double thisBottom = y + h;

	double otherLeft =   other.x;
	double otherTop =    other.y;
	double otherRight =  other.x + other.w;
	double otherBottom = other.y + other.h;

	// Other's left edge is farther to the left than this' left edge
	if ( otherLeft < thisLeft )
	{
		double diff = thisLeft - otherLeft;
		x = otherLeft;
		w += diff;
	}

	// Other's right edge is farther to the right than this' right edge
	if ( otherRight > thisRight )
		w = ( otherRight - x );


	// Other's top edge is farther to the top than this' top edge
	if ( otherTop < thisTop )
	{
		double diff = thisTop - otherTop;
		y = otherTop;
		h += diff;
	}

	// Other's bottom edge is farther to the bottom than this' bottom edge
	if ( otherBottom > thisBottom )
		h = ( otherBottom - y );
}
void Rect::DoubleRectSizes( )
{
	double newX = x - w;
	w += w * 2.0;
	if ( newX < 0.0 )
	{
		// Absolute value of how far outside the screen the rect would be.
		newX = fabs( newX );

		// Reduce width with said value
		w -= newX;

		x = 0;
	} else
	{
		x = newX;
	}

	double newY = y - h;
	h += h * 2.0;
	if ( newY < 0.0 )
	{
		// Absolute value of how far outside the screen the rect would be.
		newY = fabs( newY );

		// Reduce width with said value
		h -= newY;

		y = 0;
	} else
	{
		y = newY;
	}

}
SDL_Rect Rect::ToSDLRect(  ) const
{
	SDL_Rect ret;
	ret.x = static_cast< int > ( x );
	ret.y = static_cast< int > ( y );
	ret.w = static_cast< int > ( w );
	ret.h = static_cast< int > ( h );
	return ret;
}
