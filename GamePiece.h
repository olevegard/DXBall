#pragma once

#include "math/Rect.h"

enum class Player
{
	Local,
	Remote
};
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

	void Kill()
	{
		isAlive = false;
	}
	bool IsAlive() const
	{
		return isAlive;
	}
	private:
	bool isAlive;
	GamePiece( const GamePiece &gamePiece );
	GamePiece& operator=( const GamePiece &gamePiece );
};
