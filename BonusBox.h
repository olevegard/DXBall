#pragma once
#include "GamePiece.h"

struct SDL_Texture;
struct BonusBox : GamePiece
{
	BonusBox();

	std::string GetName() const;

	void SetTexture( SDL_Texture* generatedTexture );
	SDL_Texture* GetTexture( ) const;

	void SetOwner( const Player &hitBy );
	Player GetOwner() const;

	private:
		Player owner;
		std::string effectName;
		SDL_Texture* texture;
};
