// ==========================================
// || Renderer.h - Ole Vegard Mythe Moland ||
// ==========================================
// A rather large class focusd on rendering objects.
// It creates the windows, a renderer.
// It also creates ( or loads ) all textures including fonts
// All rendering happens within this class.
#pragma once

#include <memory>

#include "enums/GameState.h"
#include "enums/LobbyMenuItem.h"
#include "enums/ConfigValueType.h"
#include "enums/PauseMenuItemType.h"

#include "structs/rendering/Particle.h"
#include "structs/rendering/RenderingItem.h"

#include "structs/menu_items/ConfigItem.h"
#include "structs/menu_items/ConfigList.h"

#include "ColorConfigLoader.h"
#include "GameInfo.h"

// Forward declarations
struct Ball;
struct Tile;
struct Bullet;
struct Paddle;
struct BonusBox;
struct MenuList;
struct MainMenuItem;;
struct PauseMenuItem;
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
	void UpdateTileHit( const std::shared_ptr< Tile >  &tile ) const;
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
	void RenderText( const std::string &textToRender, const Player &player, bool fade = false);

	void RenderPlayerCaption( const std::string textToRender, const Player &player  );
	void RenderBallCount( uint64_t ballCount , const Player &player );
	void RenderPoints   ( uint64_t pointCount, const Player &player );
	void RenderLives    ( uint64_t lifeCounr , const Player &player );

	void ResetText();
	void StartFade();


	// Menu Items
	// ========================================================================

	// Main menu
	void AddMainMenuButtons(
			const std::string &singlePlayerString,
			const std::string &multiplayerString,
			const std::string &optionsString,
			const std::string &quitString
		);
	const std::shared_ptr< MenuItem > &GetMainMenuItem( const MainMenuItemType &type ) const;

	// Pause menu
	void AddPauseMenuButtons(
			const std::string &resumeString,
			const std::string &mainMenuString,
			const std::string &quitString
		);
	const std::shared_ptr< MenuItem > &GetPauseMenuItem( const PauseMenuItemType &type ) const;

	// Lobby Menu
	void AddLobbyMenuButtons(
			const std::string &newGame,
			const std::string &updaete,
			const std::string &back
		);
	const std::shared_ptr< MenuItem > &GetLobbyMenuItem( const LobbyMenuItem &type ) const;

	// GameList
	void InitGameList();
	const std::shared_ptr< MenuList >  &GetGameList();

	void AddGameToList( GameInfo gameInfo );
	void ClearGameList();



	const std::shared_ptr< ConfigList> &GetConfigList()
	{
		return configList;
	}
	SDL_Color GetTileColor( std::shared_ptr< Tile > tile  ) const;
	SDL_Color GetTileColor( uint64_t type ) const;
	SDL_Color GetHardTileColor( uint64_t index ) const;

	void InitConfigList();
	std::shared_ptr< MenuItem > GetOptionsBackButton()
	{
		return backToMenuButton;
	}
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

	// Rendering
	// ========================================================================

	// Game objects
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

	void CalculateLocalPlayerTextureRects();
	void CalculateRemotePlayerTextureRects();

	SDL_Rect CalcMenuListRect();
	void CenterLobbyButtons( );
	void CenterPauseButtons( );
	void CenterMainMenuButtons( );
	void CenterOptionsButtons( );

	void SetUnderlineHelper( const std::shared_ptr< MenuItem > &menuItem );
	void UpdateConfigValue ( const std::shared_ptr< ConfigItem > &item );
	void InitGreyAreaRect( );
	void AddMainMenuButton( const std::string &singlePlayerString, const MainMenuItemType &mit );
	std::shared_ptr< MenuItem > AddMenuButtonHelper( std::string menuItemString, const SDL_Rect &rect, TTF_Font* font );
	void AddOptionsButtonHelper( std::string caption, ConfigValueType type );

	void CleanUp();
	void CleanUpSurfaces();
	void CleanUpLists();
	void CleanUpTTF();
	void QuitSDL();

	ColorConfigLoader colorConfig;

	SDL_Window* window;
	SDL_Renderer* renderer;

	GameState gameState;

	SDL_Rect background;
	unsigned int screenFlags;

	bool isFullscreen;
	bool isTwoPlayerMode;

	std::vector< std::shared_ptr< Ball >  > ballList;
	std::vector< std::shared_ptr< Tile >  > tileList;
	std::vector< std::shared_ptr< BonusBox > > bonusBoxList;
	std::vector< std::shared_ptr< Bullet > > bulletList;

	std::shared_ptr< Paddle >  localPaddle;
	std::shared_ptr< Paddle >  remotePaddle;

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

	// Main info text...
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

	SDL_Texture*   mainMenuBackground;


	// Menu / Options / Lobby
	// =============================================
	RenderingItem< std::string > mainMenuCaption;
	RenderingItem< std::string > mainMenuSubCaption;
	RenderingItem< std::string > greyArea;

	// Main menu mode
	// =============================================
	std::map< MainMenuItemType, std::shared_ptr< MenuItem > > mainMenuItems;
	std::shared_ptr< MenuItem > singlePlayerButton;
	std::shared_ptr< MenuItem > multiPlayerButton;
	std::shared_ptr< MenuItem > optionsButton;
	std::shared_ptr< MenuItem > quitButton;

	// Options Mode
	// =============================================
	std::shared_ptr< ConfigList> configList;
	std::shared_ptr< MenuItem > backToMenuButton;

	// Pause menu mode
	// =============================================
	std::shared_ptr< MenuItem > pauseResumeButton;
	std::shared_ptr< MenuItem > pauseMainMenuButton;
	std::shared_ptr< MenuItem > pauseQuitButton;

	// MultiplayerButton menu item
	// =============================================
	std::shared_ptr< MenuItem > lobbyNewGameButton;
	std::shared_ptr< MenuItem > lobbyUpdateButton;
	std::shared_ptr< MenuItem > lobbyBackButton;
	std::shared_ptr< MenuList>  gameList;
	SDL_Rect	lobbyMenuListRect;

	// Bonus Boxes
	std::map< BonusType, SDL_Color > bonusTypeColors;

	std::vector< Particle > particles;
};
