#include "BonusBox.h"


void BonusBox::SetTexture( SDL_Texture* generatedTexture )
{
	texture = generatedTexture;
}
SDL_Texture* BonusBox::GetTexture( ) const
{
	return texture;
}

