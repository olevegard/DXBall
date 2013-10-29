#include "Board.h"

#include <SDL2/SDL.h>

#include <iostream>
#include <algorithm>

double Board::FindMaxScale( const SDL_Rect &rect )
{
	Edges edges = FindEdges( rect );

	double distToBottom = rect.h - edges.bottom;
	double minDistToBottom = 150;

	double scale = (  boardWidth - ( ( minDistToBottom - distToBottom ) * 2 )) / boardWidth ;
	return scale;
}
void Board::CenterAndFlip( const SDL_Rect &rect, bool isServer )
{
	Edges edges = FindEdges( rect );

	boardHeight  = edges.GetDiffY();
	boardWidth  = edges.GetDiffX();

	double distToBottom = rect.h - ( edges.bottom );
	double boardMoveY = ( distToBottom - edges.top ) / 2.0;

	double distToRight = rect.w - ( edges.right );
	double boardMoveX = ( distToRight - edges.left ) / 2.0;

	std::cout << "Screen size... "		<< rect.w << " x " << rect.h  << std::endl;
	std::cout	<< "Top : "				<< edges.top
		<< "\nBottom : "		<< edges.bottom
		<< "\nBoard height :"	<< boardHeight
		<< "\nBoard widhth :"	<< boardWidth

		<< "\nDist to top :"	<< edges.top
		<< "\nDist to bottom :"	<< distToBottom
		<< "\nBoard move Y : "	<< boardMoveY

		<< "\nDist to left :"	<< edges.left
		<< "\nDist to right :"	<< distToRight
		<< "\nBoard move X : "	<< boardMoveX
		<< std::endl;

	for ( auto &p : tiles )
	{
		p.xPos += boardMoveX;
		p.yPos += boardMoveY;
	}

	std::cout << "Is Server ? " << std::boolalpha << isServer << std::endl;
	if ( !isServer )
		FlipBoard( rect.h / 2.0 );
}
void Board::FlipBoard( double middle )
{
	for ( size_t i = 0; i < tiles.size() ; ++i )
	{
		double tileMiddle = tiles[i].yPos + 10;
		if ( tileMiddle  > middle )
			tiles[i].yPos -= ( tileMiddle - middle ) * 2;
		if ( tileMiddle  < middle )
			tiles[i].yPos += ( middle - tileMiddle ) * 2;
	}
}
Edges Board::FindEdges( const SDL_Rect &rect )
{
	Edges edges;
	edges.left = rect.w;
	edges.right = 0.0;
	edges.top = rect.h;
	edges.bottom = 0.0;

	for ( const auto &p : tiles )
	{
		if ( p.yPos < edges.top )
			edges.top = p.yPos;

		if ( p.yPos > edges.bottom )
			edges.bottom = p.yPos;

		if ( p.xPos < edges.left )
			edges.left = p.xPos;

		if ( p.xPos > edges.right )
			edges.right = p.xPos;
	}

	edges.bottom += 20;
	edges.right += 60;

	return edges;
}
