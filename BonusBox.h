#pragma once
#include "GamePiece.h"

struct SDL_Texture;
struct BonusBox : GamePiece
{
	BonusBox()
		:	effectName( "Life" )
	{
		rect.x = 100;
		rect.y = 100;
		rect.w =  40;
		rect.h =  40;//84
	}

	std::string GetName() const
	{
		return effectName;
	}

	void SetTexture( SDL_Texture* generatedTexture );
	SDL_Texture* GetTexture( ) const;

	private:
		std::string effectName;
		SDL_Texture* texture;
};
