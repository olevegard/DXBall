#include "RenderTools.h"

#include <iostream>
#include "../structs/rendering/RenderingItem.h"
#include "../structs/rendering/Particle.h"

#include "../structs/menu_items/MainMenuItem.h"
#include "../structs/menu_items/ConfigItem.h"

#include "../structs/game_objects/GamePiece.h"

#include "../structs/menu_items/MenuList.h"
#include "../structs/menu_items/ConfigList.h"

const int32_t RenderHelpers::SCREEN_BPP = 32;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	const uint32_t RenderHelpers::R_MASK = 0xff000000;
	const uint32_t RenderHelpers::G_MASK = 0x00ff0000;
	const uint32_t RenderHelpers::B_MASK = 0x0000ff00;
	const uint32_t RenderHelpers::A_MASK = 0x000000ff;
#else
	const uint32_t RenderHelpers::R_MASK = 0x000000ff;
	const uint32_t RenderHelpers::G_MASK = 0x0000ff00;
	const uint32_t RenderHelpers::B_MASK = 0x00ff0000;
	const uint32_t RenderHelpers::A_MASK = 0xff000000;
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
	SDL_Surface* surface = SDL_CreateRGBSurface( 0, width, height,
		SCREEN_BPP, R_MASK, G_MASK, B_MASK, A_MASK
	);

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
void RenderHelpers::RenderMenuItem( SDL_Renderer* renderer, const std::shared_ptr< MenuItem > &item )
{
	if( item->GetTexture() != nullptr )
		SDL_RenderCopy( renderer, item->GetTexture(), nullptr, item->GetRectPtr() );
}
void RenderHelpers::RenderOptionsItem( SDL_Renderer* renderer, const std::shared_ptr< ConfigItem > &item )
{
	RenderMenuItem( renderer, item );

	if( item->GetValueTexture() != nullptr )
		SDL_RenderCopy( renderer, item->GetValueTexture(), nullptr, item->GetValueRectPtr() );

	RenderPluss( renderer, item->GetPlussRect() );
	RenderMinus( renderer, item->GetMinusRect() );
}
void RenderHelpers::RenderMinus( SDL_Renderer* renderer, SDL_Rect square )
{
	SDL_Rect minus = square;
	minus .h /= 3;
	minus .y += ( square.h / 2 ) - ( minus .h / 2 );

	SDL_SetRenderDrawColor( renderer, 255,0,0,255 );
	SDL_RenderFillRect( renderer, &minus );
}
void RenderHelpers::RenderPluss( SDL_Renderer* renderer, SDL_Rect square )
{
	SDL_Rect horizontalLine = square;
	horizontalLine .w /= 3;
	horizontalLine .x += ( square.w / 2 ) - ( horizontalLine .w / 2 );

	SDL_Rect verticalLine = square;
	verticalLine.h /= 3;
	verticalLine.y += ( square.h / 2 ) - ( verticalLine.h / 2 );

	SDL_SetRenderDrawColor( renderer, 0,255,0,255 );
	SDL_RenderFillRect( renderer, &horizontalLine );
	SDL_RenderFillRect( renderer, &verticalLine );
}
void RenderHelpers::RenderPlussMinus ( SDL_Renderer* renderer, SDL_Rect origin )
{
	SDL_Rect square = origin;
	square.w = 15;
	square.h = square.w;

	square.x += ( origin.w /2 ) - ( square.w / 2 );
	square.y -= square.h - 5;

	RenderPluss( renderer, square );
	square.y = origin.y + origin.h - 5;
	RenderMinus( renderer, square );
}
void RenderHelpers::RenderMenuList( SDL_Renderer* renderer, const MenuList &menuList, const SDL_Rect &screenSize )
{
	RenderTextItem( renderer, menuList.GetMainArea() );
	RenderTextItem( renderer, menuList.GetCaption() );

	RenderScrollBar( renderer, menuList ); 
	RenderMenuListItems( renderer, menuList, screenSize ); 
}
void RenderHelpers::RenderConfigList   ( SDL_Renderer* renderer, const ConfigList &menuList, const SDL_Rect &screenSize )
{
	RenderTextItem( renderer, menuList.GetMainArea() );
	RenderTextItem( renderer, menuList.GetCaption() );

	RenderScrollBar( renderer, menuList ); 
	RenderMenuListItems( renderer, menuList, screenSize ); 
}
void RenderHelpers::RenderScrollBar    ( SDL_Renderer* renderer, const ConfigList &menuList )
{
	SDL_SetRenderDrawColor( renderer, 0,0,255,255 );
	SDL_Rect r = menuList.GetScrollBar();
	SDL_RenderFillRect( renderer, &r );

	SDL_SetRenderDrawColor( renderer, 0,255,0,255 );
	r = menuList.GetTopArrow();
	SDL_RenderFillRect( renderer, &r );

	SDL_SetRenderDrawColor( renderer, 255,0,0,255 );
	r = menuList.GetBottomArrow();
	SDL_RenderFillRect( renderer, &r);
}
void RenderHelpers::RenderMenuListItems( SDL_Renderer* renderer, const ConfigList &menuList, const SDL_Rect &screenSize )
{
	SDL_RenderSetClipRect( renderer, menuList.GetListClipRect()  );

	const auto &configList  = menuList.GetConfigList();
	for ( const auto &p : configList )
		RenderOptionsItem( renderer, p.second );

	SDL_RenderSetClipRect( renderer, &screenSize );
}
void RenderHelpers::RenderScrollBar  ( SDL_Renderer* renderer, const MenuList &menuList )
{
	SDL_SetRenderDrawColor( renderer, 0,0,255,255 );
	SDL_Rect r = menuList.GetScrollBar();
	SDL_RenderFillRect( renderer, &r );

	SDL_SetRenderDrawColor( renderer, 0,255,0,255 );
	r = menuList.GetTopArrow();
	SDL_RenderFillRect( renderer, &r );

	SDL_SetRenderDrawColor( renderer, 255,0,0,255 );
	r = menuList.GetBottomArrow();
	SDL_RenderFillRect( renderer, &r);
}
void RenderHelpers::RenderMenuListItems( SDL_Renderer* renderer, const MenuList &menuList, const SDL_Rect &screenSize )
{
	SDL_RenderSetClipRect( renderer, menuList.GetListClipRect()  );

	const auto &gameList = menuList.GetGameList();
	for ( const auto &p : gameList )
	{
		SDL_Rect r = p.GetRect();
		SDL_RenderCopy( renderer, p.GetTexture(), nullptr, &r  );
	}
	SDL_RenderSetClipRect( renderer, &screenSize );
}
void RenderHelpers::RenderParticle( SDL_Renderer* renderer, const Particle& particle )
{
	if ( particle.isAlive )
	{
		SDL_Rect r = particle.rect.ToSDLRect();
		SetDrawColor( renderer, particle.color );
		SDL_RenderFillRect( renderer, &r );
	}
}
void RenderHelpers::RenderGamePiece( SDL_Renderer* renderer, const std::shared_ptr< GamePiece > &gamePiece )
{
	SDL_Rect pieceRect = gamePiece->rect.ToSDLRect();
	SDL_RenderCopy( renderer, gamePiece->GetTexture(), nullptr, &pieceRect );
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
SDL_Texture* RenderHelpers::CreateBonusBoxTexture(
		SDL_Renderer* renderer,
		SDL_Rect bonusBoxRect,
		const SDL_Color &outerColor,
		const SDL_Color &innerColor
	)
{
	// Background
	SDL_Surface* bonus = SDL_CreateRGBSurface( 0, bonusBoxRect.w, bonusBoxRect.h, SCREEN_BPP, R_MASK, G_MASK, B_MASK, A_MASK);

	uint32_t pixelValue = RenderHelpers::MapRGBA( bonus->format, outerColor );
	SDL_FillRect( bonus, NULL, pixelValue );

	bonusBoxRect = bonus->clip_rect;
	SetBonusBoxIcon( bonusBoxRect.w, bonus, innerColor );

	SDL_Texture* bonusBoxTexture = SDL_CreateTextureFromSurface( renderer, bonus );

	SDL_FreeSurface( bonus );

	return bonusBoxTexture;
}
void RenderHelpers::SetBonusBoxIcon( int32_t width, SDL_Surface* bonusBox,  const SDL_Color &innerColor  )
{
	int32_t bbMargin = width / 8;
	int32_t doubleMargin = bbMargin * 2;

	SDL_Rect logoPosition;
	logoPosition.x = bbMargin;
	logoPosition.y = bbMargin;
	logoPosition.w = width - doubleMargin;
	logoPosition.h = width - doubleMargin;

	SDL_Surface* logo = SDL_CreateRGBSurface( 0, logoPosition.w, logoPosition.h, SCREEN_BPP, R_MASK, G_MASK, B_MASK, A_MASK);
	SDL_FillRect( logo, NULL, SDL_MapRGBA( bonusBox->format, innerColor.r, innerColor.g,  innerColor.b, innerColor.a ) );

	SDL_BlitSurface( logo, NULL, bonusBox, &logoPosition);
	SDL_FreeSurface( logo );
}
SDL_Texture* RenderHelpers::LoadImage( const std::string &filename, SDL_Renderer* renderer )
{
	SDL_Surface* loadedImage = IMG_Load( filename.c_str() );

	SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, loadedImage );

	SDL_FreeSurface( loadedImage );

	// If the image loaded
	if ( texture == nullptr )
		std::cout << "Renderer@" << __LINE__  << " Failed to load " << filename << std::endl;

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
