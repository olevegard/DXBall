#include "List.h"

#include "tools/RenderTools.h"
#include "math/RectHelpers.h"

#include <iostream>
List::List( )
{
	height = 0;
}
List::~List( )
{
}
void List::Init( SDL_Renderer* renderer, SDL_Rect mainRect_, const SDL_Color &backgroundColor  )
{
	mainArea.rect = mainRect_;
	Rect r;
	r.FromSDLRect( mainArea.rect );

	mainArea.texture = RenderHelpers::InitSurface( r, backgroundColor, renderer );
	SDL_SetTextureAlphaMod( mainArea.texture, 173 );

	InitScrollBar();
}
void List::InitScrollBar()
{
	scrollBar.x = mainArea.rect.x + mainArea.rect.w -20;
	scrollBar.y = mainArea.rect.y + 10;
	scrollBar.w = 10;
	scrollBar.h = mainArea.rect.h - 20;

	topArrow.x = scrollBar.x;
	topArrow.y = scrollBar.y;
	topArrow.w = scrollBar.w;
	topArrow.h = 10;

	bottomArrow.x = scrollBar.x;
	bottomArrow.y = scrollBar.y + scrollBar.h - 10;
	bottomArrow.w = scrollBar.w;
	bottomArrow.h = 10;

}
void List::InitClipRect()
{
	listClipRect.x = mainArea.rect.x;
	listClipRect.y = caption.rect.y + caption.rect.h;

	listClipRect.w = mainArea.rect.w;
	listClipRect.h = ( mainArea.rect.y + mainArea.rect.h ) - listClipRect.y;
}
void List::InitTexture( SDL_Renderer* renderer, const std::string &text, TTF_Font* font, const SDL_Color &textColor )
{
	caption.texture = RenderHelpers::RenderTextTexture_Solid
	(
		font,
		text,
		textColor,
		caption.rect,
		renderer
	);

	caption.rect.x = mainArea.rect.x + static_cast< int32_t > ( ( mainArea.rect.w * 0.5 ) - ( caption.rect.w * 0.5 ) );
	caption.rect.y = mainArea.rect.y;
	height = caption.rect.y + caption.rect.h + 10;

	InitClipRect();
}
/*
int32_t List::FindIntersectedItem( int32_t x, int32_t y )
{
	int32_t index = -1;
	for ( const auto &p : gameList )
	{
		++index;

		if ( RectHelpers::CheckMouseIntersection( x, y, p.GetRect() ) )
			return index;
	}

	CheckScrollBarIntersection( x, y );

	return -1;
}
*/
void List::CheckScrollBarIntersection( int32_t x, int32_t y )
{
	if ( RectHelpers::CheckMouseIntersection( x, y, bottomArrow ) )
		ScrollDown();
	else if ( RectHelpers::CheckMouseIntersection( x, y, topArrow ) )
		ScrollUp();
}
SDL_Rect List::GetRect() const
{
	return mainArea.rect;
}
const RenderingItem< uint64_t > & List::GetMainArea() const
{
	return mainArea;
}
const RenderingItem< uint64_t > & List::GetCaption() const
{
	return caption;
}
SDL_Rect List::GetScrollBar() const
{
	return scrollBar;
}
SDL_Rect List::GetTopArrow() const
{
	return topArrow;
}
SDL_Rect List::GetBottomArrow() const
{
	return bottomArrow;
}
const SDL_Rect* List::GetListClipRect() const
{
	return &listClipRect;
}
