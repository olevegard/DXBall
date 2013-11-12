#include "MenuList.h"
#include "Renderer.h"

MenuList::MenuList( )
{
	height = 0;
}
void MenuList::Init( std::string name, SDL_Rect mainRect_, Renderer &renderer_  )
{
	mainRect = mainRect_;
	Rect r;
	r.FromSDLRect( mainRect );
	mainTexture = renderer_.InitSurface( r, {255,255,255,255 } );

	captionTexture = renderer_.RenderTextTexture_Solid( renderer_.GetFont(), name, { 255, 0, 255, 255}, captionRect  );
	captionRect.x = mainRect.x + static_cast< int32_t > ( ( mainRect.w * 0.5 ) - ( captionRect.w * 0.5 ) );
	captionRect.y = mainRect.y;
	height = captionRect.y + captionRect.h + 10;
}
void MenuList::AddItem( std::string itemCaption, Renderer &renderer_ )
{
	MenuItem item( itemCaption  );

	SDL_Rect r;
	r.x = mainRect.x + 10;
	r.y = height;
	SDL_Texture* text =
		renderer_.RenderTextTexture_Solid
		(
		 renderer_.GetFont(),
		 itemCaption,
		 {255,0,0,255 },
		 r
		);

	height += r.h;
	item.SetTexture( text );
	item.SetRect( r );
	gameList.emplace_back( item );
}
void MenuList::Render( SDL_Renderer* renderer )
{
	/*
	   std::cout << "Rendering..."
	   <<  mainRect.x << " , "
	   << mainRect.y << " : "
	   << mainRect.w << " x "
	   << mainRect.h
	   << std::endl;
	   */

	SDL_RenderCopy( renderer, mainTexture, nullptr, &mainRect  );

	SDL_RenderCopy( renderer, captionTexture, nullptr, &captionRect  );

	for ( const auto p : gameList )
	{
		std::cout << "Rendering : " << p.GetName() << "\n";
		SDL_Rect r = p.GetRect();
		SDL_RenderCopy( renderer, p.GetTexture(), nullptr, &r  );
	}
}

