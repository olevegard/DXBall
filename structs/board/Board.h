#pragma once

#include <vector>

#include "TilePosition.h"

struct Edges
{
	Edges()
		:	left	( 0.0 )
		,	right	( 0.0 )
		,	top		( 0.0 )
		,	bottom	( 0.0 )
		{

		}
	double left;
	double right;

	double top;
	double bottom;

	double GetDiffY()
	{
		return ( bottom - top );
	}
	double GetDiffX()
	{
		return ( right - left );
	}
};


struct Board
{
	void AddTile( short xPos, short yPos, TileType tt );
	void AddTile( TilePosition pos );

	double GetResolutionX( ) const;
	double GetResolutionY( ) const;

	std::vector< TilePosition > GetTiles();

	double GetScale() const;

	void CalcMaxScale( const SDL_Rect &rect );
	void CenterAndFlip( const SDL_Rect &rect );

	std::string levelName;

	private:
	Edges FindEdges( const SDL_Rect &rect );

	std::vector< TilePosition > tiles;
	double scale;
	double boardWidth;
	double boardHeight;
};
