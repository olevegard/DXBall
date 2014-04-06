#pragma once

#include "structs/menu_items/List.h"
#include "structs/menu_items/MenuItem.h"

#include "GameInfo.h"

struct MenuList : public List
{
	MenuList( );

	// Menu List
	// ========================================================================================================
	void ClearList();
	const std::vector< MenuItem >& GetGameList() const;
	int32_t FindIntersectedItem( int32_t x, int32_t y );
	GameInfo GetGameInfoForIndex( int32_t index ) const;

	// GameList specific
	void AddItem( GameInfo gameInfo, SDL_Renderer* renderer, TTF_Font* font, const SDL_Color &color );

	private:
	void ScrollDown( );
	void ScrollUp( );

	int32_t FindTopItem() const;
	int32_t FindBottomItem() const;

	void AddItem( GameInfo info, const std::string &gameLine,  SDL_Texture* texture, const SDL_Rect &rect );

	std::vector< MenuItem > itemList;
	std::vector< GameInfo > hostInfoList;
};
