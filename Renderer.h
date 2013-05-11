#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "GamePiece.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Init();
	bool Render( );
	void AddObject( std::shared_ptr< GamePiece >  &gamePiece );

	SDL_Rect GetTileSize()
	{
		return tileSize;
	}
	SDL_Rect GetWindowSize()
	{
		return screenSize;
	}
private:

	Renderer( const Renderer &renderer );
	Renderer& operator=( const Renderer &renderer );

	SDL_Surface* LoadImage( const std::string &filename );
	void SetColorKey( SDL_Surface* source, int r, int g, int b );
	void FillSurface( SDL_Surface* source, int r, int g, int b );

	void ApplySurface( int x, int y, SDL_Surface* source, SDL_Surface* destination );

	void LoadAllTextures( );

	void BlitBackground();
	void BlitForeground();

	const int SCREEN_WIDTH;
	const int SCREEN_HEIGHT;
	const int SCREEN_BPP;

	SDL_Surface *tile;
	SDL_Surface *ball;

	SDL_Surface *backgroundArea;
	SDL_Surface *background;

	SDL_Surface *gameArea;

	SDL_Surface *screen;
	
	std::vector< std::shared_ptr< GamePiece >  > gamePieceList;

	SDL_Rect tileSize;
	SDL_Rect screenSize;
};
