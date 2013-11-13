#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <vector>

#include "MenuItem.h"
#include "HostInfo.h"

#include "math/Rect.h"
#include "math/RectHelpers.h"

class Renderer;
struct MenuList
{
	MenuList( );

	void Init( std::string name, SDL_Rect mainRect_, Renderer &renderer_ );

	void AddItem( HostInfo hostInfo, Renderer &renderer_ );

	void Render( SDL_Renderer* renderer ) const;

	int32_t FindIntersectedItem( int32_t x, int32_t y ) const;
	HostInfo GetHostInfoForIndex( int32_t index ) const
	{
		return hostInfoList[index];
	}

	SDL_Rect GetRect() const;

	private:
	SDL_Texture* mainTexture;
	SDL_Rect mainRect;

	SDL_Texture* captionTexture;
	SDL_Rect captionRect;

	int32_t height;
	std::vector< MenuItem > gameList;
	std::vector< HostInfo > hostInfoList;

	std::string caption;
};
