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
	void SetScale( double scale_ )
	{
		scale = scale_;
		rect.w *= scale;
		rect.h *= scale;
	}
	void ResetScale()
	{
		rect.w *= ( 1.0 / scale );
		rect.h *= ( 1.0 / scale );
	}
	private:
	GamePiece( const GamePiece &gamePiece );
	GamePiece& operator=( const GamePiece &gamePiece );
	bool isAlive;
	double scale;
};
