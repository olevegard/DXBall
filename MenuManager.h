#pragma once

#include "MenuList.h"
#include "GameInfo.h"

#include "structs/menu_items/MenuItem.h"

#include "enums/GameState.h"
#include "enums/LobbyState.h"
#include "enums/LobbyMenuItem.h"
#include "enums/MainMenuItemType.h"
#include "enums/PauseMenuItemType.h"

class Renderer;
class MenuManager
{
public:
	MenuManager();

	void CheckItemMouseOver( int x, int y, Renderer &renderer ) const;
	void CheckItemMouseOver_Pause( int x, int y, Renderer &renderer ) const;
	void CheckItemMouseOver_MainMenu( int x, int y, Renderer &renderer ) const;
	void CheckItemMouseOver_Lobby( int x, int y, Renderer &renderer ) const;

	bool CheckItemMouseClick( int x, int y);
	//bool CheckItemMouseClick_Pause( int x, int y);

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

	void Init( Renderer &renderer );

	void AddGameToList( Renderer &renderer, GameInfo gameInfo )
	{
		lobbyGameList->AddItem( gameInfo, renderer );
	}
	void ClearGameList()
	{
		lobbyGameList->ClearList();
	}
	int32_t GetSelectedGame() const
	{
		return seletedGameID;
	}
	bool IsAnItemSelected() const
	{
		return ( seletedGameID >= 0 );
	}
	GameInfo GetSelectedGameInfo() const
	{
		return lobbyGameList->GetGameInfoForIndex( seletedGameID);;
	}

	void SetMainMenuItem( const MainMenuItemType &type, const std::shared_ptr< MenuItem >& button );
	void SetLobbyMenuItem( const LobbyMenuItem &type, const std::shared_ptr< MenuItem >  &button );
	void SetPauseMenuItem( const PauseMenuItemType &type, const std::shared_ptr< MenuItem >  &button );

private:
	MainMenuItemType CheckIntersections( int x, int y ) const;
	PauseMenuItemType CheckIntersections_Pause( int x, int y ) const;
	LobbyMenuItem CheckIntersections_Lobby( int x, int y ) const;

	void RemoevAllUnderscores( Renderer &renderer  );

	GameState currentGameState;
	GameState prevGameState;
	bool hasGameStateChanged;
	bool isTwoPlayerMode;

	// Main menu
	std::shared_ptr< MenuItem > singlePlayerButton;
	std::shared_ptr< MenuItem > multiPlayerButton;
	std::shared_ptr< MenuItem > optionsButton;
	std::shared_ptr< MenuItem > quitButton;

	// Pause
	std::shared_ptr< MenuItem > pauseResumeButton;
	std::shared_ptr< MenuItem > pauseMainMenuButton;
	std::shared_ptr< MenuItem > pauseQuitButton;

	// Lobby
	std::shared_ptr< MenuItem > lobbyNewGameButton;
	std::shared_ptr< MenuItem > lobbyUpdateButton;
	std::shared_ptr< MenuItem > lobbyBackButton;

	std::shared_ptr <MenuList > lobbyGameList;
	int32_t seletedGameID;
	LobbyMenuItem lobbyState;
	bool lobbyStateChanged;
};
