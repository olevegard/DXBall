#pragma once

#include <vector>
//#include <SDL2/SDL.h>

#include "structs/menu_items/MenuItem.h"
#include "structs/rendering/RenderingItem.h"

#include "GameInfo.h"

#include "math/Rect.h"

struct List
{
	// General
	// ========================================================================================================
	List( );
	virtual ~List( );
	void Init( SDL_Renderer* renderer, SDL_Rect mainRect_, const SDL_Color &backgroundColor  );
	void InitTexture( SDL_Renderer* renderer, const std::string &text, TTF_Font* font, const SDL_Color &textColor );

	SDL_Rect GetRect() const;
	const RenderingItem< uint64_t > & GetMainArea() const;
	const RenderingItem< uint64_t > & GetCaption() const;

	// Scroll Bar
	// ========================================================================================================
	void InitScrollBar();
	void InitClipRect();

	SDL_Rect GetScrollBar() const;
	SDL_Rect GetTopArrow() const;
	SDL_Rect GetBottomArrow() const;
	const SDL_Rect* GetListClipRect() const;

	void CheckScrollBarIntersection( int32_t x, int32_t y );

	void IncrementHeight( int32_t amount )
	{
		height += amount;
	}
	void ResetHeight()
	{
		height = caption.rect.y + caption.rect.h + 10;
	}
	int32_t GetHeight()
	{
		return height;
	}
	private:
	virtual void ScrollDown() = 0;
	virtual void ScrollUp() = 0;
	SDL_Rect scrollBar;
	SDL_Rect topArrow;
	SDL_Rect bottomArrow;
	SDL_Rect listClipRect;

	int32_t height;

	RenderingItem< uint64_t > mainArea;
	RenderingItem< uint64_t > caption;
};
