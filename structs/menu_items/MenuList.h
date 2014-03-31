#pragma once

#include <vector>
//#include <SDL2/SDL.h>

#include "structs/menu_items/MenuItem.h"
#include "structs/rendering/RenderingItem.h"

#include "GameInfo.h"

#include "math/Rect.h"

struct MenuList
{
	MenuList( );

	void Init( SDL_Renderer* renderer, SDL_Rect mainRect_, const SDL_Color &backgroundColor  );
	void InitTexture( SDL_Renderer* renderer, const std::string &text, TTF_Font* font, const SDL_Color &textColor );
	void InitScrollBar();

	void AddItem( GameInfo gameInfo, SDL_Renderer* renderer, TTF_Font* font, const SDL_Color &color );
	void ClearList();

	int32_t FindIntersectedItem( int32_t x, int32_t y );
	GameInfo GetGameInfoForIndex( int32_t index ) const;

	SDL_Rect GetRect() const;

	const std::vector< MenuItem >& GetGameList() const;
	const RenderingItem< uint64_t > & GetMainArea() const;
	const RenderingItem< uint64_t > & GetCaption() const;

	SDL_Rect GetScrollBar() const;
	SDL_Rect GetTopArrow() const;
	SDL_Rect GetBottomArrow() const;

	void CheckScrollBarIntersection( int32_t x, int32_t y );
	void ScrollDown( );
	void ScrollUp( );
	private:

	void AddItem( GameInfo info, const std::string &gameLine,  SDL_Texture* texture, const SDL_Rect &rect );

	SDL_Rect scrollBar;
	SDL_Rect topArrow;
	SDL_Rect bottomArrow;

	int32_t height;
	std::vector< GameInfo > hostInfoList;
	std::vector< MenuItem > gameList;

	RenderingItem< uint64_t > mainArea;
	RenderingItem< uint64_t > caption;

};
