#pragma once

#include <SDL/SDL_ttf.h>

#include <sstream>
#include <vector>
#include <memory>
#include <map>

#include "GamePiece.h"

// Forward declarations
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

	void AddTile( const std::shared_ptr< Tile > &tile );
	void RemoveTile( const std::shared_ptr< Tile >  &tile );

	void SetLocalPaddle( std::shared_ptr< Paddle >  &paddle );

	void RenderText( const std::string &textToRender );
	void RemoveText();
	void RenderLives( unsigned long lifeCount );
	void RenderPoints( unsigned int pointCount );

	SDL_Rect GetTileSize();
	SDL_Rect GetWindowSize();
private:

	Renderer( const Renderer &renderer );
	Renderer& operator=( const Renderer &renderer );

	SDL_Surface* LoadImage( const std::string &filename, GamePiece::TextureType textureType );

	void SetColorKey( SDL_Surface* source             , unsigned char r, unsigned char g, unsigned char b );
	void SetColorKey( GamePiece::TextureType textureID, unsigned char r, unsigned char g, unsigned char b );

	void FillSurface( SDL_Surface* source, unsigned char r, unsigned char g, unsigned char b );
	void FillSurface( SDL_Surface* source, const SDL_Color &color );

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
	SDL_Surface* localPlayerCaption;
	SDL_Surface* text;
	SDL_Surface* lives;
	SDL_Surface* points;
	SDL_Color textColor;

	SDL_Color tileColors[4];
	std::vector< SDL_Surface* > tileSurfaces;
};
