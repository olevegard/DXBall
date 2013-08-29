#include "Ball.h"
#include <cmath>
#include <iostream>

Ball::Ball()
	:	speed( 0.0705f )
	,	dirX( -0.83205f )
	,	dirY(-0.5547f )
	,	paddleHitInPrevFrame( false )
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

	rect.x = 50;
	rect.y = 50;

	paddleHitInPrevFrame = false;

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

	rect.x += tick * speed * dirX;
	rect.y += tick * speed * dirY;
}

bool Ball::BoundCheck( const Rect &boundsRect )
{

	double left = boundsRect.x;
	double right = boundsRect.x + boundsRect.w;
	double top = boundsRect.y;

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

bool Ball::DeathCheck( const Rect &boundsRect )
{
	double bottom = boundsRect.y + boundsRect.h;

	if (  ( rect.y + rect.h ) > bottom  )
	{
		Reset();
		dirY *= -1.0f;
		return true;
	}

	return false;
}

bool Ball::PaddleCheck( const Rect &paddleRect )
{
	double left = paddleRect.x;
	double right = paddleRect.x + paddleRect.w;
	double top = paddleRect.y;

	if ( ( left < ( rect.x + rect.w ) ) && ( right > rect.x ) && ( top < ( rect.y + rect.h  )  ) )
	{
		if ( !paddleHitInPrevFrame )
		{
			paddleHitInPrevFrame = false;
			HandlePaddleHit( paddleRect );
			return true;
		}
	}

	paddleHitInPrevFrame = false;
	return false;
}

void Ball::HandlePaddleHit( const Rect &paddleRect )
{
	double hitPosition = CalculatePaddleHitPosition( paddleRect );

	CalculateNewBallDirection( hitPosition );
	speed *= 1.0005f;
}
double Ball::CalculatePaddleHitPosition( const Rect &paddleRect ) const
{
	double ballMidpoint = rect.x + ( rect.w / 2.0 );
	double paddleMidpoint = paddleRect.x + ( paddleRect.w / 2.0 );

	std::cout << "Ball midpoint   : " << ballMidpoint << std::endl;
	std::cout << "Paddle size     : " << paddleRect.w << " , " << paddleRect.h << std::endl;
	std::cout << "Paddle midpoint : " << paddleMidpoint << std::endl;

	return ( ballMidpoint - paddleMidpoint ) / ( ( paddleRect.w / 2.0 ) + ( rect.w / 2.0 ));
}
void  Ball::CalculateNewBallDirection( double hitPosition )
{
	dirX = hitPosition;
	dirY *= -1.0f;

	NormalizeDirection();
}
bool Ball::TileCheck( const Rect &tileRect, unsigned int tileID )
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
void Ball::HandleTileIntersection( const Rect &tileRect )
{
	// Check Which Face Collided
	// 0,0 = upper left
	double tileLeft =   tileRect.x;
	double tileTop =    tileRect.y;
	double tileRight =  tileRect.x + tileRect.w;
	double tileBottom = tileRect.y + tileRect.h;

	double ballLeft =   rect.x;
	double ballTop =    rect.y;
	double ballRight =  rect.x + rect.w;
	double ballBottom = rect.y + rect.h;

	double oldLeft   = oldRect.x;
	double oldTop    = oldRect.y;
	double oldRight  = oldLeft + rect.w;
	double oldBottom = oldTop  + rect.h;

	PrintPosition( tileRect,"Tile");
	PrintPosition( rect    , "Ball cur ");
	PrintPosition( oldRect , "Ball old ");

	std::cout << "\tBall direction   : " << dirX << " , " << dirY << std::endl << std::endl;

	if ( oldTop > tileBottom )
	{
		// Colliding with underside of tile...
		dirY *= -1.0f;

		double dist = tileBottom - ballTop;

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

			double ratioVertical = ballBottom - tileTop;
			double ratioHorizontal = 0.0f;
			if ( ballLeft < tileLeft )
			{
				 ratioHorizontal = ballRight - tileLeft;
				std::cout << "\t\tLeft  : " << ratioHorizontal  << " ratio : " << ratioHorizontal  / rect.w << std::endl;
			} else if ( ballRight > tileRight )
			{
				ratioHorizontal = tileRight - ballLeft;
				std::cout << "\t\tRight : " << ratioHorizontal << " ratio : " << ratioHorizontal / rect.w << std::endl;
			}

			std::cout << "\t\tTop : " << ratioVertical << " ratio : " << ratioVertical / rect.h << std::endl;
			double length = sqrt( ratioHorizontal * ratioHorizontal + ratioVertical * ratioVertical );
			std::cout << "\t\tSuggest dir change : " << ratioHorizontal / length << " , " << ratioVertical / length << std::endl;
			//dirX *=  ( ( ratioHorizontal / length ) * -1.0f );
			//dirY *=  ( ( ratioVertical   / length ) * -1.0f );
			std::cout << "\t\tSuggest dir change : " << dirX << " , " << dirY << std::endl;

			dirY *= -1.f;
		} else
		{
			dirY *= -1.0f;
		}
	}
	else if ( oldRight < tileLeft )
	{
		// Colliding with left side of tile...
		dirX *= -1.0f;

		double dist = tileLeft - ballRight;

		std::cout << "\tLeft collision : " << dist << "\n";
	}
	else if ( oldLeft > tileRight )
	{
		// Colliding with right side of tile...
		dirX *= -1.0f;

		double dist = tileTop - ballLeft;

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
void Ball::HandleTileIntersection2( const Rect &tileRect )
{
	bool stop = true;

	double tileLeft =   tileRect.x;
	double tileTop =    tileRect.y;
	double tileRight =  tileRect.x + tileRect.w;
	double tileBottom = tileRect.y + tileRect.h;

	double oldLeft   = oldRect.x;
	double oldTop    = oldRect.y;
	double oldRight  = oldLeft + rect.w;
	double oldBottom = oldTop  + rect.h;

	if ( dirY < 0.0f )
	{
		std::cout << "Checking collision bottom : \n";
		double distBottom = oldTop - tileBottom;

		double intersect = distBottom / dirY;
		double intersectPosLeft  = oldLeft + dirX * fabs( intersect );
		double intersectPosRight = intersectPosLeft + rect.w;

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
		double distTop = tileTop - oldBottom;

		double intersect = distTop / dirY;
		double intersectPosLeft  = oldLeft + dirX * intersect;
		double intersectPosRight = intersectPosLeft + rect.w;

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
		double distLeft = tileLeft - oldRight;

		double intersect = distLeft / dirX;
		double intersectPosTop     = oldTop + dirY * fabs( intersect );
		double intersectPosBottom  = intersectPosTop + rect.h;

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
		double distRight = oldLeft - tileRight;

		double intersect = distRight / dirX;
		double intersectPosTop     = oldTop + dirY * fabs( intersect );
		double intersectPosBottom  = intersectPosTop + rect.h;

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

bool Ball::CheckTileIntersection( const Rect &tile, const Rect &ball ) const
{
	double tileLeft =   tile.x;
	double tileTop =    tile.y;
	double tileRight =  tile.x + tile.w;
	double tileBottom = tile.y + tile.h;

	double ballLeft =   ball.x;
	double ballTop =    ball.y;
	double ballRight =  ball.x + ball.w;
	double ballBottom = ball.y + ball.h;

	// Intersection test
	return !(
			   ballTop    > tileBottom
			|| ballLeft   > tileRight
			|| ballRight  < tileLeft
			|| ballBottom < tileTop
	);
}
void Ball::PrintPosition( const Rect &pos, const std::string &tileName )
{
	std::cout << "\t" << tileName << " position tl : " << pos.x              << " , " << pos.y             << std::endl;
	std::cout << "\t" << tileName << " position br : " << ( pos.x + pos.w )  << " , " << ( pos.y + pos.h ) << std::endl << std::endl;
}
