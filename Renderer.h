#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "GamePiece.h"
#include <sstream>

struct Ball;
struct Tile;
struct Paddle;

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Init();
	bool Render( );

	void AddBall( const std::shared_ptr< Ball > &ball );
	void RemoveBall( const std::shared_ptr< Ball >  &ball );

	void SetLocalPaddle( std::shared_ptr< Paddle >  &paddle )
	{
		localPaddle = paddle;
	}

	void RenderText( const std::string &textToRender )
	{
		textColor.g = 123;
		textColor.b = 123;
		text = TTF_RenderText_Solid( bigFont, textToRender.c_str(), textColor );
	}

	void RemoveText()
	{
		text = NULL;
	}
	
	void RenderLives( unsigned long lifeCount )
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
		return rects[GamePiece::Paddle];
	}
	SDL_Rect GetWindowSize()
	{
		return rects[GamePiece::Background];
	}
private:

	Renderer( const Renderer &renderer );
	Renderer& operator=( const Renderer &renderer );

	SDL_Surface* LoadImage( const std::string &filename, GamePiece::TextureType textureType );
	void SetColorKey( SDL_Surface* source, int r, int g, int b );
	void SetColorKey( GamePiece::TextureType textureID, int r, int g, int b );
	void FillSurface( SDL_Surface* source, int r, int g, int b );

	void ApplySurface( short x, short y, SDL_Surface* source, SDL_Surface* destination ) const;
	void ApplySurface( const SDL_Rect &r, SDL_Surface* source, SDL_Surface* destination ) const;

	bool LoadAllFiles( );

	void BlitBackground() const;
	void BlitForeground();
	void BlitText();

	const int SCREEN_WIDTH;
	const int SCREEN_HEIGHT;
	const int SCREEN_BPP;

	std::map< int, SDL_Rect > rects;
	std::map< int, SDL_Surface* > textures;

	SDL_Surface *backgroundArea;
	SDL_Surface *backgroundImage;
	SDL_Surface *screen;

	std::vector< std::shared_ptr< Ball >  > ballList;
	std::vector< std::shared_ptr< Tile >  > tileList;

	std::shared_ptr< Paddle >  localPaddle;

	// Text
	TTF_Font* font;
	TTF_Font* bigFont;
	SDL_Surface* text;
	SDL_Surface* lives;
	SDL_Surface* points;
	SDL_Color textColor;
};
