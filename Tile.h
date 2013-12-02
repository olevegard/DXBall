#pragma once

#include "GamePiece.h"
#include "enums/TileType.h"

struct Tile : GamePiece
{
	public:
	Tile( TileType type_, unsigned int ID);

	virtual ~Tile();

	TileType GetTileType()
	{
		return type;
	}

	unsigned long GetTileTypeAsIndex()
	{
		return static_cast<unsigned long > ( type );
	}

	unsigned int GetHitsLeft()
	{
		return hitsLeft;
	}

	bool CheckExplosion( const Rect &explodingTile );

	void Hit();

	private:
		TileType type;
		unsigned short hitsLeft;
};
inline bool operator==( const Tile &tile1, const Tile &tile2)
{
	return tile1.GetObjectID() == tile2.GetObjectID();
}
inline bool operator!=( const Tile &tile1, const Tile &tile2)
{
	return tile1.GetObjectID() != tile2.GetObjectID();
}
