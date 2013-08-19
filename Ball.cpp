#include "Ball.h"
#include <cmath>
#include <iostream>

Ball::Ball()
	:	speed( 0.0705f )
	,	dirX( -0.83205f )
	,	dirY(-0.5547f )
{
	rect.w = 20;
	oldRect.w = 20;

	rect.h = 20;
	oldRect.h = 20;

	Reset();
}

Ball::~Ball()
{
}

void Ball::Reset()
{
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
	oldRect.x = rect.x;
	oldRect.y = rect.y;

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
		HandlePaddleHit( paddleRect );
		return true;
	}

	return false;
}

void Ball::HandlePaddleHit( const SDL_Rect &paddleRect )
{
	float hitPosition = CalculatePaddleHitPosition( paddleRect );

	CalculateNewBallDirection( hitPosition );
	speed *= 1.0005f;
}
float Ball::CalculatePaddleHitPosition( const SDL_Rect &paddleRect ) const
{

	int ballMidpoint = rect.x + ( rect.w / 2 );
	int paddleMidpoint = paddleRect.x + ( paddleRect.w / 2 );

	return ( static_cast< float >( ballMidpoint - paddleMidpoint ) / ( paddleRect.w + 20 ) ) * 2.0f;
	//return ( static_cast< float >( ballMidpoint - paddleMidpoint ) / ( paddleRect.w + 50 ) ) * 2.0f;
}
void  Ball::CalculateNewBallDirection( float hitPosition )
{
	dirX = hitPosition;//* -1.0f;
	dirY = fabsf( hitPosition ) - 1.6f;

	dirY = dirY < -0.8f ? -0.8f : dirY;

	std::cout << "Hit     : " << hitPosition << std::endl;
	std::cout << "New dir : " << dirX << " , " << dirY << std::endl;
	//std::cin.ignore();

	NormalizeDirection();

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
	//short oldLeft   = rect.x + static_cast<short>(-speed * dirX  * 5.0f);
	//short oldTop    = rect.y + static_cast<short>(-speed * dirY  * 5.0f);
	short oldLeft   = oldRect.x;
	short oldTop    = oldRect.y;
	short oldRight  = oldLeft + rect.w;
	short oldBottom = oldTop  + rect.h;

	if ( oldTop > tileBottom )
	{
		std::cout << "Bottom collision\n";
		dirY *= -1.0f;
		rect.y = tileBottom + 5;

	} else if ( oldBottom < tileTop )
	{
		std::cout << "Top collision\n";
		dirY *= -1.0f;
		rect.y = tileTop - 5;
		//rect.y = oldBottom - rect.h;
	}
	else if ( oldRight < tileLeft )
	{
		std::cout << "Left collision\n";
		dirX *= -1.0f;
		rect.x = tileLeft - 5;
		//rect.x = oldLeft - rect.w;
	} else if ( oldLeft > tileRight )
	{
		std::cout << "Right collision\n";
		dirX *= -1.0f;
		rect.x = tileRight + 5;
		//rect.x = oldLeft;
	} else 
	{
		std::cout << "Could not determine collision edge\n";

		std::cout << "Edges"
		<< "\n\tTop    : " << ballTop
		<< "\n\tLeft   : " << ballLeft   << "--------------------Right  : " << ballRight
		<< "\n\tBottom : " << ballBottom
		<< std::endl;

		std::cout << "Tile edges"
		<< "\n\tTop    : " << tileTop
		<< "\n\tLeft   : " << tileLeft   << "--------------------Right  : " << tileRight
		<< "\n\tBottom : " << tileBottom
		<< std::endl;

		std::cout << "Old edges"
		<< "\n\tTop    : " << oldTop
		<< "\n\tLeft   : " << oldLeft   << "--------------------Right  : " << oldRight
		<< "\n\tBottom : " << oldBottom
		<< std::endl;

		dirX *= -1.0f;
		dirY *= -1.0f;
		
		rect.x += dirX * 2;
		rect.y += dirY * 2;

		//std::cin.ignore();
	}
	return true;
}
