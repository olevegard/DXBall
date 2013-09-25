#pragma once

#include "MainMenuItem.h"

#include "enums/GameState.h"
#include "enums/MainMenuItemType.h"

class Renderer;
class MenuManager
{
public:
	MenuManager();

	//GameState currentState;
	void AddMenuElememts( Renderer &renderer );

	void CheckItemMouseOver( int x, int y, Renderer &renderer );

	bool CheckItemMouseClick( int x, int y);

	GameState GetGameState() const;
	void SetGameState( GameState gs );

	GameState GetPrevGameState() const;

	bool HasGameStateChanged();

	GameState GoBackToPreviousMenuState();
	GameState GoToMenu();
private:
	MainMenuItemType CheckIntersections( int x, int y );
	void RemoevAllUnderscores( Renderer &renderer  );

	GameState currentGameState;
	GameState prevGameState;
	bool hasGameStateChanged;

	MainMenuItem singlePlayer;
	MainMenuItem multiPlayer;
	MainMenuItem options;
	MainMenuItem quit;
};
