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
	mainRect = mainRect_;
	Rect r;
	r.FromSDLRect( mainRect );
	mainTexture = RenderHelpers::InitSurface( r, renderer_.GetBackgroundColor(), renderer_.GetRenderer() );
	SDL_SetTextureAlphaMod( mainTexture, 173 );

	captionTexture = RenderHelpers::RenderTextTexture_Solid(
			renderer_.GetFont(),
			text,
			renderer_.GetTextColor(),
			captionRect,
			renderer_.GetRenderer()
		);

	captionRect.x = mainRect.x + static_cast< int32_t > ( ( mainRect.w * 0.5 ) - ( captionRect.w * 0.5 ) );
	captionRect.y = mainRect.y;
	height = captionRect.y + captionRect.h + 10;
}
void MenuList::AddItem( GameInfo gameInfo, Renderer &renderer_ )
{
	std::string gameLine = gameInfo.GetAsSrting();
	MenuItem item( gameLine );

	SDL_Rect r;
	r.x = mainRect.x + 10;
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
void MenuList::Render( SDL_Renderer* renderer ) const
{
	SDL_RenderCopy( renderer, mainTexture, nullptr, &mainRect  );
	SDL_RenderCopy( renderer, captionTexture, nullptr, &captionRect  );

	for ( const auto p : gameList )
	{
		SDL_Rect r = p.GetRect();
		SDL_RenderCopy( renderer, p.GetTexture(), nullptr, &r  );
	}
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
SDL_Rect MenuList::GetRect() const
{
	return mainRect;
}
