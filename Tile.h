#pragma once

#include "GamePiece.h"

enum class TileTypes
{
	Regular,
	Explosive,
	Hard,
	Unbreakable
};
struct Tile : GamePiece
{
	Tile( TileTypes type_);

	virtual ~Tile();

	size_t GetTileTypeAsIndex()
	{
		return static_cast<size_t> ( type );
	}
	private:
	TileTypes type;
};
