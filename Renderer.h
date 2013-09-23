#pragma once

#include <vector>
#include <map>

#include <memory>

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "GamePiece.h"
#include "BonusBox.h"

#include "enums/GameState.h"

// Forward declarations
struct Ball;
struct Tile;
struct Paddle;
struct BonusBox;
enum class Player : int;

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
	void ClearBoard( );

	void AddBall( const std::shared_ptr< Ball > &ball );
	void RemoveBall( const std::shared_ptr< Ball >  &ball );

	void AddBonusBox( const std::shared_ptr< BonusBox > &bb );
	void RemoveBonusBox( const std::shared_ptr< BonusBox >  &bb );

	void SetLocalPaddle( std::shared_ptr< Paddle >  &paddle );
	void SetRemotePaddle( std::shared_ptr< Paddle >  &paddle );

	void Render( );

	void RenderText( const std::string &textToRender, const Player &player );
	void RemoveText();

	void RenderPlayerCaption( const std::string textToRender, const Player &player  );
	void RenderLives( unsigned long lifeCount, const Player &player  );
	void RenderPoints( unsigned long pointCount, const Player &player  );
	void RenderBallCount( unsigned long ballCount, const Player &player  );

	void AddMenuButtons( const std::string &singlePlayerString, const std::string &multiplayerString, const std::string &optionsString, const std::string &quitString );
	void AddSinglePlayerButton( const std::string &singlePlayerString );
	void AddMultiplayerButton( const std::string &multiplayerString );
	void AddOptionsButton( const std::string &optionsString );
	void AddQuitButton( const std::string &quitString );
	void CenterMenuButtons( );
	void InitGreyAreaRect( );

	void ForceMouseFocus();


	void SetGameState( const GameState &gs );
	void HideMouseCursor( bool hideCursor );
private:
	Renderer( const Renderer &renderer );
	Renderer& operator=( const Renderer &renderer );

	void CreateBonusBox( );

	void Setup();
	bool CreateRenderer();

	bool CreateWindow();
	void SetFlags_VideoMode();

	void RenderForeground();
	void RenderText();

	void RenderMenu();

	void FillSurface( SDL_Surface* source, unsigned char r, unsigned char g, unsigned char b ) const;
	void FillSurface( SDL_Surface* source, const SDL_Color &color ) const;

	SDL_Texture* InitSurface( const Rect &rect     , unsigned char r, unsigned char g, unsigned char b ) const;
	SDL_Texture* InitSurface( int width, int height, unsigned char r, unsigned char g, unsigned char b ) const;
	SDL_Texture* InitSurface( const Rect &rect     , const SDL_Color &clr ) const;
	SDL_Texture* InitSurface( int width, int height, const SDL_Color &clr ) const;

	SDL_Surface* SetDisplayFormat( SDL_Surface* surface ) const;

	void SetTileColorSurface( size_t index, const SDL_Color &color, std::vector< SDL_Texture* > &list );

	bool LoadImages();

	void PrintSDL_TTFVersion();

	SDL_Texture* LoadImage( const std::string &filename);
	TTF_Font* LoadFont( const std::string &fontname, int fontSize ) const;

	bool LoadFontAndText();
	SDL_Texture* RenderTextTexture_Solid  (  TTF_Font* font, const std::string &text, const SDL_Color &color, SDL_Rect &rect );
	SDL_Texture* RenderTextTexture_Blended(  TTF_Font* font, const std::string &text, const SDL_Color &color, SDL_Rect &rect );

	void CalculateRemotePlayerTextureRects();

	void CleanUp();
	void CleanUpSurfaces();
	void CleanUpLists();
	void CleanUpTTF();
	void QuitSDL();


	SDL_Window* window;
	SDL_Renderer* renderer;

	GameState gameState;

	unsigned int rmask;
	unsigned int gmask;
	unsigned int bmask;
	unsigned int amask;

	SDL_Rect background;
	const int SCREEN_BPP;
	unsigned int screenFlags;
	bool fullscreen;
	SDL_Color backgroundColor;

	SDL_Texture* bonusBoxTexture;
	SDL_Rect bonusBoxRect;

	SDL_Color tileColors[4];
	std::vector< SDL_Texture* > tileTextures;

	SDL_Color hardTileColors[5];
	std::vector< SDL_Texture* > hardTileTextures;

	std::vector< std::shared_ptr< Ball >  > ballList;
	std::vector< std::shared_ptr< Tile >  > tileList;
	std::vector< std::shared_ptr< BonusBox > > bonusBoxList;

	std::shared_ptr< Paddle >  localPaddle;
	std::shared_ptr< Paddle >  remotePaddle;

	SDL_Color    localPlayerColor;
	SDL_Texture* localPlayerBallTexture;
	SDL_Texture* localPlayerPaddle;

	SDL_Color    remotePlayerColor;
	SDL_Texture* remotePlayerBallTexture;
	SDL_Texture* remotePlayerPaddle;

	// Text
	// =============================================
	TTF_Font* tinyFont;
	TTF_Font* font;
	TTF_Font* mediumFont;
	TTF_Font* bigFont;
	TTF_Font* hugeFont;

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

	// balls
	SDL_Texture*   localPlayerBallsTexture;
	SDL_Rect       localPlayerBallsRect;
	unsigned long  localPlayerBallsValue;


	// Player name
	SDL_Texture* remotePlayerCaptionTexture;
	SDL_Rect     remotePlayerCaptionRect;
	std::string  remotePlayerCaptionValue;

	// lives
	SDL_Texture*   remotePlayerLivesTexture;
	SDL_Rect       remotePlayerLivesRect;
	unsigned long  remotePlayerLivesValue;

	// points
	SDL_Texture*   remotePlayerPointsTexture;
	SDL_Rect       remotePlayerPointsRect;
	unsigned long  remotePlayerPointsValue;

	// balls
	SDL_Texture*   remotePlayerBallsTexture;
	SDL_Rect       remotePlayerBallsRect;
	unsigned long  remotePlayerBallsValue;

	// Menu mode
	// =============================================
	short margin;
	SDL_Texture*   mainMenuBackground;

	SDL_Texture*   mainMenuCaptionTexture;
	SDL_Rect       mainMenuCaptionRect;

	SDL_Texture*   mainMenuSubCaptionTexture;
	SDL_Rect       mainMenuSubCaptionRect;

	SDL_Texture*   singlePlayerButtonTexture;
	SDL_Rect       singlePlayerButtonRect;

	SDL_Texture*   multiPlayerButtonTexture;
	SDL_Rect       multiPlayerButtonRect;

	SDL_Texture*   optionsButtonTexture;
	SDL_Rect       optionsButtonRect;

	SDL_Texture*   quitButtonTexture;
	SDL_Rect       quitButtonRect;

	SDL_Texture*   greyAreaTexture;
	SDL_Rect       greyAreaRect;
};
