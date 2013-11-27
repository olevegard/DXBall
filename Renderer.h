// ==========================================
// || Renderer.h - Ole Vegard Mythe Moland ||
// ==========================================
// A rather large class focusd on rendering objects.
// It creates the windows, a renderer.
// It also creates ( or loads ) all textures including fonts
// All rendering happens within this class.

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

#include "MainMenuItem.h"
#include "PauseMenuItem.h"
#include "MenuList.h"

// Forward declarations
struct Ball;
struct Tile;
struct Paddle;
struct BonusBox;
enum class Player : int;
enum class BonusType : int;
enum class MenuItemType : int;
enum class LobbyMenuItem : int;
enum class MainMenuItemType : int;

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Init( const SDL_Rect &r, bool startFS, bool isServer);

	void ToggleFullscreen();
	bool SetFullscreen( bool fullscreenOn );

	void AddTile( const std::shared_ptr< Tile > &tile );
	void RemoveTile( const std::shared_ptr< Tile >  &tile );
	void ClearBoard( );

	void AddBall( const std::shared_ptr< Ball > &ball );
	void RemoveBall( const std::shared_ptr< Ball >  &ball );

	void AddBonusBox( const std::shared_ptr< BonusBox > &bb );
	void RemoveBonusBox( const std::shared_ptr< BonusBox >  &bb );
	void SetBonusBoxIcon( int32_t width, SDL_Surface* bonusBox, const BonusType &bonusType );
	SDL_Color GetBonusBoxColor( const BonusType &bonusType );

	void SetLocalPaddle( std::shared_ptr< Paddle >  &paddle );
	void SetRemotePaddle( std::shared_ptr< Paddle >  &paddle );

	void Render( );

	// Ingame text
	void RenderText( const std::string &textToRender, const Player &player, bool fade = false);
	void RemoveText();

	void RenderPlayerCaption( const std::string textToRender, const Player &player  );
	void RenderLives( unsigned long lifeCount, const Player &player, bool force = false   );
	void RenderPoints( unsigned long pointCount, const Player &player, bool force = false   );
	void RenderBallCount( unsigned long ballCount, const Player &player, bool force = false  );
	void ResetText();

	// Main menu
	void AddMainMenuButtons( const std::string &singlePlayerString, const std::string &multiplayerString, const std::string &optionsString, const std::string &quitString );
	void AddMainMenuButton( const std::string &singlePlayerString, const MainMenuItemType &mit );
	MenuItem AddMenuButtonHelper( MenuItem mainmenuItem, std::string menuItemStringconst, const SDL_Rect &singlePlayerRect  );

	void SetMainMenuItemUnderline( bool setUnderline, const MainMenuItemType &mit  );
	MenuItem SetUnderlineHelper( MenuItem menuItem, bool setUnderline );
	void RemoveMainMenuItemsUnderlines( );
	void CenterMainMenuButtons( );
	void InitGreyAreaRect( );
	SDL_Rect GetSinglePlayerRect() const;
	SDL_Rect GetMultiplayerPlayerRect() const;
	SDL_Rect GetOptionsPlayerRect() const;
	SDL_Rect GetQuitPlayerRect() const;

	// Pause menu
	void AddPauseMenuButtons( const std::string &resumeString, const std::string &mainMenuString, const std::string &quitString );
	void CenterPauseButtons( );
	void SetMainMenuItemUnderline( bool setUnderline, const PauseMenuItemType &mit  );
	SDL_Rect GetPauseResumeRect() const;
	SDL_Rect GetPauseMainMenuRect() const;
	SDL_Rect GetPauseQuitRect() const;

	// Lobby Menu
	void AddLobbyMenuButtons( const std::string &newGame, const std::string &updaete, const std::string &back );
	void CenterLobbyButtons( );
	void SetLobbyItemUnderline( bool setUnderline, const LobbyMenuItem &mit  );
	SDL_Rect GetLobbyNewGameRect() const;
	SDL_Rect GetLobbyUpdateRect() const;
	SDL_Rect GetLobbyBackRect() const;

	void ForceMouseFocus();

	void SetGameState( const GameState &gs );
	void HideMouseCursor( bool hideCursor );

	void SetIsTwoPlayerMode( bool isTwoPlayerMode_ )
	{
		isTwoPlayerMode = isTwoPlayerMode_;
	}

	SDL_Renderer* GetRenderer() const
	{
		return renderer;
	}

	TTF_Font* GetFont() const
	{
		return font;
	}

	SDL_Color GetBackgroundColor() const
	{
		return backgroundColor;
	}

	SDL_Color GetTextColor() const
	{
		return textColor;
	}

	void AddMenuList( MenuList* mitem )
	{
		ml = mitem;
	}
	SDL_Rect CalcMenuListRect()
	{
		lobbyMenuListRect.w = static_cast< int32_t > ( greyAreaRect.w * 0.6 );
		lobbyMenuListRect.h = static_cast< int32_t > ( greyAreaRect.h * 0.9 );

		lobbyMenuListRect.x = static_cast< int32_t > ( ( background.w * 0.5   ) - ( lobbyMenuListRect.w * 0.5 ) );
		lobbyMenuListRect.y = static_cast< int32_t > ( ( greyAreaRect.h * 0.5 ) - ( lobbyMenuListRect.h * 0.5 ) ) + greyAreaRect.y;

		std::cout << "AddLobbyMenuButtons.w set to : " << lobbyMenuListRect.w << std::endl;
		return lobbyMenuListRect;
	}
	void Update( double delta )
	{
		if ( !localPlayerTextFade )
			return;

		if ( localPlayerTextAlpha  > 0.0 )
		{
			localPlayerTextAlpha -= ( delta * 0.001 );

			if ( localPlayerTextAlpha < 0.0 )
			{
				localPlayerTextAlpha = 1.0;
				localPlayerTextFade = false;
			} else
			{
				Uint8 alpha  =  static_cast< Uint8 > ( 255 * localPlayerTextAlpha );
				SDL_SetTextureAlphaMod( localPlayerTextTexture, alpha );
			}
		}
	}
	void ResetAlpha()
	{
		localPlayerTextAlpha = 1.0;
		SDL_SetTextureAlphaMod( localPlayerTextTexture, 255 );
	}
	void StartFade()
	{
		localPlayerTextFade = true;
	}

