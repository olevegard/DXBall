#pragma once

#include "GamePiece.h"
#include <math.h>
struct Ball : GamePiece
{
	Ball()
		:	speed( 0.7f )
		,	dirX( 0.83205 )
		,	dirY( 0.5547 )
		,	stop( false )
	{
		rect.w = 20;
		rect.h = 20;
		NormalizeDirection();
	}

	virtual ~Ball()
	{
	}

	void NormalizeDirection()
	{
		float length = sqrt( ( dirX * dirX ) + ( dirY * dirY ) );
		dirX /= length;
		dirY /= length;
		std::cout << "dir X " << dirX << std::endl;
		std::cout << "dir Y " << dirY << std::endl;

	}
	void update( float tick )
	{ 
		int deltaMovement = static_cast<int>( tick * speed + 0.5f );
		rect.x += static_cast<int> ( deltaMovement * dirX );
		rect.y += static_cast<int> ( deltaMovement * dirY );
	}

	bool BoundCheck( const SDL_Rect &boundsRect )
	{
		int left = boundsRect.x;
		int right = boundsRect.x + boundsRect.w;
		int top = boundsRect.y;
		int bottom = boundsRect.y + boundsRect.h;

		if ( false )
			std::cout
				<< "pos x  : " << rect.x << std::endl
				<< "pot x+w: " << rect.x + rect.w << std::endl
				<< "pos y  : " << rect.y << std::endl
				<< "pot y+h: " << rect.y + rect.h << std::endl
				<< "left   : " << left << std::endl
				<< "right  : " << right << std::endl
				<< "bottom : " << bottom << std::endl
				<< "top    : " << top << std::endl;

		if ( rect.x < left || ( rect.x + rect.w ) > right )
		{
			dirX *= -1.0f;
			return true;
		}

		if ( ( rect.y + rect.h ) > bottom || rect.y < top )
		{
			dirY *= -1.0f;
			return true;
		}

		return false;
	}

	bool PaddleCheck( const SDL_Rect &paddleRect )
	{
		int left = paddleRect.x;
		int right = paddleRect.x + paddleRect.w;
		int top = paddleRect.y;
		int bottom = paddleRect.y + paddleRect.h;

		if ( false )
			std::cout
				<< "pos x  : " << rect.x << std::endl
				<< "pot x+w: " << rect.x + rect.w << std::endl
				<< "pos y  : " << rect.y << std::endl
				<< "pot y+h: " << rect.y + rect.h << std::endl
				<< "left   : " << left << std::endl
				<< "right  : " << right << std::endl
				<< "bottom : " << bottom << std::endl
				<< "top    : " << top << std::endl;

		//if  ( ( rect.x > left  ) && ( ( rect.x + rect.w ) < right ) && ( ( rect.y + rect.h ) < bottom ) && ( rect.y > top ) )
		if ( ( left < ( rect.x + rect.w ) ) && ( right > rect.x ) && ( top < ( rect.y + rect.h  )  ) )
		{
			std::cout << "Hit\n";
			dirX *= -1.0f;
			dirY *= -1.0f;
			return true;
		} else
			std::cout << "Miss\n";

		if ( false )
		{
		}

		return false;
	}

	float speed;


	float dirX;
	float dirY;
	bool stop;
};
