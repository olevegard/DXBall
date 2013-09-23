#pragma once

#include "enums/MenuItemType.h"
#include <SDL2/SDL.h>

struct MenuItem
{
	MenuItem( std::string name, SDL_Rect r, MenuItemType mit)
	{
		itemName = name;
		itemRect = r;
		menuItemType = mit;
	}

	void SetRect( SDL_Rect r )
	{
		itemRect = r;
	}

	SDL_Rect GetRect() const
	{
		return itemRect;
	}

	private:
	std::string itemName;
	SDL_Rect itemRect;
	MenuItemType menuItemType;

};
