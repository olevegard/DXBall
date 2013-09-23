#pragma once

#include "MenuItem.h"

#include "enums/GameState.h"
#include "enums/MenuItemType.h"

class Renderer;
class MenuManager
{
public:
	MenuManager();

	//GameState currentState;
	void AddMenuElememts( Renderer &renderer );

	void CheckItemMouseOver( int x, int y, Renderer &renderer );

	bool CheckItemMouseClick( int x, int y);

private:
	MenuItemType CheckIntersections( int x, int y );
	void RemoevAllUnderscores( Renderer &renderer  );

	//GameState currentGameState;
	MenuItem singlePlayer;
	MenuItem multiPlayer;
	MenuItem options;
	MenuItem quit;
};

