#include "RenderTools.h"

#include <iostream>


int32_t RenderHelpers::SCREEN_BPP = 32;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
uint32_t RenderHelpers::rmask = 0xff000000;
uint32_t RenderHelpers::gmask = 0x00ff0000;
uint32_t RenderHelpers::bmask = 0x0000ff00;
uint32_t RenderHelpers::amask = 0x000000ff;
#else
uint32_t RenderHelpers::rmask = 0x000000ff;
uint32_t RenderHelpers::gmask = 0x0000ff00;
uint32_t RenderHelpers::bmask = 0x00ff0000;
uint32_t RenderHelpers::amask = 0xff000000;
#endif

RenderHelpers::RenderHelpers()
{
}
SDL_Texture* RenderHelpers::InitSurface( const Rect &rect     , const SDL_Color &clr, SDL_Renderer* renderer  )
{
	return RenderHelpers::InitSurface( static_cast< int > ( rect.w ), static_cast< int > ( rect.h ), clr.r, clr.g, clr.b, renderer  );
}
SDL_Texture* RenderHelpers::InitSurface( int width, int height, const SDL_Color &clr, SDL_Renderer* renderer  )
{
	return InitSurface( width, height, clr.r, clr.g, clr.b, renderer  );
}
SDL_Texture* RenderHelpers::InitSurface( const Rect &rect, unsigned char r, unsigned char g, unsigned char b, SDL_Renderer* renderer )
{
	return InitSurface( static_cast< int > ( rect.w ), static_cast< int > ( rect.h ), r, g, b, renderer );
}
SDL_Texture* RenderHelpers::InitSurface( int width, int height, unsigned char r, unsigned char g, unsigned char b, SDL_Renderer* renderer )
{
	SDL_Surface* surface = SDL_CreateRGBSurface( 0, width, height, SCREEN_BPP, rmask, gmask, bmask, amask);

	FillSurface( surface, r, g, b );

	SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, surface );
	SDL_FreeSurface( surface );

	return texture;
}
void RenderHelpers::FillSurface( SDL_Surface* source, unsigned char r, unsigned char g, unsigned char b )
{
	SDL_FillRect( source, NULL, SDL_MapRGBA( source->format, r, g, b, 255 )  );
}
void RenderHelpers::FillSurface( SDL_Surface* source, const SDL_Color &color )
{
	FillSurface( source, color.r, color.g, color.b );
}
SDL_Texture* RenderHelpers::LoadImage( const std::string &filename, SDL_Renderer* renderer )
{
	// Temporary stoare for the iamge that's loaded
	SDL_Surface* loadedImage = nullptr;

	// Load the image
	loadedImage = IMG_Load( filename.c_str() );


	SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, loadedImage );

	// Free the old image
	SDL_FreeSurface( loadedImage );

	// If the image loaded
	if ( texture == nullptr )
	{
		std::cout << "Renderer@" << __LINE__  << " Failed to load " << filename << std::endl;
	}

	return texture;
}
SDL_Surface* RenderHelpers::SetDisplayFormat( SDL_Surface* surface )
{
	if ( !surface )
	{
		std::cout << "Renderer@" << __LINE__  << " Cannot set display format : nullptr\n";
	}

	return surface;
}
void RenderHelpers::SetTileColorSurface( size_t index, const SDL_Color &color, std::vector< SDL_Texture* > &list, SDL_Renderer *renderer  )
{
	SDL_Texture *text = InitSurface(  60, 20, color.r, color.g, color.b, renderer );
	list.at( index ) = text;
}

SDL_Texture* RenderHelpers::RenderTextTexture_Solid(
		TTF_Font* textFont,
		const std::string &textToRender,
		const SDL_Color &color,
		SDL_Rect &rect,
		SDL_Renderer *renderer
	)
{
	SDL_Surface* surface = TTF_RenderText_Solid( textFont, textToRender.c_str(), color );

	if ( surface != nullptr )
	{
		rect.w = surface->clip_rect.w;
		rect.h = surface->clip_rect.h;

		SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, surface );

		SDL_FreeSurface( surface );

		return texture;
	} else
	{
		std::cout << "Renderer@" << __LINE__  << " Failed to create text surface..." << textFont << " " << textToRender << " \n";
		return nullptr;
	}
}
SDL_Texture* RenderHelpers::RenderTextTexture_Blended(
		TTF_Font* textFont,
		const std::string &textToRender,
		const SDL_Color &color,
		SDL_Rect &rect,
		SDL_Renderer * renderer,
		int style /* = 0 */
	)
{
	if ( style != 0 )
		TTF_SetFontStyle( textFont, style );

	SDL_Surface* surface = TTF_RenderText_Blended( textFont, textToRender.c_str(), color);


	if ( style != 0 )
		TTF_SetFontStyle( textFont, TTF_STYLE_NORMAL );

	if ( surface != nullptr )
	{
		rect.w = surface->clip_rect.w;
		rect.h = surface->clip_rect.h;

		SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, surface );

		SDL_FreeSurface( surface );

		return texture;
	}
	else
	{
		std::cout << "Renderer@" << __LINE__  << " Failed to create text surface..." << textFont << " " << textToRender << " \n";
		return nullptr;
	}
}
uint32_t RenderHelpers::MapRGBA( SDL_PixelFormat* pixelFormat, const SDL_Color &clr )
	{
		return SDL_MapRGBA( pixelFormat, clr.r, clr.g, clr.b, clr.a );
	}

	void RenderHelpers::SetDrawColor( SDL_Renderer* renderer, const SDL_Color &clr )
	{
		SDL_SetRenderDrawColor( renderer, clr.r, clr.g, clr.b, clr.a);
	}
	void RenderHelpers::RenderTextItem( SDL_Renderer* renderer,  const RenderingItem< std::string >  &item )
	{
		if ( item.texture != nullptr  )
			SDL_RenderCopy( renderer, item.texture, nullptr, &item.rect );
	}

	void RenderHelpers::RenderTextItem( SDL_Renderer* renderer, const RenderingItem< uint64_t >  &item )
	{
		if ( item.texture != nullptr  )
			SDL_RenderCopy( renderer, item.texture, nullptr, &item.rect );
	}
	void RenderHelpers::RenderMenuItem( SDL_Renderer* renderer, const MenuItem &item )
	{
		if( item.GetTexture() != nullptr )
		{
			SDL_Rect r = item.GetRect();
			SDL_RenderCopy( renderer, item.GetTexture(), nullptr, &r );
		}
	}
	void RenderHelpers::SetTileColorSurface( SDL_Renderer* renderer, size_t index, const SDL_Color &color, std::vector< SDL_Texture* > &list  )
	{
		SDL_Texture* text = RenderHelpers::InitSurface(  60, 20, color.r, color.g, color.b, renderer );
		list.at( index ) = text;
	}
	void RenderHelpers::HideMouseCursor( bool hide)
	{
		if ( hide )
			SDL_ShowCursor( SDL_DISABLE );
		else
			SDL_ShowCursor( SDL_ENABLE );
	}
	void RenderHelpers::ForceInputGrab( SDL_Window *window, bool grab )
	{
		if ( grab )
			SDL_SetWindowGrab( window, SDL_TRUE );
		else
			SDL_SetWindowGrab( window, SDL_FALSE );
	}
	TTF_Font* RenderHelpers::LoadFont( const std::string &name, int size )
	{
		TTF_Font* font = TTF_OpenFont( name.c_str(), size );

		if ( font == nullptr )
			std::cout << "renderer@" << __LINE__  << " failed to open font : " << name << " : " << TTF_GetError() << std::endl;

		return font;
	}
