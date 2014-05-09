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
void GamePiece::SetOriginalSize( const SDL_Rect &r )
{
	originalSize.w = r.w;
	originalSize.h = r.h;
}
void GamePiece::ResetSize( )
{
	rect.w = originalSize.w;
	rect.h = originalSize.h;
}
