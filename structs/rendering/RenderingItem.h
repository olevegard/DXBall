#pragma once

#include <SDL2/SDL.h>

template < class Value > class RenderingItem
{
	public :
	RenderingItem()
	{
		texture = nullptr;
	}

	bool CheckTexture()
	{
		return ( texture != nullptr );
	}

	template < class T >
	bool CheckValue( const T &object )
	{
		return ( object != value );
	}

	SDL_Texture* texture;
	SDL_Rect rect;
	Value value;
};
