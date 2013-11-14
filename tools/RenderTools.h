#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "math/Rect.h"

#include <string>
#include <vector>

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

	private:
	static int32_t SCREEN_BPP;

	static uint32_t rmask;
	static uint32_t gmask;
	static uint32_t bmask;
	static uint32_t amask;
};
