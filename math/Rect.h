#pragma once

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
	
	void Set( const SDL_Rect &r )
	{
		this->x = r.x;
		this->y = r.y;
		this->w = r.w;
		this->h = r.h;
	}

	SDL_Rect GetAsSDL_Rect() const
	{
		SDL_Rect r;
		r.x = static_cast< short > ( this->x );
		r.y = static_cast< short > ( this->y );
		r.w = static_cast< unsigned short > ( this->w );
		r.h = static_cast< unsigned short > ( this->h );
		return r;
	}

		double x;
		double y;

		double w;
		double h;
	
	
};

