#include "MenuList.h"

#include "tools/RenderTools.h"
#include "math/RectHelpers.h"

#include <iostream>
MenuList::MenuList( )
{
	height = 0;
}
void MenuList::Init( SDL_Renderer* renderer, SDL_Rect mainRect_, const SDL_Color &backgroundColor  )
{
	mainArea.rect = mainRect_;
	Rect r;
	r.FromSDLRect( mainArea.rect );

	mainArea.texture = RenderHelpers::InitSurface( r, backgroundColor, renderer );
	SDL_SetTextureAlphaMod( mainArea.texture, 173 );

	InitScrollBar();
}
void MenuList::InitScrollBar()
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
void MenuList::InitTexture( SDL_Renderer* renderer, const std::string &text, TTF_Font* font, const SDL_Color &textColor )
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
}
void MenuList::AddItem( GameInfo gameInfo, SDL_Renderer* renderer, TTF_Font* font, const SDL_Color &color )
{
	std::string gameLine = gameInfo.GetAsSrting();

	SDL_Rect r;
	r.x = mainArea.rect.x + 10;
	r.y = height;

	SDL_Texture* text = RenderHelpers::RenderTextTexture_Solid
	(
	 	font,
		gameLine,
		color,
		r,
		renderer
	);

	AddItem( gameInfo, gameLine, text, r );
}
void MenuList::AddItem( GameInfo gameInfo, const std::string &gameLine,  SDL_Texture* texture, const SDL_Rect &rect )
{
	MenuItem item( gameLine  );

	height += rect.h;

	item.SetTexture( texture );
	item.SetRect( rect );
	gameList.emplace_back( item );
	hostInfoList.emplace_back( gameInfo );
}
void MenuList::ClearList()
{
	height = caption.rect.y + caption.rect.h + 10;
	gameList.clear();
	hostInfoList.clear();
}
int32_t MenuList::FindIntersectedItem( int32_t x, int32_t y )
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
void MenuList::CheckScrollBarIntersection( int32_t x, int32_t y )
{
	if ( RectHelpers::CheckMouseIntersection( x, y, bottomArrow ) )
		ScrollDown();
	else if ( RectHelpers::CheckMouseIntersection( x, y, topArrow ) )
		ScrollUp();
}
void MenuList::ScrollDown( )
{
	for ( auto &item : gameList )
		item.MoveUp( 10 );
}
void MenuList::ScrollUp( )
{
	for ( auto &item : gameList )
		item.MoveDown( 10 );
}
GameInfo MenuList::GetGameInfoForIndex( int32_t index ) const
{
	return hostInfoList[index];
}
SDL_Rect MenuList::GetRect() const
{
	return mainArea.rect;
}
const std::vector< MenuItem >& MenuList::GetGameList() const
{
	return gameList;
}
const RenderingItem< uint64_t > & MenuList::GetMainArea() const
{
	return mainArea;
}
const RenderingItem< uint64_t > & MenuList::GetCaption() const
{
	return caption;
}
SDL_Rect MenuList::GetScrollBar() const
{
	return scrollBar;
}
SDL_Rect MenuList::GetTopArrow() const
{
	return topArrow;
}
SDL_Rect MenuList::GetBottomArrow() const
{
	return bottomArrow;
}
