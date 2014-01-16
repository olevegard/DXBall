#include "Ball.h"
#include <cmath>
#include <iostream>

#include <SDL2/SDL.h>

#include "math/Math.h"
#include "math/RectHelpers.h"
#include "math/Vector2f.h"
#include "math/VectorHelpers.h"

#include "enums/Side.h"
#include "enums/Corner.h"

Ball::Ball( const SDL_Rect &windowSize, const Player &owner, int32_t ID   )
	:	dirX( -0.83205f )
	,	dirY(-0.5547f )
	,	ballOwner( owner )
{
	SetObjectID( ID );
	rect.w = 20;
	oldRect.w = 20;

	rect.h = 20;
	oldRect.h = 20;

	Reset( windowSize );
}
Ball::~Ball()
{
}
void Ball::Reset( const SDL_Rect &windowSize )
{
	SetSpeed(0.3f * Math::GenRandomNumber( 1 ));

	// X pos and dirX is the same for both local and remote player
	dirX = Math::GenRandomNumber( -1.0, 1.0 );
	rect.x = ( windowSize.w * 0.5 ) - ( rect.w * 0.5 );

	if ( ballOwner == Player::Local )
	{
		dirY = Math::GenRandomNumber( -0.9, -0.1 );
		rect.y = windowSize.h - 75;
	}
	else if ( ballOwner == Player::Remote )
	{
		dirY = Math::GenRandomNumber(  0.1, 0.9 );
		rect.y = 150;
	}

	NormalizeDirection();
}
void Ball::NormalizeDirection()
{
	double length = sqrt( ( dirX * dirX ) + ( dirY * dirY ) );
	dirX /= length;
	dirY /= length;
}
void Ball::ChangeBallDirection( const Side &side)
{
	switch ( side )
	{
		case Side::Top :
			dirY = ( dirY > 0.0f ) ? dirY * -1.0f : dirY;
			break;
		case Side::Right :
			dirX = ( dirX < 0.0f ) ? dirX * -1.0f : dirX;
			break;
		case Side::Bottom :
			dirY = ( dirY < 0.0f ) ? dirY * -1.0f : dirY;
			break;
		case Side::Left :
			dirX = ( dirX > 0.0f ) ? dirX * -1.0f : dirX;
			break;
		case Side::Unknown :
			std::cout << "Ball.cpp@" << __LINE__ << " Uknown collosion\n";
			dirX *= -1.0f;
			dirY *= -1.0f;
			break;
	}
}
void Ball::Update( double tick )
{
	oldRect.x = rect.x;
	oldRect.y = rect.y;

	rect.x += tick * GetSpeed() * dirX;
	rect.y += tick * GetSpeed() * dirY;
}
bool Ball::BoundCheck( const SDL_Rect &boundsRect )
{
	if ( CheckSideCollisions( boundsRect ) )
		return true;

	if ( CheckTopBottomCollision( boundsRect ) )
		return true;

	return false;
}
bool Ball::CheckSideCollisions( const SDL_Rect &boundsRect )
{
	double left   = static_cast< double > ( boundsRect.x );
	double right  = static_cast< double > ( boundsRect.x + boundsRect.w );

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

	return false;
}
bool Ball::CheckTopBottomCollision( const SDL_Rect &boundsRect )
{
	if ( ballOwner == Player::Local )
	{
		double top    = static_cast< double > ( boundsRect.y );
		if ( rect.y < top )
		{
			rect.y = top;
			dirY = ( dirY < 0.0f ) ? dirY * -1.0f : dirY;
			return true;
		}
	}
	else if ( ballOwner == Player::Remote )
	{
		double bottom = static_cast< double > ( boundsRect.y + boundsRect.h );
		if ( ( rect.y + rect.h ) > bottom )
		{
			rect.y = bottom - rect.h;
			dirY = ( dirY > 0.0f ) ? dirY * -1.0f : dirY;
			return true;
		}
	}
	return false;
}
bool Ball::DeathCheck( const SDL_Rect &boundsRect )
{
	if ( ballOwner == Player::Local )
	{
		double bottom = static_cast< double > ( boundsRect.y + boundsRect.h );

		if (  ( rect.y + rect.h ) > bottom  )
		{
			Reset( boundsRect);
			return true;
		}
	} else  if ( ballOwner == Player::Remote )
	{
		double top  = static_cast< double > ( boundsRect.y );

		if ( rect.y < top  )
		{
			Reset( boundsRect );
			return true;
		}
	}

	return false;
}
bool Ball::PaddleCheck( const Rect &paddleRect )
{
	if ( paddleRect.CheckTileIntersection( rect ) )
	{
		HandlePaddleHit( paddleRect );
		return true;
	}

	return false;
}
void Ball::HandlePaddleHit( const Rect &paddleRect )
{
	double hitPosition = CalculatePaddleHitPosition( paddleRect );

	CalculateNewBallDirection( hitPosition );

	SetSpeed( GetSpeed() * 1.0005f);
	MoveBallOutOfPaddle( paddleRect.y );
}
void Ball::MoveBallOutOfPaddle( double paddleTop )
{
	double bottom = rect.y + rect.h;
	double delta = bottom - paddleTop;

	oldRect.x = rect.x;
	oldRect.y = rect.y;

	rect.x += delta * dirX;
	rect.y += delta * dirY;
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
	dirY = ( dirY > 0.0f ) ? dirY * -1.0f : dirY;

	NormalizeDirection();
}
bool Ball::DidBallHitTile( const Rect &tileRect )
{
	if ( !tileRect.CheckTileIntersection( rect ) )
		return false;

	double dist = 0.0;
	if ( !CheckTileSphereIntersection( tileRect, rect, dist ) )
		return false;

	return true;
}
bool Ball::TileCheck( const Rect &tileRect, bool isSuperBall )
{
	// If the ball collided with the same rect in previous frame too,
	// This means the ball needs an extra frame to get fully out of the tile
	//if ( lastTileHit == tileID ) return false;
	//lastTileHit = tileID;

	if ( !isSuperBall )
		FindIntersectingSide( tileRect );

	return true;
}
bool Ball::CheckTileSphereIntersection( const Rect &tile, const Rect &ball, double &retDistance ) const
{
	double ballRadius  = ball.w / 2.0;
	double ballCenterX = ball.x + ballRadius;
	double ballCenterY = ball.y + ballRadius;

	double tileHalfWidth  = ( tile.w / 2.0 );
	double tileHalfHeight = ( tile.h / 2.0 );
	double tileCenterX = tile.x + tileHalfWidth;
	double tileCenterY = tile.y + tileHalfHeight;

	double distX = fabs( ballCenterX - tileCenterX );
	double distY = fabs( ballCenterY - tileCenterY );

	// If distance from center of sphere to center of rect is larger than
	// the sum of the raidus of the ball and the distance from the center of the rect to the edge
	if ( distX > ( tileHalfWidth + ballRadius ) )
		return false;

	if ( distY > ( tileHalfHeight + ballRadius ) )
		return false;

	// if the distance from the center of the sphere to the center of the rect is smaller or equal to
	// the the distance from the center of the rect to the edge
	if ( distX <= tileHalfWidth )
	{
		retDistance = 0.0f;
		return true;
	}

	if ( distY <= tileHalfHeight )
	{
		retDistance = 0.0f;
		return true;
	}

	// Get the distance from center of the sphere, to the edge of the rect squared.
	double cornerDistance = ( distX - tileHalfWidth ) * ( distX - tileHalfWidth );
	cornerDistance += ( distY - tileHalfHeight ) * ( distY - tileHalfHeight );

	if ( cornerDistance > ( ballRadius * ballRadius ) )
		return false;

	retDistance = sqrt( cornerDistance );

	return true;
}
bool Ball::LineLineIntersectionTestV2( const Vector2f &tile1, const Vector2f &tile2, const Vector2f &ball1, const Vector2f &ball2, double &ret ) const
{
	Vector2f tile( tile2 - tile1 );
	Vector2f ball( ball2 - ball1 );

	Vector2f ballTile( ball2 - tile2 );

	double dot = Math::PerpDot( tile, ball );
	double dotTile = Math::PerpDot( tile, ballTile );
	double dotBall = Math::PerpDot( ball, ballTile );

	if ( CheckDotProducts( dot, dotTile, dotBall ) )
	{
		ret = 1.0f - ( dotTile / dot );
		return true;
	}
	return false;
}
bool Ball::CheckDotProducts( double dot, double dotTile, double dotBall ) const
{
	// Paralell check
	if ( dot == 0.0f )
		return false;

	if ( dot < 0.0 )
	{
		if ( dotTile > 0.0
				|| dotTile < dot
				|| dotBall > 0.0
				|| dotBall < dot
		   )
			return false;
	} else
	{
		if ( dotTile < 0.0
			|| dotTile > dot
			|| dotBall < 0.0
			|| dotBall > dot
		)
			return false;
	}

	return true;
}
int Ball::FindIntersectingSide( const Rect &tileRect )
{
	Vector2f ballDir = GetEsimtatedDir( );
	Vector2f ballCurrentPos( rect.x, rect.y );

	Vector2f ballEstOldPos( ballCurrentPos - ( ballDir * 5.0 ) );
	Vector2f ballEstNewPos( ballCurrentPos + ( ballDir * 20.0 ) );

	// Get ball positions
	Vector2f ballEstOldPos_Bl = Transform( ballEstOldPos, Corner::BottomLeft, rect );
	Vector2f ballEstNewPos_Bl = Transform( ballEstNewPos, Corner::BottomLeft, rect );

	Vector2f ballEstOldPos_Br = Transform( ballEstOldPos, Corner::BottomRight, rect );
	Vector2f ballEstNewPos_Br = Transform( ballEstNewPos, Corner::BottomRight, rect );

	Vector2f ballEstOldPos_Tl = Transform( ballEstOldPos, Corner::TopLeft, rect  );
	Vector2f ballEstNewPos_Tl = Transform( ballEstNewPos, Corner::TopLeft, rect  );

	Vector2f ballEstOldPos_Ml( ballEstOldPos.x, ballEstOldPos.y + ( rect.y / 2.0 ));
	Vector2f ballEstNewPos_Ml( ballEstNewPos.x, ballEstNewPos.y + ( rect.y / 2.0 ));

	Vector2f ballEstOldPos_Tr = Transform( ballEstOldPos, Corner::TopRight, rect  );
	Vector2f ballEstNewPos_Tr = Transform( ballEstNewPos, Corner::TopRight, rect  );

	// Top left corner
	Vector2f rect_Tl( tileRect.x             , tileRect.y );
	Vector2f rect_Tr = Transform( rect_Tl, Corner::TopRight   , tileRect );

	Vector2f rect_Bl = Transform( rect_Tl, Corner::BottomLeft , tileRect );
	Vector2f rect_Br = Transform( rect_Tl, Corner::BottomRight, tileRect );

	double dist = 0.0f;
	double distMax = 0.0f;
	Side collisionSide = Side::Unknown;

	// Top collisions - bottom side of ball
	// ========================================================================================
	if ( LineLineIntersectionTestV2( rect_Tr, rect_Tl, ballEstNewPos_Bl, ballEstOldPos_Bl, dist ) )
	{
		distMax = dist;
		collisionSide = Side::Top;
	}

	if ( LineLineIntersectionTestV2( rect_Tr, rect_Tl, ballEstNewPos_Br, ballEstOldPos_Br, dist  ) )
	{
		distMax = dist;
		collisionSide = Side::Top;
	}

	// Right collisions - left side of ball
	// ========================================================================================
	if ( LineLineIntersectionTestV2( rect_Tr, rect_Br, ballEstNewPos_Tl, ballEstOldPos_Tl, dist  ) )
	{
		if ( dist >= distMax )
		{
			distMax = dist;
			collisionSide = Side::Right;
		}
	}

	if ( LineLineIntersectionTestV2( rect_Tr, rect_Br, ballEstNewPos_Bl, ballEstOldPos_Bl, dist ) )
	{
		if ( dist >= distMax )
		{
			distMax = dist;
			collisionSide = Side::Right;
		}
	}

	if ( LineLineIntersectionTestV2( rect_Tr, rect_Br, ballEstNewPos_Ml, ballEstOldPos_Ml, dist ) )
	{
		/*if ( dist >= distMax )
		  {
		  distMax = dist;
		  collisionSide = Side::Right;
		  }*/
	}

	// Bottom collisions - top side of ball
	// ========================================================================================
	if ( LineLineIntersectionTestV2( rect_Bl, rect_Br, ballEstNewPos_Tl, ballEstOldPos_Tl, dist  ) )
	{
		if ( dist >= distMax )
		{
			distMax = dist;
			collisionSide = Side::Bottom;
		}
	}
	if ( LineLineIntersectionTestV2( rect_Bl, rect_Br, ballEstNewPos_Tr, ballEstOldPos_Tr, dist  ) )
	{
		if ( dist >= distMax )
		{
			distMax = dist;
			collisionSide = Side::Bottom;
		}
	}

	// Left collisions - right side of ball
	// ========================================================================================
	if ( LineLineIntersectionTestV2( rect_Bl, rect_Tl, ballEstNewPos_Br, ballEstOldPos_Br, dist  ) )
	{
		if ( dist >= distMax )
		{
			distMax = dist;
			collisionSide = Side::Left;
		}
	}

	// Check intersection left II
	if ( LineLineIntersectionTestV2( rect_Bl, rect_Tl, ballEstNewPos_Tr, ballEstOldPos_Tr, dist  ) )
	{
		if ( dist >= distMax )
		{
			distMax = dist;
			collisionSide = Side::Left;
		}
	}

	ChangeBallDirection( collisionSide );

	return 0;
}
Vector2f Ball::Transform( const Vector2f &vec, const Corner &side, const Rect &size ) const
{
	switch ( side )
	{
		case Corner::TopLeft:
			return vec;
		case Corner::TopRight:
			return Vector2f( vec.x + size.w, vec.y );
		case Corner::BottomLeft:
			return Vector2f( vec.x, vec.y + size.h );
		case Corner::BottomRight:
			return Vector2f( vec.x + size.w, vec.y + size.h );
	}
}
Vector2f Ball::GetEsimtatedDir( ) const
{
	Vector2f ballCurrentPos( rect.x   , rect.y );
	Vector2f ballPrevPos   ( oldRect.x, oldRect.y );

	return Vector2f( ballCurrentPos - ballPrevPos  );
}
Vector2f Ball::GetDirection( ) const
{
	return Vector2f( dirX, dirY );
}
void Ball::SetDirection( const Vector2f &newDir )
{
	dirX = newDir.x;
	dirY = newDir.y;
}
void Ball::SetOwner( Player owner )
{
	ballOwner = owner;
}
Player Ball::GetOwner( ) const
{
	return ballOwner;
}
void Ball::SetRemoteScale( double scale_ )
{
	SetSpeed( GetSpeed() * scale_ );
}
