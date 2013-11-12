#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <vector>

#include "MenuItem.h"
#include "math/Rect.h"

class Renderer;
struct MenuList
{
	MenuList( );

	void Init( std::string name, SDL_Rect mainRect_, Renderer &renderer_ );

	void AddItem(
			std::string itemCaption,
			Renderer &renderer_
	);
/*
	void SetMainRect( SDL_Rect mainRect_  )
	{
		mainRect= mainRect_;
	}
	*/

	void Render( SDL_Renderer* renderer );

	void SetCaption( std::string caption_ )
	{
		caption = caption_;
	}

	private:
	SDL_Texture* mainTexture;
	SDL_Rect mainRect;

	SDL_Texture* captionTexture;
	SDL_Rect captionRect;

	int32_t height;
	std::vector< MenuItem > gameList;
	std::string caption;
};

