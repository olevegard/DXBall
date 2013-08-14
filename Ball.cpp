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
			float hitPosition = CalculatePaddleHitPosition( paddleRect );
	
			// if -1.0f -> [  1.0f,  0,0f]
			// if -0.5f -> [  0.5f, -0,5f]
			// if  0.0f -> [  0.0f, -1,0f]
			// if  0.5f -> [ -0.5f, -0,5f]
			// if  1.0f -> [ -1.0f,  0,0f]

			//dirX = static_cast<float> ( ballMidpoint - paddleMidpoint ) / 70.0f;
			dirX = hitPosition;//* -1.0f;
			dirY = fabs( hitPosition ) - 1.6f;
			std::cout << "Hit     : " << hitPosition << std::endl;
			std::cout << "New dir : " << dirX << " , " << dirY << std::endl;
			//std::cin.ignore();
		
			NormalizeDirection();

			speed *= 1.0005f;

			return true;
		}

		return false;
	}

	float Ball::CalculatePaddleHitPosition( const SDL_Rect &paddleRect )
	{

		int ballMidpoint = rect.x + ( rect.w / 2 );
		int paddleMidpoint = paddleRect.x + ( paddleRect.w / 2 );

		return ( static_cast< float >( ballMidpoint - paddleMidpoint ) / ( paddleRect.w + 20 ) ) * 2.0f;
		//return ( static_cast< float >( ballMidpoint - paddleMidpoint ) / ( paddleRect.w + 50 ) ) * 2.0f;
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

		// Intersection test
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

		// Check Which Face Collided
		short oldLeft   = rect.x + static_cast<short>(-speed * dirX  * 5.0f);
		short oldTop    = rect.y + static_cast<short>(-speed * dirY  * 5.0f);
		short oldRight  = oldLeft + rect.w;
		short oldBottom = oldTop  + rect.h;

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
/*		
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
*/
		//std::cin.ignore();
		return true;
	}


