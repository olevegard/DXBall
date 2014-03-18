#pragma once

#include <map>
#include <memory>

#include "GameInfo.h"

#include "enums/GameState.h"
#include "enums/LobbyState.h"
#include "enums/LobbyMenuItem.h"
#include "enums/MainMenuItemType.h"
#include "enums/PauseMenuItemType.h"

#include "structs/menu_items/ConfigChange.h"
#include "structs/menu_items/ConfigItem.h"

class Logger;
struct MenuList;
struct MenuItem;
class MenuManager
{
public:
	MenuManager();

	void CheckItemMouseOver( int x, int y );
	void CheckItemMouseOver_Pause( int x, int y );
	void CheckItemMouseOver_MainMenu( int x, int y );
	void CheckItemMouseOver_Lobby( int x, int y );
	void CheckItemMouseOver_Options( int x, int y );

	void CheckItemMouseClick( int x, int y);

	GameState GetGameState() const;
	void SetGameState( const GameState &gs );
	bool IsGameStateChangeValid( const GameState &gs) const;

	GameState GoBackToPreviousMenuState();
	GameState GoToMenu();
	LobbyMenuItem GetLobbyState();

	GameState GetPrevGameState() const;
	bool HasGameStateChanged();
	bool HasLobbyStateChanged();

	bool WasGameStarted() const;
	bool WasGameQuited() const;
	bool WasGameResumed() const;

	bool IsTwoPlayerMode() const;
	bool IsInAMenu() const;

	int32_t GetSelectedGame() const;
	bool IsAnItemSelected() const;
	GameInfo GetSelectedGameInfo() const;

	void SetMainMenuItem( const MainMenuItemType &type, const std::shared_ptr< MenuItem >& button );
	void SetLobbyMenuItem( const LobbyMenuItem &type, const std::shared_ptr< MenuItem >  &button );
	void SetPauseMenuItem( const PauseMenuItemType &type, const std::shared_ptr< MenuItem >  &button );
	void SetGameList( const std::shared_ptr< MenuList >  gameList_ )
	{
		lobbyGameList = gameList_;
	}
	void SetOptionsMenuItem( const std::shared_ptr< MenuItem >  &button )
	{
		backToMenuButton = button;
	}
	void SetBallSpeed( const std::shared_ptr< ConfigItem >  &button )
	{
		ballSpeedSetter = button;
	}
private:
	MainMenuItemType CheckIntersections( int x, int y );
	PauseMenuItemType CheckIntersections_Pause( int x, int y );
	LobbyMenuItem CheckIntersections_Lobby( int x, int y );

	GameState currentGameState;
	GameState prevGameState;
	bool hasGameStateChanged;
	bool hasLobbyStateChanged;
	bool isTwoPlayerMode;

	// Main menu
	std::map< MainMenuItemType, std::shared_ptr< MenuItem > > mainMenuItems;

	// Pause
	std::map< PauseMenuItemType, std::shared_ptr< MenuItem > > pauseMenuItems;

	// Lobby
	std::map< LobbyMenuItem, std::shared_ptr< MenuItem > > lobbyMenuItems;
	std::shared_ptr <MenuList > lobbyGameList;

	// Options
	std::shared_ptr< ConfigItem > ballSpeedSetter;
	//std::map< ConfigItem, std::shared_ptr< ConfigItem > > configItems;
	std::shared_ptr< MenuItem > backToMenuButton;

	int32_t seletedGameID;
	LobbyMenuItem lobbyState;

	Logger* logger;
};
