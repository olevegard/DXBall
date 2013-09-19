#pragma once
#include <algorithm>
#include <iostream>

struct TilePosition
{
	TilePosition()
		:	TilePosition( 0, 0, TileTypes::Regular )
	{
	}
	TilePosition( short xPos_, short yPos_, const TileTypes &tt )
		:	xPos(xPos_)
		,	yPos(yPos_)
		,	type( tt )
	{

	}

	short xPos;
	short yPos;

	TileTypes type;
};

struct Board
{
	void AddTile( short xPos, short yPos, TileTypes tt )
	{
		tiles.push_back( TilePosition( xPos, yPos, tt  ) );
	}

	/*
	void TransformBoard( const SDL_Rect &windowSize )
	{
		short tileHeight = 25;
		short boardHeight = ( countRows * tileHeight );
		short totalVerticalFreeSpace = static_cast< short > (windowSize.h - boardHeight  + 20 );
		short halfVerticalFreeSpace = static_cast< short > ( totalVerticalFreeSpace / 2 );
	}*/


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
	}

	std::vector< TilePosition > GetTiles()
	{
		return tiles;
	}
	private:
	long countRows;
	//short startX;
	//short startY;

	std::vector< TilePosition > tiles;
};

struct BoardLoader
{
	//SDL_Rect windowSize;
	Board board;

	std::vector< TilePosition > GetTiles()
	{
		return board.GetTiles();
	}
	void GenerateBoard( const SDL_Rect &windowSize )
	{
		short x = 60;
		short y = 200;

		short countRows = 15;
		short tileHeight = 25;
		short boardHeight = ( countRows * tileHeight );
		short totalVerticalFreeSpace = static_cast< short > (windowSize.h - boardHeight  + 20 );
		short halfVerticalFreeSpace = static_cast< short > ( totalVerticalFreeSpace / 2 );

		y = halfVerticalFreeSpace;

		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );//Middle
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );

		x = 60;
		y += 25;

		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );

		x = 60;
		y += 25;

		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );

		x = 60;
		y += 25;

		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );

		x = 60;
		y += 25;

		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );

		x = 60;
		y += 25;

		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );

		x = 60;
		y += 25;

		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );

		x = 60;
		y += 25;

		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );

		x = 60;
		y += 25;

		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );

		x = 60;
		y += 25;


		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );

		x = 60;
		y += 25;

		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Hard );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );

		x = 60;
		y += 25;

		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );

		x = 60;
		y += 25;

		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );

		x = 60;
		y += 25;

		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );

		x = 60;
		y += 25;

		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );
		x += 65;
		board.AddTile( x, y, TileTypes::Regular );

	}
};
