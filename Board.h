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
	Edges FindEdges( const SDL_Rect &rect );
	void FlipBoard( double height );

	void AddTile( short xPos, short yPos, TileType tt )
	{
		tiles.push_back( TilePosition( xPos, yPos, tt  ) );
	}
	void AddTile( TilePosition pos )
	{
		tiles.push_back( pos );
	}

	double GetResolutionX( ) const
	{
		return boardWidth;
	}

	double GetResolutionY( ) const
	{
		return boardHeight;
	}

	void CalcMaxScale( const SDL_Rect &rect );

	void CenterAndFlip( const SDL_Rect &rect, bool isServer );

	std::vector< TilePosition > GetTiles()
	{
		return tiles;
	}
	double GetScale() const
	{
		return scale;
	}
	private:
	std::vector< TilePosition > tiles;
	double scale;
	double boardWidth;
	double boardHeight;
};
