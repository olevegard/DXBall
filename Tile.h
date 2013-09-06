#pragma once

#include "GamePiece.h"
#include <cstddef>

enum class TileTypes
{
	Regular,
	Explosive,
	Hard,
	Unbreakable
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
		return ( type != TileTypes::Unbreakable && hitsLeft == 0 );
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
