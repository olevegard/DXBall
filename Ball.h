#pragma once

#include "GamePiece.h"
#include <math.h>


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

	private:

	float speed;

	float dirX;
	float dirY;
};

