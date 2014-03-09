#include "GamePiece.h"

#include <SDL2/SDL.h>

GamePiece::GamePiece()
	:	rect( )
	,	oldRect( )
	,	textureType( TextureType::e_Paddle )
	,	isAlive( true )
	,	scale( 1.0 )
	,	speed( 0.01 )
	{

	}
void GamePiece::SetTexture( SDL_Texture* generatedTexture )
{
	texture = generatedTexture;
}
SDL_Texture* GamePiece::GetTexture( ) const
{
	return texture;
}
