#include "MenuList.h"

#include "tools/RenderTools.h"
#include "math/RectHelpers.h"

#include <iostream>
MenuList::MenuList( )
	:	List()
{
}
void MenuList::AddItem( GameInfo gameInfo, SDL_Renderer* renderer, TTF_Font* font, const SDL_Color &color )
{
	std::string gameLine = gameInfo.GetAsSrting();

	SDL_Rect r;
	r.x = GetMainArea().rect.x + 10;
	r.y = GetItemsTop();

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

	IncrementItemsTop( rect.h );

	item.SetTexture( texture );
	item.SetRect( rect );
	itemList .emplace_back( item );
	hostInfoList.emplace_back( gameInfo );
}
void MenuList::ClearList()
{
	ResetItemsTop();
	itemList.clear();
	hostInfoList.clear();
}
int32_t MenuList::FindIntersectedItem( int32_t x, int32_t y )
{
	int32_t index = -1;
	for ( const auto &p : itemList )
	{
		++index;

		if ( RectHelpers::CheckMouseIntersection( x, y, p.GetRect() ) )
			return index;
	}

	CheckScrollBarIntersection( x, y );

	return -1;
}
GameInfo MenuList::GetGameInfoForIndex( int32_t index ) const
{
	return hostInfoList[index];
}
const std::vector< MenuItem >& MenuList::GetGameList() const
{
	return itemList;
}
void MenuList::ScrollUp()
{
	for ( auto &item : itemList )
		item.MoveDown( 10 );
}
void MenuList::ScrollDown()
{
	for ( auto &item : itemList )
		item.MoveUp( 10 );
}
int32_t MenuList::FindTopItem() const
{
	int32_t topItem = 100000;
	for ( const auto &item : itemList )
	{
		if ( item.GetTop() < topItem )
			topItem = item.GetTop();
	}

	return topItem;
}
int32_t MenuList::FindBottomItem() const
{
	int32_t bottomItem = -1;
	for ( const auto &item : itemList )
	{
		if ( item.GetBottom() > bottomItem )
			bottomItem = item.GetBottom();
	}

	return bottomItem;
}
