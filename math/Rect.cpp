#include "Rect.h"

#include "Vector2f.h"

#include <cmath>

double Rect::FindDistanceBetweenTiles( const Rect &other ) const
{
	Vector2f otherCenter( other.x + ( other.w / 2.0 ) , other.y + ( other.h / 2.0 ) );

	Vector2f thisCenter( x + ( w / 2.0 ) , y + ( h / 2.0 ) );

	Vector2f dist( otherCenter - thisCenter );

	return sqrt( ( dist.x * dist.x ) + ( dist.y * dist.y ) );
}
