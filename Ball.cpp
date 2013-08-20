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

	bool stop = true;

	
/*
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
		<< std::endl << std::endl;
*/
	std::cout << "\tTile position tl : " << tileLeft   << " , " << tileTop  << std::endl;
	std::cout << "\tTile position br : " << tileRight  << " , " << tileBottom << std::endl << std::endl;

	std::cout << "\tBall position tl : " << oldLeft     << " , " << oldTop   << std::endl;
	std::cout << "\tBall position br : " << oldRight    << " , " << oldBottom  << std::endl << std::endl;

	std::cout << "\tBall direction   : " << dirX << " , " << dirY << std::endl << std::endl;

	if ( dirY < 0.0f )
	{
		std::cout << "Checking collision bottom : \n";
		float distBottom = oldTop - tileBottom;

		float intersect = static_cast<float > ( distBottom / dirY );
		float intersectPosLeft  = oldLeft + static_cast<float > ( dirX * fabsf( intersect ) );
		float intersectPosRight = intersectPosLeft + rect.w;

		std::cout << "\tDist bottom      : " << distBottom << std::endl;
		std::cout << "\tIntersect        : " << intersect << std::endl;
		std::cout << "\tIntersect left   : " << intersectPosLeft << std::endl;
		std::cout << "\tIntersect right  : " << intersectPosRight << std::endl;

		std::cout << "\tif ( " << intersectPosLeft  << " > " << tileLeft  << " )" << std::endl;
		std::cout << "\tif ( " << intersectPosRight << " < " << tileRight << " )" << std::endl;

		if ( distBottom > 0.0f && intersectPosRight > tileLeft && intersectPosLeft < tileRight )
		{
			std::cout << "\t\tIntersected bottom" << std::endl;
			stop = false;
		}
		else
		{
			std::cout << "\t\tMissed bottom" << std::endl;
			//std::cin.ignore();
		}
	} else 
	{
		std::cout << "Checking collision top : \n";
		float distTop = tileTop - oldBottom;

		float intersect = static_cast<float > ( distTop / dirY );
		float intersectPosLeft  = oldLeft + static_cast<float > ( dirX * intersect  );
		float intersectPosRight = intersectPosLeft + rect.w;

		std::cout << "rect.w : " << rect.w << std::endl;

		std::cout << "\tDist bottom      : " << distTop << std::endl;
		std::cout << "\tIntersect        : " << intersect << std::endl;
		std::cout << "\tIntersect left   : " << intersectPosLeft << std::endl;
		std::cout << "\tIntersect right  : " << intersectPosRight << std::endl;

		std::cout << "\tif ( " << intersectPosLeft  << " > " << tileLeft  << " )" << std::endl;
		std::cout << "\tif ( " << intersectPosRight << " < " << tileRight << " )" << std::endl;
		if ( distTop > 0.0f &&  intersectPosRight > tileLeft && intersectPosLeft < tileRight )
		{
			std::cout << "\t\tIntersected top" << std::endl;
			stop = false;
		}
		else
			std::cout << "\t\tMissed top" << std::endl;

		//else if ( distBottom > 0.0f && intersectPosX > tileLeft && intersectPosX < tileRight )
		//std::cout << "\nIntersected top" << std::endl;
	}

	if ( dirX > 0.0f )
	{
		std::cout << "Checking collision left : \n";
		float distLeft = tileLeft - oldRight;

		float intersect = static_cast<float > ( distLeft / dirX );
		float intersectPosTop     = oldTop + static_cast<float > ( dirY * fabsf( intersect ) );
		float intersectPosBottom  = intersectPosTop + rect.h;

		std::cout << "\tDist lwf          : " << distLeft           << std::endl;
		std::cout << "\tIntersect         : " << intersect          << std::endl;
		std::cout << "\tIntersect top     : " << intersectPosTop    << std::endl;
		std::cout << "\tIntersect bottom  : " << intersectPosBottom << std::endl;

		std::cout << "\tif ( " << intersectPosTop    << " < " << tileBottom  << " )" << std::endl;
		std::cout << "\tif ( " << intersectPosBottom << " > " << tileTop     << " )" << std::endl;

		if ( distLeft > 0.0f && intersectPosTop < tileBottom && intersectPosBottom > tileTop )
		{
			std::cout << "\t\tIntersected left" << std::endl;
			stop = false;
		}
		else
		{
			std::cout << "\t\tMissed left" << std::endl;
			//std::cin.ignore();
		}
	} else if ( dirX < 0.0f )
	{
		std::cout << "Checking collision left : \n";
		float distRight = oldLeft - tileRight;

		float intersect = static_cast<float > ( distRight / dirX );
		float intersectPosTop     = oldTop + static_cast<float > ( dirY * fabsf( intersect ) );
		float intersectPosBottom  = intersectPosTop + rect.h;

		std::cout << "\tDist right        : " << distRight          << std::endl;
		std::cout << "\tIntersect         : " << intersect          << std::endl;
		std::cout << "\tIntersect top     : " << intersectPosTop    << std::endl;
		std::cout << "\tIntersect bottom  : " << intersectPosBottom << std::endl;

		std::cout << "\tif ( " << intersectPosTop    << " < " << tileBottom  << " )" << std::endl;
		std::cout << "\tif ( " << intersectPosBottom << " > " << tileTop     << " )" << std::endl;

		if ( distRight > 0.0f && intersectPosTop < tileBottom && intersectPosBottom > tileTop )
		{
			std::cout << "\t\tIntersected right" << std::endl;
			stop = false;
		}
		else
		{
			std::cout << "\t\tMissed right" << std::endl;
			//std::cin.ignore();
		}
	}

	if ( oldTop > tileBottom )
	{
		// Colliding with underside of tile...
		dirY *= -1.0f;

		short dist = tileBottom - ballTop;
		float f = dist / ( dirY );
		//f *= 1.05f;	

		std::cout << "Bottom collision : " << dist << "\n";
		std::cout << "f : " << f << std::endl;

		//rect.x += dirX * f;
		//rect.y += dirY * f;

	} else if ( oldBottom < tileTop )
	{
		// Colliding with top side of tile...
		dirY *= -1.0f;

		short dist = tileTop - ballBottom;
		float f = dist / ( dirY );
		//f *= 1.05f;	

		std::cout << "Top collision : " << dist << "\n";
		std::cout << "f : " << f << std::endl;

		//rect.x += dirX * f;
		//rect.y += dirY * f;
	}
	else if ( oldRight < tileLeft )
	{
		// Colliding with left side of tile...
		dirX *= -1.0f;

		short dist = tileLeft - ballRight;
		float f = dist / ( dirY );
		//f *= 1.05f;	

		std::cout << "Left collision : " << dist << "\n";
		std::cout << "f : " << f << std::endl;

		//rect.x += dirX * f;
		//rect.y += dirY * f;

	} else if ( oldLeft > tileRight )
	{
		// Colliding with right side of tile...
		dirX *= -1.0f;

		short dist = tileTop - ballBottom;
		float f = dist / ( dirY );
		//f *= 1.05f;	

		std::cout << "Right collision : " << dist << "\n";
		std::cout << "f : " << f << std::endl;

		//rect.x += dirX * f;
		//rect.y += dirY * f;
	} else 
	{
		std::cout << "Could not determine collision edge\n";


		dirX *= -1.0f;
		dirY *= -1.0f;
		rect.x += dirX * 2;
		rect.y += dirY * 2;

		stop = true;
	}
	if ( stop )
		std::cin.ignore();

	return true;
}
