#pragma once

#include <SDL/SDL_ttf.h>

#include <sstream>
#include <vector>
#include <memory>
#include <map>

#include "GamePiece.h"


#include <iostream>

// Forward declarations
struct Ball;
struct Tile;
struct Paddle;

	enum class Player
	{
		Local,
		Remote
	};

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

	void RenderText( const std::string &textToRender, const Player &player );
	void RenderLives( unsigned long lifeCount, const Player &player  );
	void RenderPoints( unsigned int pointCount, const Player &player  );
	void RenderPlayerCaption( const std::string textToRender, const Player &player  );

	void RemoveText();

	Rect GetTileSize();
	Rect GetWindowSize();

	void ToggleFullscreen();
private:

	Renderer( const Renderer &renderer );
	Renderer& operator=( const Renderer &renderer );

	bool SetVideoMode( bool fullscreenOn );
	bool ApplyVideoMode();
	void SetFlags_VideoMode();

	SDL_Surface* LoadImage( const std::string &filename, GamePiece::TextureType textureType );
	TTF_Font* LoadFont( const std::string &fontname, int fontSize ) const;

	void SetColorKey( SDL_Surface* source             , unsigned char r, unsigned char g, unsigned char b );
	void SetColorKey( GamePiece::TextureType textureID, unsigned char r, unsigned char g, unsigned char b );

	void FillSurface( SDL_Surface* source, unsigned char r, unsigned char g, unsigned char b );
	void FillSurface( SDL_Surface* source, const SDL_Color &color );

	void SetTileColorSurface( size_t index, const SDL_Color &color, std::vector< SDL_Surface* > &list );

	SDL_Surface* RenderTextSurface_Solid(  TTF_Font* font, const std::string &text, const SDL_Color &color );

	SDL_Surface* InitSurface( const Rect &rect ) const;
	SDL_Surface* InitSurface( unsigned int flags, int width, int height ) const;
	SDL_Surface* InitSurface(GamePiece::TextureType textureType, unsigned int flags, int width, int height );

	void SetArrayData( GamePiece::TextureType textureType, SDL_Surface* surface );

	SDL_Surface* SetDisplayFormat( SDL_Surface* surface ) const;

	void ApplySurface( short x, short y, SDL_Surface* source, SDL_Surface* destination ) const;
	void ApplySurface( const Rect &r, SDL_Surface* source, SDL_Surface* destination ) const;

	bool LoadImages();
	bool LoadFontAndText();

	void BlitBackground() const;
	void BlitForeground();
	void BlitText();

	void CleanUp();
	void CleanUpSurfaces();
	void CleanUpLists();
	void CleanUpTTF();
	void QuitSDL();

	Uint32 rmask, gmask, bmask, amask;

	const int SCREEN_WIDTH;
	const int SCREEN_HEIGHT;
	const int SCREEN_BPP;
	Uint32 screenFlags;
	bool fullscreen;

	std::map< int, SDL_Surface* > textures;

	SDL_Surface *backgroundArea;
	SDL_Surface *backgroundImage;
	SDL_Surface *screen;

	std::vector< std::shared_ptr< Ball >  > ballList;
	std::vector< std::shared_ptr< Tile >  > tileList;

	std::shared_ptr< Paddle >  localPaddle;

	Rect background;

	// Text
	TTF_Font* font;
	TTF_Font* bigFont;

	SDL_Color textColor;

	// Local player surfaces
	SDL_Surface* localPlayerText;
	SDL_Surface* localPlayerLives;
	SDL_Surface* localPlayerPoints;
	SDL_Surface* localPlayerCaption;

	SDL_Surface* remotePlayerText;
	SDL_Surface* remotePlayerLives;
	SDL_Surface* remotePlayerPoints;
	SDL_Surface* remotePlayerCaption;

	SDL_Color tileColors[4];
	std::vector< SDL_Surface* > tileSurfaces;

	SDL_Color hardTileColors[5];
	std::vector< SDL_Surface* > hardTileSurfaces;

};
