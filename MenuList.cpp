#include "MenuList.h"
#include "Renderer.h"

#include "tools/RenderTools.h"
#include "math/RectHelpers.h"

#include <iostream>
MenuList::MenuList( )
{
	height = 0;
}
void MenuList::Init( std::string text, SDL_Rect mainRect_, Renderer &renderer_  )
{
	mainArea.rect = mainRect_;
	Rect r;
	r.FromSDLRect( mainArea.rect );

	mainArea.texture = RenderHelpers::InitSurface( r, renderer_.GetBackgroundColor(), renderer_.GetRenderer() );
	SDL_SetTextureAlphaMod( mainArea.texture, 173 );

	caption.texture = RenderHelpers::RenderTextTexture_Solid
	(
		renderer_.GetFont(),
		text,
		renderer_.GetTextColor(),
		caption.rect,
		renderer_.GetRenderer()
	);

	caption.rect.x = mainArea.rect.x + static_cast< int32_t > ( ( mainArea.rect.w * 0.5 ) - ( caption.rect.w * 0.5 ) );
	caption.rect.y = mainArea.rect.y;
	height = caption.rect.y + caption.rect.h + 10;
}
void MenuList::AddItem( GameInfo gameInfo, Renderer &renderer_ )
{
	std::string gameLine = gameInfo.GetAsSrting();
	MenuItem item( gameLine );

	SDL_Rect r;
	r.x = mainArea.rect.x + 10;
	r.y = height;
	SDL_Texture* text = RenderHelpers::RenderTextTexture_Solid
	(
		 renderer_.GetFont(),
		 gameLine,
		 renderer_.GetTextColor(),
		 r,
		 renderer_.GetRenderer()
	);

	height += r.h;
	item.SetTexture( text );
	item.SetRect( r );
	gameList.emplace_back( item );
	hostInfoList.emplace_back( gameInfo );
}
void MenuList::ClearList()
{
	height = caption.rect.y + caption.rect.h + 10;
	gameList.clear();
	hostInfoList.clear();
}
int32_t MenuList::FindIntersectedItem( int32_t x, int32_t y ) const
{
	int32_t index = -1;
	for ( const auto &p : gameList )
	{
		++index;

		if ( RectHelpers::CheckMouseIntersection( x, y, p.GetRect() ) )
			return index;
	}

	return -1;
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
