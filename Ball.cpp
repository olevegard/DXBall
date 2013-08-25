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

	dirX = 0.83205f;
	dirY =  -0.87f;

	rect.x = 150;
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

	std::cout << "Delta movement : " <<  deltaMovement << std::endl;
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
}
void  Ball::CalculateNewBallDirection( float hitPosition )
{
	dirX = hitPosition;//* -1.0f;
	dirY = fabsf( hitPosition ) - 1.6f;

	dirY = dirY < -0.8f ? -0.8f : dirY;

	std::cout << "Hit     : " << hitPosition << std::endl;
	std::cout << "New dir : " << dirX << " , " << dirY << std::endl;

	NormalizeDirection();
}
bool Ball::TileCheck( const SDL_Rect &tileRect, unsigned int tileID )
{
	// Intersection test
	if ( !CheckTileIntersection( tileRect, rect ) )
		return false;
	std::cout << "===============================================================================\n";
	std::cout << "Colliding in current frame\n";

	// If the ball collided with the same rect in the previous frame too,
	// This means the ball needs an extra frame to get fully out of the tile
	if ( lastTileHit == tileID && CheckTileIntersection( tileRect, oldRect ) )
	{
		std::cout << "\tCollided in the prev frame too...\n";
		PrintPosition( tileRect, "Tile" );
		PrintPosition( rect,     "Ball" );
		return false;
	}

	lastTileHit = tileID;
	HandleTileIntersection( tileRect );
	//HandleTileIntersection2( tileRect );

	return true;
}
void Ball::HandleTileIntersection( const SDL_Rect &tileRect )
{
	// Check Which Face Collided
	// 0,0 = upper left
	short tileLeft =   tileRect.x;
	short tileTop =    tileRect.y;
	short tileRight =  tileRect.x + tileRect.w;
	short tileBottom = tileRect.y + tileRect.h;

	short ballLeft =   rect.x;
	short ballTop =    rect.y;
	short ballRight =  rect.x + rect.w;
	short ballBottom = rect.y + rect.h;

	short oldLeft   = oldRect.x;
	short oldTop    = oldRect.y;
	short oldRight  = oldLeft + rect.w;
	short oldBottom = oldTop  + rect.h;

	PrintPosition( tileRect,"Tile");
	PrintPosition( rect    , "Ball cur ");
	PrintPosition( oldRect , "Ball old ");

	std::cout << "\tBall direction   : " << dirX << " , " << dirY << std::endl << std::endl;

	if ( oldTop > tileBottom )
	{
		// Colliding with underside of tile...
		dirY *= -1.0f;

		short dist = tileBottom - ballTop;

		std::cout << "\tBottom collision : " << dist << "\n";

		if ( ballLeft < tileLeft || ballRight > tileRight )
		{
			std::cout << "\tCorner collision\n";
		}
	}

	else if ( oldBottom < tileTop )
	{
		// Colliding with top side of tile...

		std::cout << "\tTop collision\n";
		if ( ballLeft < tileLeft || ballRight > tileRight )
		{
			std::cout << "\tCorner collision\n";

			float ratioVertical = static_cast< float > ( ballBottom - tileTop );
			float ratioHorizontal = 0.0f;
			if ( ballLeft < tileLeft )
			{
				 ratioHorizontal = static_cast< float > ( ballRight - tileLeft );
				std::cout << "\t\tLeft  : " << ratioHorizontal  << " ratio : " << ratioHorizontal  / static_cast< float > ( rect.w ) << std::endl;
			} else if ( ballRight > tileRight )
			{
				ratioHorizontal = static_cast< float > ( tileRight - ballLeft );
				std::cout << "\t\tRight : " << ratioHorizontal << " ratio : " << ratioHorizontal / static_cast< float > ( rect.w ) << std::endl;
			}

			std::cout << "\t\tTop : " << ratioVertical << " ratio : " << static_cast< float > ( ratioVertical / rect.h ) << std::endl;
			float length = sqrt( ratioHorizontal * ratioHorizontal + ratioVertical * ratioVertical );
			//std::cout << "\t\tSuggest dir change : " << ratioHorizontal / length << " , " << ratioVertical / length << std::endl;
			dirX *=  ( ( ratioHorizontal / length ) * -1.0f );
			dirY *=  ( ( ratioVertical   / length ) * -1.0f );
			std::cout << "\t\tSuggest dir change : " << dirX << " , " << dirY << std::endl;
			//std::cin.ignore();
		} else 
		{
			dirY *= -1.0f;
		}

	}
	else if ( oldRight < tileLeft )
	{
		// Colliding with left side of tile...
		dirX *= -1.0f;

		short dist = tileLeft - ballRight;

		std::cout << "\tLeft collision : " << dist << "\n";
	}
	else if ( oldLeft > tileRight )
	{
		// Colliding with right side of tile...
		dirX *= -1.0f;

		short dist = tileTop - ballLeft;

		std::cout << "\tRight collision : " << dist << "\n";
	}
	else
	{
		std::cout << "\tCould not determine collision edge\n";
		std::cin.ignore();
		dirX *= -1.0f;
		dirY *= -1.0f;
	}
}
void Ball::HandleTileIntersection2( const SDL_Rect &tileRect )
{
	bool stop = true;

	short tileLeft =   tileRect.x;
	short tileTop =    tileRect.y;
	short tileRight =  tileRect.x + tileRect.w;
	short tileBottom = tileRect.y + tileRect.h;

	short oldLeft   = oldRect.x;
	short oldTop    = oldRect.y;
	short oldRight  = oldLeft + rect.w;
	short oldBottom = oldTop  + rect.h;

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
		}
	}
}

bool Ball::CheckTileIntersection( const SDL_Rect &tile, const SDL_Rect &ball ) const
{
	short tileLeft =   tile.x;
	short tileTop =    tile.y;
	short tileRight =  tile.x + tile.w;
	short tileBottom = tile.y + tile.h;

	short ballLeft =   ball.x;
	short ballTop =    ball.y;
	short ballRight =  ball.x + ball.w;
	short ballBottom = ball.y + ball.h;

	// Intersection test
	return !(
			   ballTop    > tileBottom
			|| ballLeft   > tileRight
			|| ballRight  < tileLeft
			|| ballBottom < tileTop
	);
}
void Ball::PrintPosition( const SDL_Rect &pos,	const std::string &tileName )
{
	std::cout << "\t" << tileName << " position tl : " << pos.x              << " , " << pos.y             << std::endl;
	std::cout << "\t" << tileName << " position br : " << ( pos.x + pos.w )  << " , " << ( pos.y + pos.h ) << std::endl << std::endl;

}


