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

	bool PaddleCheck( const Rect &paddleRect );

	bool TileCheck( const Rect &paddleRect, unsigned int tileID );

	void SetOwner( int owner )
	{
		ballOwner = owner;
	}

	int SetOwner( )
	{
		return ballOwner;
	}

	private:
	bool CheckTileIntersection( const Rect &tile, const Rect &ball ) const;
	void HandleTileIntersection( const Rect &tile );
	void HandleTileIntersection2( const Rect &tileRect );
	
	bool CheckTileSphereIntersection( const Rect &tile, const Rect &ball ) const;

	void HandlePaddleHit( const Rect &paddleRect );
	double CalculatePaddleHitPosition( const Rect &paddleRect ) const;
	void  CalculateNewBallDirection( double  hitPosition );

	void PrintPosition( const Rect &pos, const std::string &tilename ) const;

	double speed;

	double dirX;
	double dirY;

	int ballOwner;

	unsigned int lastTileHit;

	bool paddleHitInPrevFrame;

	bool debugMode;
};
