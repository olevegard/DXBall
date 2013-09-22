#pragma once

struct SDL_Rect;
struct Rect
{
	Rect()
		:	x( 0.0 )
		,	y( 0.0 )
		,	w( 0.0 )
		,	h( 0.0 )
	{


	}
	Rect( double x_, double y_, double w_, double h_ )
		:	x( x_ )
		,	y( y_ )
		,	w( w_ )
		,	h( h_ )
	{

	}

	SDL_Rect ToSDLRect(  ) const;

	bool CheckTileIntersection( const Rect &other ) const;

	void DoubleRectSizes( );

	double FindDistanceBetweenTiles( const Rect &other ) const;

	void CombineRects( const Rect &other );

	double x;
	double y;
	double w;
	double h;
};

