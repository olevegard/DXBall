#pragma once

#include "GamePiece.h"
#include <iostream>

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
	Tile( TileTypes type_);

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

	void Hit()
	{
		--hitsLeft;

		if ( type == TileTypes::Hard )
			std::cout << "Hits left : " << hitsLeft << std::endl;
	}

	private:
		TileTypes type;
		unsigned short hitsLeft;
};
