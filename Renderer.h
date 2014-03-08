// ==========================================
// || Renderer.h - Ole Vegard Mythe Moland ||
// ==========================================
// A rather large class focusd on rendering objects.
// It creates the windows, a renderer.
// It also creates ( or loads ) all textures including fonts
// All rendering happens within this class.
#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "enums/GameState.h"

#include "structs/game_objects/BonusBox.h"
#include "structs/game_objects/Bullet.h"

#include "structs/menu_items/MainMenuItem.h"
#include "structs/menu_items/PauseMenuItem.h"

#include "structs/rendering/Particle.h"
#include "structs/rendering/RenderingItem.h"

#include "ColorConfigLoader.h"

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

	void AddBullet( const std::shared_ptr< Bullet > &bb );
	void RemoveBullet( const std::shared_ptr< Bullet >  &bb );

	void SetLocalPaddle( std::shared_ptr< Paddle >  &paddle );
	void SetRemotePaddle( std::shared_ptr< Paddle >  &paddle );

	void Render( );
	void Update( double delta );

	void SetGameState( const GameState &gs );
	void SetIsTwoPlayerMode( bool isTwoPlayerMode_ );

	void GenerateParticleEffect( std::shared_ptr< Tile > tile );

	// Ingame text
	void ResetAlpha();
	void StartFade()
	{
		localPlayerText.StartFade();
	}

	void RenderText( const std::string &textToRender, const Player &player, bool fade = false);

	void RenderPlayerCaption( const std::string textToRender, const Player &player  );
	void RenderBallCount( uint64_t ballCount , const Player &player );
	void RenderPoints   ( uint64_t pointCount, const Player &player );
	void RenderLives    ( uint64_t lifeCounr , const Player &player );

	void RemoveText();
	void ResetText();


	// Main menu
	// ========================================================================
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

	void AddMenuList( MenuList* mitem );

	SDL_Renderer* GetRenderer() const;
	TTF_Font* GetFont() const;
	SDL_Color GetBackgroundColor() const;
	SDL_Color GetTextColor() const;
	SDL_Color GetTileColor( std::shared_ptr< Tile > tile  );
	SDL_Color GetTileColor( uint64_t type );
	SDL_Color GetHardTileColor( uint64_t index );

	SDL_Rect CalcMenuListRect();
private:
	Renderer( const Renderer &renderer );
	Renderer& operator=( const Renderer &renderer );

	SDL_Texture* CreateBonusBoxTexture( const std::shared_ptr< BonusBox >  &bb );
	SDL_Color GetBonusBoxColor( const BonusType &bonusType );

	void Setup();
	bool CreateRenderer();
	bool InitSDLSubSystems() const;

	bool CreateWindow( bool server );
	void SetFlags_VideoMode();

	// Rende Game Objects
	void RenderGameObjects();

	void RenderText();
	void RenderBalls();
	void RenderTiles();
	void RenderPaddles();
	void RenderBullets();
	void RenderBonusBoxes();

	void RenderParticles();

	// Render main menu
	void RenderMenu();
	void RenderLobbyFooter();
	void RenderMainMenuFooter();

	void RenderPause();

	bool LoadAssets();
	void LoadColors();
	bool InitializeTextures();
	void InitializeMainMenuTextures();

	void PrintSDL_TTFVersion();

	TTF_Font* LoadFont( const std::string &fontname, int fontSize ) const;

	bool LoadFontAndText();

	void CalculateRemotePlayerTextureRects();

	void CleanUp();
	void CleanUpSurfaces();
	void CleanUpLists();
	void CleanUpTTF();
	void QuitSDL();

	ColorConfigLoader colorConfig;

	MenuList* ml;

	SDL_Window* window;
	SDL_Renderer* renderer;

	GameState gameState;

	SDL_Rect background;
	unsigned int screenFlags;
	bool fullscreen;

	std::vector< std::shared_ptr< Ball >  > ballList;
	std::vector< std::shared_ptr< Tile >  > tileList;
	std::vector< std::shared_ptr< BonusBox > > bonusBoxList;
	std::vector< std::shared_ptr< Bullet > > bulletList;

	std::shared_ptr< Paddle >  localPaddle;
	std::shared_ptr< Paddle >  remotePaddle;

	bool isTwoPlayerMode;
	SDL_Texture* localPlayerBallTexture;
	SDL_Texture* localPlayerPaddle;

	SDL_Texture* remotePlayerBallTexture;
	SDL_Texture* remotePlayerPaddle;

	std::vector< SDL_Texture* > tileTextures;
	std::vector< SDL_Texture* > hardTileTextures;

	// Text
	// =============================================
	TTF_Font* tinyFont;
	TTF_Font* font;
	TTF_Font* mediumFont;
	TTF_Font* bigFont;
	TTF_Font* hugeFont;

	// main info text...
	RenderingItem< std::string > localPlayerText;

	// Player name
	RenderingItem< std::string > localPlayerCaption;
	RenderingItem< std::string > remotePlayerCaption;

	// Lives
	RenderingItem< uint64_t > localPlayerLives;
	RenderingItem< uint64_t > remotePlayerLives;

	// Points
	RenderingItem< uint64_t > localPlayerPoints;
	RenderingItem< uint64_t > remotePlayerPoints;

	// Balls
	RenderingItem< uint64_t > localPlayerBalls;
	RenderingItem< uint64_t > remotePlayerBalls;

	short margin;

	// Main menu mode
	// =============================================
	SDL_Texture*   mainMenuBackground;

	MenuItem	singlePlayerText;
	MenuItem	multiplayerPlayerText;
	MenuItem	optionsButton;
	MenuItem	quitButton;

	RenderingItem< std::string > mainMenuCaption;
	RenderingItem< std::string > mainMenuSubCaption;
	RenderingItem< std::string > greyArea;

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

	// Bonus Boxes
	std::map< BonusType, SDL_Color > bonusTypeColors;

	std::vector< Particle > particles;
};
