#pragma once

#include "math/Rect.h"

#include "enums/TextureType.h"

struct GamePiece
{
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
