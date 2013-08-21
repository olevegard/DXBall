#pragma once

#include "GamePiece.h"

struct Ball : GamePiece
{
	Ball();

	virtual ~Ball();

	void Reset();
	void NormalizeDirection();

	void Update( double tick );

	bool BoundCheck( const SDL_Rect &boundsRect );

	bool DeathCheck( const SDL_Rect &boundsRect );

	bool PaddleCheck( const SDL_Rect &paddleRect );

	bool TileCheck( const SDL_Rect &paddleRect );

	void SetOwner( int owner )
	{
		ballOwner = owner;
	}

	int SetOwner( )
	{
		return ballOwner;
	}

	private:
	bool CheckTileIntersection( const SDL_Rect &tile, const SDL_Rect &ball ) const;

	void HandlePaddleHit( const SDL_Rect &paddleRect );
	float CalculatePaddleHitPosition( const SDL_Rect &paddleRect ) const;
	void  CalculateNewBallDirection( float hitPosition );

	float speed;

	float dirX;
	float dirY;

	int ballOwner;
};

