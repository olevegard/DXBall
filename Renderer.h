#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "GamePiece.h"
#include <sstream>

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Init();
	bool Render( );
	void AddObject( std::shared_ptr< GamePiece >  &gamePiece );

	void RenderText( const std::string &textToRender )
	{
		//.r = 123;
		textColor.g = 123;
		textColor.b = 123;
		text = TTF_RenderText_Solid( font, textToRender.c_str(), textColor );
	}
	
	void RenderLives( unsigned short lifeCount )
	{
		textColor.r = 123;
		textColor.g = 123;
		textColor.b = 123;

		std::stringstream ss;
		ss << "Lives : " << lifeCount;
		lives = TTF_RenderText_Solid( font, ss.str().c_str(), textColor );
	}
	
	void RenderPoints( unsigned short pointCount )
	{
		textColor.r = 123;
		textColor.g = 123;
		textColor.b = 123;

		std::stringstream ss;
		ss << "Points : " << pointCount;
		points = TTF_RenderText_Solid( font, ss.str().c_str(), textColor );
	}
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

	bool LoadAllFiles( );

	void BlitBackground();
	void BlitForeground();
	void BlitText();

	const int SCREEN_WIDTH;
	const int SCREEN_HEIGHT;
	const int SCREEN_BPP;

	SDL_Surface *paddle;
	SDL_Surface *ball;

	SDL_Surface *backgroundArea;
	SDL_Surface *backgroundImage;

	SDL_Surface *gameArea;

	SDL_Surface *screen;

	std::vector< std::shared_ptr< GamePiece >  > gamePieceList;

	SDL_Rect tileSize;
	SDL_Rect screenSize;

	TTF_Font* font;
	SDL_Surface* text;
	SDL_Surface* lives;
	SDL_Surface* points;
	SDL_Color textColor;
};
