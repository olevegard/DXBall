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

	void ClearList();

	int32_t FindIntersectedItem( int32_t x, int32_t y ) const;
	GameInfo GetGameInfoForIndex( int32_t index ) const;

	SDL_Rect GetRect() const;
	//SDL_Rect GetMainRect();
	const std::vector< MenuItem >& GetGameList() const;
	const RenderingItem< uint64_t > & GetMainArea() const;
	const RenderingItem< uint64_t > & GetCaption() const;

	private:

	int32_t height;
	std::vector< GameInfo > hostInfoList;
	std::vector< MenuItem > gameList;

	RenderingItem< uint64_t > mainArea;
	RenderingItem< uint64_t > caption;
};
