#include "Board.h"

#include <SDL2/SDL.h>

#include <iostream>
#include <algorithm>


void Board::CenterAndFlip( const SDL_Rect &rect, bool isServer )
{
	Edges edges = FindEdges( rect );

	boardHeight  = edges.GetDiffY();
	boardWidth  = edges.GetDiffX();

	double distToBottom = rect.h - ( edges.bottom );
	double boardMoveY = ( distToBottom - edges.top ) / 2.0;

	double distToRight = rect.w - ( edges.right  );
	double boardMoveX = ( distToRight - edges.left ) / 2.0;

	for ( auto &p : tiles )
	{
		p.xPos += boardMoveX;
		p.yPos += boardMoveY;
	}

	if ( !isServer )
		FlipBoard( rect.h / 2.0 );
}
void Board::CalcMaxScale( const SDL_Rect &rect )
{
	Edges edges = FindEdges( rect );

	double distToBottom = rect.h - edges.bottom;
	double minDistToBottom = 100;

	scale = (  boardHeight - ( ( minDistToBottom - distToBottom ) * 2 )) / boardHeight;

	if ( scale > 1.0 )
	{
		double scaleX = 0.0;
		if ( edges.left != 0 && ( edges.right != rect.w ) )
		{
			//double distToLeft = rect.w - edges.right;
			double distToLeft =  edges.left;
			double minDistToRight = 0;

			scaleX = (  boardWidth - ( ( minDistToRight - distToLeft ) * 2 )) / boardWidth;
		} else
		{
			scaleX = 1.0;
		}

		scale = ( scale > scaleX ) ? scaleX : scale;
		scale = ( scale < 1.0 ) ? scale : 1.0;
	}
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

void Board::AddTile( short xPos, short yPos, TileType tt )
{
	tiles.push_back( TilePosition( xPos, yPos, tt  ) );
}
void Board::AddTile( TilePosition pos )
{
	tiles.push_back( pos );
}
double Board::GetResolutionX( ) const
{
	return boardWidth;
}
double Board::GetResolutionY( ) const
{
	return boardHeight;
}
std::vector< TilePosition > Board::GetTiles()
{
	return tiles;
}
double Board::GetScale() const
{
	return scale;
}
