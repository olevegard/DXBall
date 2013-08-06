#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

struct GamePiece
{
	GamePiece()
		:	rect( )
		,	textureType( Paddle )
	{

	}

	/*virtual ~GamePiece()
	{

	}*/

	SDL_Rect rect;


	enum TextureType
	{
		Paddle = 0,
		Ball = 1,
		Tile = 2,

		Background = 99,
	};

	
	TextureType textureType;

	private:
	GamePiece( const GamePiece &gamePiece );
	GamePiece& operator=( const GamePiece &gamePiece );
};
