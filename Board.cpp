#include "Board.h"

#include <SDL2/SDL.h>

#include <iostream>
#include <algorithm>


void Board::CenterAndFlip( const SDL_Rect &rect, bool isServer )
{
	std::cout << "Screen size... " << rect.w << " x " << rect.h  << std::endl;
	std::cout << "Is Server ? " << std::boolalpha << isServer << std::endl;

	double bottom = rect.h - 75 + 20;
	double top = 75;
	double middle = ( bottom + top ) / 2.0;
	std::cout << "Top : " << top << "\nBottom : " << bottom << "\nMiddle : " << middle << "\nBoard height :" << bottom - top <<  std::endl;
	double heighestTile = rect.h;
	double lowestTile = 0.0;

	for ( const auto &p : tiles )
	{
		if ( p.yPos < heighestTile )
			heighestTile = p.yPos;

		if ( p.yPos > lowestTile )
			lowestTile = p.yPos;
	}
	lowestTile  += 20;
	std::cout << "Lowset tile : " << lowestTile << "\nHighest tile : " << heighestTile << std::endl;
	double boardHeight  = lowestTile - heighestTile;
	std::cout << "Height : " << boardHeight << std::endl;
	std::cout << "Middle : " << ( heighestTile + lowestTile ) / 2 << std::endl;
	double newTop = top  +  (( ( bottom - top ) - boardHeight )  / 2 );
	double diff = newTop - heighestTile;
	std::cout << "New top : " << newTop << std::endl;
	std::cout << "Diff : " << diff << std::endl;

	//for ( auto &p : tiles ) p.yPos += 50;
	for ( auto &p : tiles ) p.yPos += (diff - 10);

	if ( !isServer )
	{	
		for ( size_t i = 0; i < tiles.size() ; ++i )
			tiles[i].yPos = static_cast< short > (rect.h ) - tiles[i].yPos;
	}
}