#pragma once

#include "enums/MenuItemType.h"
#include <SDL2/SDL.h>
#include <iostream>

struct MenuItem
{
	MenuItem( std::string name, SDL_Rect r, MenuItemType mit)
	{
		itemName = name;
		itemRect = r;
		menuItemType = mit;
		texture = nullptr;
		isSelected = false;
	}

	void SetRect( SDL_Rect r )
	{
		itemRect = r;
	}
	void SetRectXY( int x, int y )
	{
		itemRect.x = x;
		itemRect.y = y;
	}
	void SetRectX( int x )
	{
		itemRect.x = x;
	}
	void SetRectY( int y )
	{
		itemRect.y = y;
	}
	int GetRectX( ) const
	{
		return itemRect.x;
	}

	int GetRectY( ) const
	{
		return itemRect.y;
	}
	int GetRectH( ) const
	{
		return itemRect.h;
	}
	int GetEndX() const
	{
		return itemRect.x + itemRect.w;
	}

	SDL_Rect GetRect() const
	{
		return itemRect;
	}
	SDL_Rect* GetRectPtr()
	{
		return &itemRect;
	}

	void SetTexture( SDL_Texture* text )
	{
		SDL_DestroyTexture( texture );
		texture = text;
	}
	SDL_Texture* GetTexture( ) const
	{
		return texture;
	}
	bool IsSelected() const
	{
		return isSelected;
	}
	void SetSelcted( bool selected )
	{
		isSelected = selected;
	}

	bool HasValidTexture() const
	{
		return texture != nullptr;
	}

	void SetName( std::string str )
	{
		itemName = str;
	}
	std::string GetName( ) const
	{
		return itemName;
	}
	private:
	bool isSelected;
	std::string itemName;
	SDL_Rect itemRect;
	MenuItemType menuItemType;
	SDL_Texture* texture;

};

