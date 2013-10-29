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
	/*
	void SetResolution( double resX_, double resY_ )
	{
		resX = resX_;
		resY = resY_;
	}*/
	double GetResolutionX( ) const
	{
		return boardWidth;
	}

	double GetResolutionY( ) const
	{
		return boardHeight;
	}
	double FindMaxScale( const SDL_Rect &rect );

	/*
	   void TransformBoard( const SDL_Rect &windowSize )
	   {
	   short tileHeight = 25;
	   short boardHeight = ( countRows * tileHeight );

	   short totalVerticalFreeSpace = static_cast< short > (windowSize.h - boardHeight  + 20 );
	   short halfVerticalFreeSpace = static_cast< short > ( totalVerticalFreeSpace / 2 );
	   }*/

	/*
	   void MoveVertically( short amount )
	   {
	   auto moveDown_Func = [amount]( TilePosition tt )
	   {
	   tt.yPos += amount;
	   return tt;
	   };

	   std::transform( tiles.begin(), tiles.end(),tiles.begin(),  moveDown_Func );

	   }
	   void CountRows( )
	   {
	   std::vector<int> yValues;
	   auto count_Func = [&]( TilePosition tt )
	   {
	   if ( std::find( yValues.begin(), yValues.end(), tt.yPos ) == yValues.end() )
	   {
	   yValues.push_back( tt.yPos );
	   return true;
	   }

	   return false;
	   };
	   countRows= std::count_if( tiles.begin(), tiles.end(), count_Func );
	   std::cout << "Count : " << countRows << std::endl;
	   }*/
	void CenterAndFlip( const SDL_Rect &rect, bool isServer );

	std::vector< TilePosition > GetTiles()
	{
		return tiles;
	}
	private:
	//long countRows;
	//short startX;
	//short startY;

	std::vector< TilePosition > tiles;
	double boardWidth;
	double boardHeight;
};
