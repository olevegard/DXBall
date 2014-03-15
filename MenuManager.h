#pragma once

#include <map>
#include <memory>

#include "GameInfo.h"

#include "enums/GameState.h"
#include "enums/LobbyState.h"
#include "enums/LobbyMenuItem.h"
#include "enums/MainMenuItemType.h"
#include "enums/PauseMenuItemType.h"

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

	bool CheckItemMouseClick( int x, int y);

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
private:
	MainMenuItemType CheckIntersections( int x, int y );
	PauseMenuItemType CheckIntersections_Pause( int x, int y );
	LobbyMenuItem CheckIntersections_Lobby( int x, int y );

	void RemoevAllUnderscores( );

	GameState currentGameState;
	GameState prevGameState;
	bool hasGameStateChanged;
	bool isTwoPlayerMode;

	// Main menu
	std::map< MainMenuItemType, std::shared_ptr< MenuItem > > mainMenuItems;

	// Pause
	std::map< PauseMenuItemType, std::shared_ptr< MenuItem > > pauseMenuItems;

	// Lobby
	std::map< LobbyMenuItem, std::shared_ptr< MenuItem > > lobbyMenuItems;
	std::shared_ptr <MenuList > lobbyGameList;

	int32_t seletedGameID;
	LobbyMenuItem lobbyState;
	bool lobbyStateChanged;

	Logger* logger;
};
