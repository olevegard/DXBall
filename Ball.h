#pragma once

#include "GamePiece.h"

#include <string>

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

	bool TileCheck( const SDL_Rect &paddleRect, unsigned int tileID );

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
	void HandleTileIntersection( const SDL_Rect &tile );
	void HandleTileIntersection2( const SDL_Rect &tileRect );

	void HandlePaddleHit( const SDL_Rect &paddleRect );
	float CalculatePaddleHitPosition( const SDL_Rect &paddleRect ) const;
	void  CalculateNewBallDirection( float hitPosition );

	void PrintPosition( const SDL_Rect &pos, const std::string &tilename );

	float speed;

	float dirX;
	float dirY;

	int ballOwner;

	unsigned int lastTileHit;
};

