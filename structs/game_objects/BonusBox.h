#pragma once
#include "GamePiece.h"

#include "math/Vector2f.h"

#include "enums/Player.h"
#include "enums/BonusType.h"

struct SDL_Texture;
struct BonusBox : GamePiece
{
	BonusBox( int32_t objectID );

	//std::string GetName() const;

	void SetTexture( SDL_Texture* generatedTexture );
	SDL_Texture* GetTexture( ) const;

	void SetOwner( const Player &hitBy );
	Player GetOwner() const;

	void FlipXDir();

	BonusType GetBonusType() const
	{
		return bonusType;
	}
	void SetBonusType( BonusType bonusType_ )
	{
		bonusType = bonusType_;
	}

	private:
		Player owner;
		BonusType bonusType;
		Vector2f direction;
		SDL_Texture* texture;
};
