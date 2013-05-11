#pragma once

struct GamePiece
{
	GamePiece()
		:	rect( )
		,	textureType( 0 )
	{

	}

	virtual ~GamePiece()
	{

	}

	SDL_Rect rect;

	int textureType;

private:
	GamePiece( const GamePiece &gamePiece );
	GamePiece& operator=( const GamePiece &gamePiece );
};
