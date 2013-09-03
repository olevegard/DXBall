#pragma once

#include "math/Rect.h"

struct GamePiece
{
	enum TextureType
	{
		Paddle = 0,
		Ball = 1,
		Tile = 2,

		Background = 99,
	};

	GamePiece();

	Rect rect;
	Rect oldRect;

	TextureType textureType;

	private:
	GamePiece( const GamePiece &gamePiece );
	GamePiece& operator=( const GamePiece &gamePiece );
};
