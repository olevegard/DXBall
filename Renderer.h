#pragma once


#include <vector>
#include <memory>
#include <map>

#include "GamePiece.h"
#include <SDL2/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL2/SDL_ttf.h>

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


	bool Init( const SDL_Rect &r, bool startFS);

	void ToggleFullscreen();
	bool SetFullscreen( bool fullscreenOn );

	void AddTile( const std::shared_ptr< Tile > &tile );
	void RemoveTile( const std::shared_ptr< Tile >  &tile );

	void AddBall( const std::shared_ptr< Ball > &ball );
	void RemoveBall( const std::shared_ptr< Ball >  &ball );

	void SetLocalPaddle( std::shared_ptr< Paddle >  &paddle );
	void SetRemotePaddle( std::shared_ptr< Paddle >  &paddle );

	void Render( );

	void RenderText( const std::string &textToRender, const Player &player );
	void RemoveText();

	void RenderPlayerCaption( const std::string textToRender, const Player &player  );
	void RenderLives( unsigned long lifeCount, const Player &player  );
	void RenderPoints( unsigned long pointCount, const Player &player  );

	void ForceMouseFocus();
private:
	Renderer( const Renderer &renderer );
	Renderer& operator=( const Renderer &renderer );

	void Setup();
	bool CreateRenderer();

	bool CreateWindow();
	void SetFlags_VideoMode();

	void RenderForeground();
	void RenderText();

	void FillSurface( SDL_Surface* source, unsigned char r, unsigned char g, unsigned char b ) const;
	void FillSurface( SDL_Surface* source, const SDL_Color &color ) const;

	SDL_Texture* InitSurface( const Rect &rect     , unsigned char r, unsigned char g, unsigned char b ) const;
	SDL_Texture* InitSurface( int width, int height, unsigned char r, unsigned char g, unsigned char b ) const;

	SDL_Surface* SetDisplayFormat( SDL_Surface* surface ) const;

	void SetTileColorSurface( size_t index, const SDL_Color &color, std::vector< SDL_Texture* > &list );

	bool LoadImages();

	void PrintSDL_TTFVersion();

	SDL_Texture* LoadImage( const std::string &filename, GamePiece::TextureType textureType );
	TTF_Font* LoadFont( const std::string &fontname, int fontSize ) const;

	void SetArrayData( GamePiece::TextureType textureType, SDL_Texture* surface );

	bool LoadFontAndText();
	SDL_Texture* RenderTextTexture_Solid(  TTF_Font* font, const std::string &text, const SDL_Color &color, SDL_Rect &rect );

	void CleanUp();
	void CleanUpSurfaces();
	void CleanUpLists();
	void CleanUpTTF();
	void QuitSDL();

	SDL_Window* window;
	SDL_Renderer* renderer;

	unsigned int rmask, gmask, bmask, amask;

	SDL_Rect background;
	const int SCREEN_BPP;
	unsigned int screenFlags;
	bool fullscreen;

	SDL_Color tileColors[4];
	std::vector< SDL_Texture* > tileTextures;

	SDL_Color hardTileColors[5];
	std::vector< SDL_Texture* > hardTileTextures;

	std::vector< std::shared_ptr< Ball >  > ballList;
	std::vector< std::shared_ptr< Tile >  > tileList;
	std::shared_ptr< Paddle >  localPaddle;
	std::shared_ptr< Paddle >  remotePaddle;

	std::map< int, SDL_Texture* > textures;
	SDL_Texture* remotePlayerBall;

	// Text
	// =============================================
	TTF_Font* font;
	TTF_Font* bigFont;

	SDL_Color textColor;

	// main info text...
	SDL_Texture* localPlayerTextTexture;
	SDL_Rect     localPlayerTextRect;
	std::string  localPlayerTextValue;

	// Player name
	SDL_Texture* localPlayerCaptionTexture;
	SDL_Rect     localPlayerCaptionRect;
	std::string  localPlayerCaptionValue;

	// lives
	SDL_Texture*   localPlayerLivesTexture;
	SDL_Rect       localPlayerLivesRect;
	unsigned long  localPlayerLivesValue;

	// points
	SDL_Texture*   localPlayerPointsTexture;
	SDL_Rect       localPlayerPointsRect;
	unsigned long  localPlayerPointsValue;

	//SDL_Surface* remotePlayerLives;
	//SDL_Surface* remotePlayerText;
	//SDL_Surface* remotePlayerPoints;
	//SDL_Surface* remotePlayerCaption;
};
