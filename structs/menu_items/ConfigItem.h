#pragma once

#include <SDL2/SDL.h>
#include <string>

#include "MenuItem.h"

struct ConfigItem : public MenuItem
{
	ConfigItem ( std::string name)
		:	MenuItem( name )
	{
	}

	ConfigItem() = delete;

	void SetValueTexture( SDL_Texture* texture )
	{
		valueTexture = texture;
	}
	void SetValueRect( SDL_Rect r )
	{
		valueRect = r;
	}
	SDL_Texture* GetValueTexture( ) const
	{
		return valueTexture;
	}
	SDL_Rect GetValueRect( ) const
	{
		return valueRect;
	}
	const SDL_Rect* GetValueRectPtr( ) const
	{
		return &valueRect;
	}
	private:
		SDL_Texture* valueTexture;
		SDL_Rect valueRect;
		//int32_t optionsValue;
};

