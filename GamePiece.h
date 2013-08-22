#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

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

	SDL_Rect rect;
	SDL_Rect oldRect;

	
	
	TextureType textureType;

	private:
	GamePiece( const GamePiece &gamePiece );
	GamePiece& operator=( const GamePiece &gamePiece );
};
