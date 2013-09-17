#pragma once

#include "GamePiece.h"

enum class TileTypes
{
	Regular,
	Explosive,
	Hard,
	Unbreakable,
	Wall_Of_Death // Balls can pass through towards their owner's tile, but not the other way. They have to be hit 3 ( ? ) times before they are destroyed.
};
enum class BonusTypes
{
	// Local player bonuses
	Extra_Life, // Obvious
	Ball_Split, // Ball split into two
	Super_Ball, // Ball goes trhough everything

	// Remote player bonus.
	Ball_Steal, // Remote player looses a ball to local  player. ( Can be changed to 'looses all balls but one' )
	Ball_Loose, // Local  player looses a ball to remote player. ( Can be changed to 'looses all balls but one' )
	Death_Wall, // A line of special wall of death tiles covers the screen left to right above/under remote player paddle. Wall moves toward oponent every x seconds.
	Bonus_Steal, // Steal all bonuses from oponent.

};

struct Tile : GamePiece
{
	public:
	Tile( TileTypes type_, unsigned int ID);

	virtual ~Tile();

	TileTypes GetTileType()
	{
		return type;
	}

	BonusTypes GetBonusType()
	{
		return bonusType;
	}

	size_t GetTileTypeAsIndex()
	{
		return static_cast<size_t> ( type );
	}

	unsigned int GetHitsLeft()
	{
		return hitsLeft;
	}

	bool IsDestroyed()
	{
		return hitsLeft == 0;
	}

	bool CheckExplosion( const Rect &explodingTile );

	void ExpandRect( Rect &explodingTile ) const;

	void Hit();

	unsigned int GetTileID() const
	{
		return tileID;
	}

	private:
		TileTypes type;
		BonusTypes bonusType;
		unsigned short hitsLeft;
		unsigned int tileID;
};
inline bool operator==( const Tile &tile1, const Tile &tile2)
{
	return tile1.GetTileID() == tile2.GetTileID();
}
inline bool operator!=( const Tile &tile1, const Tile &tile2)
{
	return tile1.GetTileID() != tile2.GetTileID();
}
