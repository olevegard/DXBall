
#pragma once

#include "Tile.h"
#include <algorithm>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

// The location and type of a single tile
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
/*
	TilePosition& operator=( TilePosition &&pos )
	{
		xPos = pos.xPos;
		yPos = pos.yPos;
		type = pos.type;

		return *this;
	}
	*/
	short xPos;
	short yPos;

	TileTypes type;
};

inline std::istream& operator>>( std::istream &is, TilePosition &pos )
{
	int type = 0;
	is >> pos.xPos >> pos.yPos >> type;
	pos.type = static_cast< TileTypes > ( type );

	if ( type < 0 || type > 5 )
	{
		is.setstate( std::ios_base::failbit );
		std::cout << "Error! " << type << " is not a valid tyle type!" << std::endl;
	}

	return is;
}
inline std::ostream& operator<<( std::ostream &os, const TilePosition &pos )
{
	os << std::setw(4) << pos.xPos << " " << std::setw(4) << pos.yPos << " ";

	switch ( pos.type )
	{
		case TileTypes::Regular:
			os << "Regular";
			break;
		case TileTypes::Hard:
			os << "Hard";
			break;
		case TileTypes::Unbreakable:
			os << "Unbreakable";
			break;
		case TileTypes::Explosive:
			os << "Explosive";
			break;
		default:
			os << "Unknown";
			break;

	}
	os << std::endl;

	return os;
}

// An enire board
struct Board
{
	void AddTile( short xPos, short yPos, TileTypes tt )
	{
		tiles.push_back( TilePosition( xPos, yPos, tt  ) );
	}
	void AddTile( TilePosition pos )
	{
		tiles.push_back( pos );
	}

	/*
	void TransformBoard( const SDL_Rect &windowSize )
	{
		short tileHeight = 25;
		short boardHeight = ( countRows * tileHeight );

		short totalVerticalFreeSpace = static_cast< short > (windowSize.h - boardHeight  + 20 );
		short halfVerticalFreeSpace = static_cast< short > ( totalVerticalFreeSpace / 2 );
	}*/

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
	BoardLoader()
		:	currentLevel( 0 )
		,	levels(  )
	{
		BuildLevelList();
	}


	/*std::vector< TilePosition > GetTiles()
	{
		return board.GetTiles();
	}*/

	void BuildLevelList()
	{
		std::string line;
		std::ifstream boardFile( "boards/boardlist.txt" );

		while ( getline( boardFile, line ) )
		{
			if ( line[0] == '#' || line.empty() )
			{
				std::cout << "Skipping : " << line << std::endl;
				std::cout << "===========================================================================\n";
				continue;
			} else
			{
				std::cout << "Adding : " << line << std::endl;
			}

			levels.push_back( line );
		}
	}

	Board LoadLevel( const std::string &textFile )
	{
		std::string filePath = "boards/";
		filePath.append( textFile );
		std::cout << "Loading level : " << filePath << std::endl;

		std::ifstream boardFile( filePath );
		TilePosition pos;
		std::string line;
		Board board;
		while ( getline( boardFile, line ) )
		{
			if ( line[0] == '#' || line.empty() )
				continue;

			std::stringstream( line ) >> pos;
			board.AddTile( pos );
		}
		return board;
	}
	std::vector< TilePosition > GenerateBoard( )
	{
		if ( currentLevel >= levels.size() )
		{
			std::vector< TilePosition > emptyVec;
			return emptyVec;
		}

		std::string level = levels[ currentLevel ];

		Board b = LoadLevel( level  );

		++currentLevel;

		return b.GetTiles();
	}

	private:
	size_t currentLevel;
	std::vector< std::string > levels;
};
