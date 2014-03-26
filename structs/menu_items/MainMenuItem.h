#pragma once

#include "MenuItem.h"
#include "enums/MainMenuItemType.h"

struct SDL_Rect;
struct MainMenuItem : public MenuItem
{
	MainMenuItem( std::string name, MainMenuItemType mit);

    virtual ~MainMenuItem();

	private:
	MainMenuItemType menuItemType;
};
