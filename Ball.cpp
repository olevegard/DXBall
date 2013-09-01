#include "Ball.h"
#include <cmath>
#include <iostream>

Ball::Ball()
	:	speed( 0.0705f )
	,	dirX( -0.83205f )
	,	dirY(-0.5547f )
	,	paddleHitInPrevFrame( false )
	,	debugMode( false )
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
	speed = 0.5f;

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

bool Ball::BoundCheck( const SDL_Rect &boundsRect )
{

	double left  = static_cast< double > ( boundsRect.x );
	double right = static_cast< double > ( boundsRect.x ) + boundsRect.w;
	double top   = static_cast< double > ( boundsRect.y );

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
	double bottom = static_cast< double > ( boundsRect.y + boundsRect.h );

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
	if ( CheckTileIntersection( paddleRect, rect ) )
	{
		if ( !paddleHitInPrevFrame )
		{
			paddleHitInPrevFrame = true;
			HandlePaddleHit( paddleRect );
			return true;
		} else
		{
			return false;
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

	// Returns on which ratio the ball hit. 1.0f is right edge, -1.0f is left edge and 0.0f is middle
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

	if ( debugMode )
	{
		std::cout << "===============================================================================\n";
		std::cout << "Colliding in current frame\n";
	}

	if ( debugMode ) 
		std::cout << "Checking sphere intersection....\n";

	if ( !CheckTileSphereIntersection( tileRect, rect  ) )
	{
		if ( debugMode ) 
			std::cout << "No circle collision, skipping....\n";
		return false;
	}

	// If the ball collided with the same rect in the previous frame too,
	// This means the ball needs an extra frame to get fully out of the tile
	if ( /*lastTileHit == tileID && */ CheckTileIntersection( tileRect, oldRect ) )
	{
		if ( debugMode )
		{
			std::cout << "\tCollided in the prev frame too...\n";
			PrintPosition( tileRect, "Tile" );
			PrintPosition( rect,     "Ball" );
		}
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

	double oldLeft   = oldRect.x;
	double oldTop    = oldRect.y;
	double oldRight  = oldLeft + rect.w;
	double oldBottom = oldTop  + rect.h;

	if ( debugMode )
	{
		PrintPosition( tileRect,"Tile");
		PrintPosition( rect    , "Ball cur ");
		PrintPosition( oldRect , "Ball old ");

		std::cout << "\tBall direction       : " << dirX << " , " << dirY << std::endl << std::endl;
	}

	if ( oldTop > tileBottom )
	{
		if ( debugMode )
			std::cout << "\tColided with bottom side of tile\n";

		if ( dirY < 0.0f )
			dirY *= -1.0f;
	}

	else if ( oldBottom < tileTop )
	{
		if ( debugMode )
			std::cout << "\tColided with top side of tile\n";
		
		if ( dirY > 0.0f )
			dirY *= -1.0f;
	}
	else if ( oldRight < tileLeft )
	{
		if ( debugMode )
			std::cout << "\tColided with left side of tile\n";

		if ( dirX > 0.0f )
			dirX *= -1.0f;
	}
	else if ( oldLeft > tileRight )
	{
		if ( debugMode )
			std::cout << "\tColided with right side of tile\n";

		if ( dirX < 0.0f )
			dirX *= -1.0f;
	} else
	{
		// At this point, we know there was a collision in the previous frame.
		std::cout << "\tCould not determine collision edge\n";
		PrintPosition( tileRect,"Tile");
		PrintPosition( rect    , "Ball cur ");
		PrintPosition( oldRect , "Ball old ");

		std::cout << "\tBall direction       : " << dirX << " , " << dirY << std::endl << std::endl;

		std::cin.ignore();
		//dirX *= -1.0f; dirY *= -1.0f;
	}

	if ( debugMode )
	{
		std::cout << "\tNew ball direction   : " << dirX << " , " << dirY << std::endl << std::endl;
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

	PrintPosition( tileRect,"Tile");
	PrintPosition( rect    , "Ball cur ");
	PrintPosition( oldRect , "Ball old ");

	if ( dirY < 0.0f )
	{
		std::cout << "\tChecking collision bottom : \n\n";
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
		std::cout << "\tChecking collision top : \n\n";
		double distTop = tileTop - oldBottom;

		double intersect = distTop / dirY;
		double intersectPosLeft  = oldLeft + dirX * intersect;
		double intersectPosRight = intersectPosLeft + rect.w;

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
		std::cout << "\tChecking collision left : \n\n";
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
		std::cout << "\tChecking collision left : \n\n";
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
bool Ball::CheckTileSphereIntersection( const Rect &tile, const Rect &ball ) const
{
	double ballRadius  = ball.w / 2.0;
	double ballCenterX = ball.x + ballRadius;
	double ballCenterY = ball.y + ballRadius;

	double tileHalfWidth  = ( tile.w / 2.0 );
	double tileHalfHeight = ( tile.h / 2.0 );
	double tileCenterX = tile.x + tileHalfWidth;
	double tileCenterY = tile.y + tileHalfHeight;
	
	double distX = ballCenterX - tileCenterX;
	double distY = ballCenterY - tileCenterY;

	// If distance from center of sphere to center of rect is larger than 
	// the sum of the raidus of the ball and the distance from the center of the rect to the edge
	if ( distX > ( tileHalfWidth + ballRadius ) )
	{
		if ( debugMode )
		{
			std::cout << "\tdistx < ( tilehalfwidth + ballradius )" << std::endl;
			std::cout << "\t" << distX << " < " << tileHalfWidth << " + " << ballRadius << std::endl;
		}
		return false;
	}

	if ( distY > ( tileHalfHeight + ballRadius ) )
	{
		std::cout << "\tdistY < ( tilehalfHeight + ballradius )" << std::endl;
		std::cout << "\t" << distX << " < " << tileHalfHeight << " + " << ballRadius << std::endl;
		return false;
	}

	// if the distance from the center of the sphere to the center of the rect is smaller or equal to
	// the the distance from the center of the rect to the edge
	if ( distX <= tileHalfWidth )
	{
		if ( debugMode )
		{
			std::cout << "\tdistX < tilehalfWidth" << std::endl;
			std::cout << "\t" << distX << " <= " << tileHalfWidth << std::endl;
		}
		return true;
	}

	if ( distY <= tileHalfHeight )
	{
		if ( debugMode )
		{
			std::cout << "\tdistY < tileHalfHeight" << std::endl;
			std::cout << "\n\t" << distX << " <= " << tileHalfHeight << std::endl;
		}
		return true;
	}

	// Get the distance from center of the sphere, to the edge of the rect squared.
	double cornerDistance = ( distX - tileHalfWidth ) * ( distX - tileHalfWidth );
	cornerDistance += ( distY - tileHalfHeight ) * ( distY - tileHalfHeight );

	return cornerDistance <= ( ballRadius * ballRadius );
}
void Ball::PrintPosition( const Rect &pos, const std::string &tileName ) const
{
	std::cout << "\t" << tileName << " position tl : " << pos.x              << " , " << pos.y             << std::endl;
	std::cout << "\t" << tileName << " position br : " << ( pos.x + pos.w )  << " , " << ( pos.y + pos.h ) << std::endl << std::endl;
}
