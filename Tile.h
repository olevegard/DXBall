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
		TileType type;
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
