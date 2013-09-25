#pragma once

#include "MenuItem.h"
#include "enums/PauseMenuItemType.h.h"

struct SDL_Rect;
struct PauseMenuItem : public MenuItem
{
	PauseMenuItem ( std::string name, SDL_Rect r, MainMenuItemType mit)
		:	MenuItem( name, r )
	{
		menuItemType = mit;
	}

	private:
	PauseMenuItemType menuItemType;
}
