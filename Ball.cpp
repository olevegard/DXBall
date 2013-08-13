#include "Ball.h"

#include <iostream>

	Ball::Ball()
		:	speed( 0.0705f )
		,	dirX( -0.83205f )
		,	dirY(-0.5547f )
	{
		rect.w = 20;
		rect.h = 20;
		Reset();
	}

	Ball::~Ball()
	{
	}

	void Ball::Reset()
	{
		//speed = 0.7f;
		speed = 0.6f;
		dirX = -0.83205f;
		dirY =  -0.87f;
		rect.x = 400;
		rect.y = 110;
		NormalizeDirection();
	}

	void Ball::NormalizeDirection()
	{
		double length = sqrt( ( dirX * dirX ) + ( dirY * dirY ) );
		dirX /= length;
		dirY /= length;
	}

	void Ball::Update( double tick )
	{
		int deltaMovement = static_cast<int>( tick * speed + 0.5f );
		rect.x += static_cast<int> ( deltaMovement * dirX );
		rect.y += static_cast<int> ( deltaMovement * dirY );
	}

	bool Ball::BoundCheck( const SDL_Rect &boundsRect )
	{
		short left = boundsRect.x;
		short right = boundsRect.x + boundsRect.w;
		short top = boundsRect.y;

		if ( rect.x < left )
		{
			rect.x = left;
			dirX *= -1.0f;
			return true;
		}

		if ( ( rect.x + rect.w ) > right )
		{
			rect.x = ( right - rect.w );
			dirX *= -1.0f;
			return true;
		}

		if ( rect.y < top )
		{
			rect.y = top;
			dirY *= -1.0f;
			return true;
		}

		return false;
	}

	bool Ball::DeathCheck( const SDL_Rect &boundsRect )
	{
		int bottom = boundsRect.y + boundsRect.h;

		if (  ( rect.y + rect.h ) > bottom  )
		{
			Reset();
			dirY *= -1.0f;
			return true;
		}

		return false;
	}

	bool Ball::PaddleCheck( const SDL_Rect &paddleRect )
	{
		int left = paddleRect.x;
		int right = paddleRect.x + paddleRect.w;
		int top = paddleRect.y;

		if ( ( left < ( rect.x + rect.w ) ) && ( right > rect.x ) && ( top < ( rect.y + rect.h  )  ) )

		{
			// Make ball go left of right depending where on the paddle it hits
			int ballMidpoint = rect.x + ( rect.w / 2 );
			int paddleMidpoint = paddleRect.x + ( paddleRect.w / 2 );
			dirX = static_cast<float> ( ballMidpoint - paddleMidpoint ) / 70.0f;
			dirY = -1.0f;
		
			NormalizeDirection();

			speed *= 1.05f;

			return true;
		}

		return false;
	}

	bool Ball::TileCheck( const SDL_Rect &tileRect )
	{
		// 0,0 = upper left
		short tileLeft =   tileRect.x;
		short tileTop =    tileRect.y;
		short tileRight =  tileRect.x + tileRect.w;
		short tileBottom = tileRect.y + tileRect.h;

		short ballLeft =   rect.x;
		short ballTop =    rect.y;
		short ballRight =  rect.x + rect.w;
		short ballBottom = rect.y + rect.h;

		if (
			ballTop > tileBottom
			|| ballLeft > tileRight
			|| ballTop > tileBottom
			|| ballRight < tileLeft
			|| ballBottom < tileTop
		)
		{
			return false;
		}

		short oldLeft   = rect.x + (-speed * dirX  * 5.0f);
		short oldTop    = rect.y + (-speed * dirY  * 5.0f);
		short oldRight  = oldLeft + rect.w;
		short oldBottom = oldTop  + rect.h;

		std::cout << "Collision\n";


		if ( oldTop > tileBottom )
		{
			std::cout << "Bottom collision\n";
			dirY *= -1.0f;
			rect.y = oldTop;

		} else if ( oldBottom < tileTop )
		{
			std::cout << "Top collision\n";
			dirY *= -1.0f;
			rect.y = oldBottom - rect.h;
		}
		else if ( oldRight < tileLeft )
		{
			std::cout << "Left collision\n";
			dirX *= -1.0f;
			rect.x = oldLeft - rect.w;
		} else if ( oldLeft > tileRight )
		{
			std::cout << "Right collision\n";
			dirX *= -1.0f;
			rect.x = oldLeft;
		}
		
		std::cout << "Edges"
			<< "\n\tLeft   : " << ballLeft
			<< "\n\tRight  : " << ballRight
			<< "\n\tBottom : " << ballBottom
			<< "\n\tTop    : " << ballTop
		<< std::endl;

		std::cout << "Tile edges"
			<< "\n\tLeft   : " << tileLeft
			<< "\n\tRight  : " << tileRight
			<< "\n\tBottom : " << tileBottom
			<< "\n\tTop    : " << tileTop
		<< std::endl;

		std::cout << "Old edges"
			<< "\n\tLeft   : " << oldLeft
			<< "\n\tRight  : " << oldRight
			<< "\n\tBottom : " << oldBottom
			<< "\n\tTop    : " << oldTop
		<< std::endl;

		//std::cin.ignore();
		return true;
/*
		if (
			(  ( rect.x + rect.w ) <= left   )
			|| ( rect.x >= right)
			//|| ( ( rect.y - rect.h ) >= bottom )
			|| ( rect.y >= bottom )
			//|| ( rect.y  <= top )
			|| ( ( rect.y + rect.h )  <= top )
		   )
		{
			return false;
		} else
			std::cout << "Intersect\n";

		SDL_Rect oldPos;
		oldPos.x = rect.x -  (-speed * dirX  * 5.0f);
		oldPos.y = rect.y -  (-speed * dirY  * 5.0f);

		//std::cout << "Current pos : " << rect.x <<   " , " << rect.y   << std::endl;
		//std::cout << "Prev pos    : " << oldPos.x << " , " << oldPos.y << std::endl;
		std::cout << "Prev edges \n\tLeft " << rect.x   << "\n\tRight : " << ( rect.x + rect.w )   << "\n\tBottom : " << ( rect.y - rect.h )   << "\n\tTop : " << rect.y   << std::endl;
		std::cout << "Prev edges \n\tLeft " << oldPos.x << "\n\tRight : " << ( oldPos.x + rect.w ) << "\n\tBottom : " << ( oldPos.y - rect.h ) << "\n\tTop : " << oldPos.y << std::endl;
		std::cout << "Tile edges \n\tLeft " << left << "\n\tRight : " << right << "\n\tBottom : " << bottom << "\n\tTop : " << top << std::endl;

		//std::cout << "( oldPos.y - rect.h  ) : " << ( oldPos.y - rect.h  ) << std::endl;
		std::cout << "oldPos.y : " << oldPos.y << " size : " << rect.h << std::endl;
		std::cout << "newPos.y : " << rect.y   << " size : " << rect.h << std::endl;
		std::cout << "bottom   : " << bottom   << std::endl;

		if ( ( oldPos.y - rect.h  ) > bottom )

		{
			std::cout << "Was outside\n";
			if ( ( rect.y - rect.h ) < bottom  )
			{
				std::cout << "Bottom hit\n";
				dirY *= -1.0f;
				rect.y = oldPos.y;
			}
		}

		else if ( oldPos.y < top && rect.y > top )
		{
			std::cout << "Top hit\n";
			dirY *= -1.0f;
			rect.y = oldPos.y;
		}

		std::cin.ignore();

		return false;
		*/
	}


