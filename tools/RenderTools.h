#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "math/Rect.h"
#include "../structs/menu_items/MainMenuItem.h"
#include "../structs/game_objects/GamePiece.h"

#include <string>
#include <vector>
#include <iostream>

#include <memory>

template < class Value > class RenderingItem;
struct Particle;
class RenderHelpers
{
	public:
	RenderHelpers();

	static SDL_Texture* InitSurface( const Rect &rect     , const SDL_Color &clr, SDL_Renderer* renderer  );
	static SDL_Texture* InitSurface( int width, int height, const SDL_Color &clr, SDL_Renderer* renderer  );
	static SDL_Texture* InitSurface( const Rect &rect, unsigned char r, unsigned char g, unsigned char b, SDL_Renderer* renderer );
	static SDL_Texture* InitSurface( int width, int height, unsigned char r, unsigned char g, unsigned char b, SDL_Renderer* renderer  );

	static void FillSurface( SDL_Surface* source, unsigned char r, unsigned char g, unsigned char b );
	static void FillSurface( SDL_Surface* source, const SDL_Color &color );

	static SDL_Texture* LoadImage( const std::string &filename, SDL_Renderer* renderer );
	static SDL_Surface* SetDisplayFormat( SDL_Surface* surface );

	static TTF_Font* LoadFont( const std::string &name, int size );

	static void SetTileColorSurface( size_t index, const SDL_Color &color, std::vector< SDL_Texture* > &list, SDL_Renderer *renderer  );

	static SDL_Texture* RenderTextTexture_Solid(
		TTF_Font* textFont,
		const std::string &textToRender,
		const SDL_Color &color,
		SDL_Rect &rect,
		SDL_Renderer *renderer
	);

	static SDL_Texture* RenderTextTexture_Blended(
		TTF_Font* textFont,
		const std::string &textToRender,
		const SDL_Color &color,
		SDL_Rect &rect,
		SDL_Renderer * renderer,
		int style  = 0
	);

	static SDL_Texture*
		CreateBonusBoxTexture( SDL_Renderer* renderer, SDL_Rect bonusBoxRect, const SDL_Color &outerColor, const SDL_Color &innerColor );
	static uint32_t MapRGBA( SDL_PixelFormat* pixelFormat, const SDL_Color &clr );
	static void SetDrawColor( SDL_Renderer* renderer, const SDL_Color &clr );


	static void RenderTextItem( SDL_Renderer* renderer,  const RenderingItem< std::string >  &item );
	static void RenderTextItem( SDL_Renderer* renderer, const RenderingItem< uint64_t >  &item );
	static void RenderMenuItem( SDL_Renderer* renderer, const MenuItem &item );
	static void RenderParticle( SDL_Renderer* renderer, const Particle& particle );
	static void RenderGamePiexe( SDL_Renderer* renderer, const std::shared_ptr< GamePiece > &gamePiece )
	{
		SDL_Rect pieceRect = gamePiece->rect.ToSDLRect();
		SDL_RenderCopy( renderer, gamePiece->GetTexture(), nullptr, &pieceRect );
	}

	static void SetTileColorSurface( SDL_Renderer* renderer, size_t index, const SDL_Color &color, std::vector< SDL_Texture* > &list  );
	static void HideMouseCursor( bool hide);
	static void ForceInputGrab( SDL_Window *window, bool grab );


	private:
	static void SetBonusBoxIcon( int32_t width, SDL_Surface* bonusBox,  const SDL_Color &innerColor );

	static const int32_t SCREEN_BPP;
	static const uint32_t R_MASK;
	static const uint32_t G_MASK;
	static const uint32_t B_MASK;
	static const uint32_t A_MASK;
};
