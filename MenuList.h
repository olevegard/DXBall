#pragma once

#include <vector>

#include "structs/menu_items/MenuItem.h"
#include "structs/rendering/RenderingItem.h"

#include "GameInfo.h"

#include "math/Rect.h"
class Renderer;
struct MenuList
{
	MenuList( );

	void Init( std::string name, SDL_Rect mainRect_, Renderer &renderer_ );

	void AddItem( GameInfo gameInfo, Renderer &renderer_ );

	int32_t FindIntersectedItem( int32_t x, int32_t y ) const;
	GameInfo GetGameInfoForIndex( int32_t index ) const
	{
		return hostInfoList[index];
	}

	SDL_Rect GetRect() const;

	void ClearList()
	{
		height = caption.rect.y + caption.rect.h + 10;
		gameList.clear();
		hostInfoList.clear();
	}

	SDL_Rect GetMainRect()
	{
		return mainArea.rect;
	}

	RenderingItem< uint64_t > mainArea;
	RenderingItem< uint64_t > caption;

	std::vector< MenuItem > gameList;

	private:
	int32_t height;
	std::vector< GameInfo > hostInfoList;
};
