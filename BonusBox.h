#pragma once
#include "GamePiece.h"

#include "math/Vector2f.h"
#include "enums/Player.h"

struct SDL_Texture;
struct BonusBox : GamePiece
{
	BonusBox();

	std::string GetName() const;

	void SetTexture( SDL_Texture* generatedTexture );
	SDL_Texture* GetTexture( ) const;

	void SetOwner( const Player &hitBy );
	Player GetOwner() const;

	void SetDirection( const Vector2f &dir );
	Vector2f GetDirection( ) const;
	void FlipXDir();

	private:
		Player owner;
		Vector2f direction;
		std::string effectName;
		SDL_Texture* texture;
};
