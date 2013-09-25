#pragma once

#include "MenuItem.h"

#include "enums/GameState.h"
#include "enums/MainMenuItemType.h"
#include "enums/PauseMenuItemType.h"

class Renderer;
class MenuManager
{
public:
	MenuManager();

	//GameState currentState;
	void AddMenuElememts( Renderer &renderer );
	void AddPauseMenuElememts( Renderer &renderer );

	void CheckItemMouseOver( int x, int y, Renderer &renderer );
	void CheckItemMouseOver_Pause( int x, int y, Renderer &renderer );

	bool CheckItemMouseClick( int x, int y);
	//bool CheckItemMouseClick_Pause( int x, int y);

	GameState GetGameState() const;
	void SetGameState( const GameState &gs );
	bool IsGameStateChangeValid( const GameState &gs) const;

	GameState GetPrevGameState() const;

	bool HasGameStateChanged();

	GameState GoBackToPreviousMenuState();
	GameState GoToMenu();
private:
	MainMenuItemType CheckIntersections( int x, int y );
	PauseMenuItemType CheckIntersections_Pause( int x, int y );

	void RemoevAllUnderscores( Renderer &renderer  );

	GameState currentGameState;
	GameState prevGameState;
	bool hasGameStateChanged;

	MenuItem singlePlayer;
	MenuItem multiPlayer;
	MenuItem options;
	MenuItem quit;

	MenuItem pauseResumeButton;
	MenuItem pauseMainMenuButton;
	MenuItem pauseQuitButton;
};