private:
	Renderer( const Renderer &renderer );
	Renderer& operator=( const Renderer &renderer );

	void Setup();
	bool CreateRenderer();

	bool CreateWindow( bool server );
	void SetFlags_VideoMode();

	void RenderForeground();
	void RenderText();

	void RenderLobby();

	// Render main menu
	void RenderMainMenuHeader();
	void RenderMainMenuImage();
	void RenderMainMenuFooter();
	void RenderMenuItem( const MenuItem &menuItem ) const;

	SDL_Surface* SetDisplayFormat( SDL_Surface* surface ) const;

	void SetTileColorSurface( size_t index, const SDL_Color &color, std::vector< SDL_Texture* > &list );

	bool LoadImages();
	void LoadColors();

	void PrintSDL_TTFVersion();

	TTF_Font* LoadFont( const std::string &fontname, int fontSize ) const;

	bool LoadFontAndText();

	void CalculateRemotePlayerTextureRects();

	void CleanUp();
	void CleanUpSurfaces();
	void CleanUpLists();
	void CleanUpTTF();
	void QuitSDL();

	MenuList* ml;

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

	bool isTwoPlayerMode;
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
	SDL_Color greyAreaColor;

	// main info text...
	SDL_Texture* localPlayerTextTexture;
	SDL_Rect     localPlayerTextRect;
	std::string  localPlayerTextValue;
	double 		 localPlayerTextAlpha;
	bool 		 localPlayerTextFade;

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

	short margin;

	// Main menu mode
	// =============================================
	SDL_Texture*   mainMenuBackground;

	MenuItem	singlePlayerText;
	MenuItem	multiplayerPlayerText;
	MenuItem	optionsButton;
	MenuItem	quitButton;

	SDL_Texture*   mainMenuCaptionTexture;
	SDL_Rect       mainMenuCaptionRect;

	SDL_Texture*   mainMenuSubCaptionTexture;
	SDL_Rect       mainMenuSubCaptionRect;

	SDL_Texture*   greyAreaTexture;
	SDL_Rect       greyAreaRect;

	// Pause menu mode
	// =============================================
	MenuItem 	pauseResumeButton;
	MenuItem 	pauseMainMenuButton;
	MenuItem 	pauseQuitButton;

	// Multiplayer menu item
	// =============================================
	MenuItem 	lobbyNewGameButton;
	MenuItem 	lobbyUpdateButton;
	MenuItem 	lobbyBackButton;
	SDL_Rect	lobbyMenuListRect;
};
