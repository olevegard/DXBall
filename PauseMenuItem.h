#pragma once

#include "MenuItem.h"
#include "enums/PauseMenuItemType.h"

struct SDL_Rect;
struct PauseMenuItem : public MenuItem
{
	PauseMenuItem ( std::string name, PauseMenuItemType pit)
		:	MenuItem( name )
	{
		pauseItemType = pit;
	}

	private:
	PauseMenuItemType pauseItemType;
};
