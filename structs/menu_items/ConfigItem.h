#pragma once

#include <SDL2/SDL.h>
#include <string>

#include "MenuItem.h"

struct ConfigItem : public MenuItem
{
	ConfigItem ( std::string name);

    virtual ~ConfigItem();
	ConfigItem() = delete;

	void SetValueTexture( SDL_Texture* texture );

	void SetValueRect( SDL_Rect r );

	SDL_Texture* GetValueTexture( ) const;

	SDL_Rect GetValueRect( ) const;

	const SDL_Rect* GetValueRectPtr( ) const;

	const SDL_Rect* GetPlussRectPtr( ) const;

	const SDL_Rect* GetMinusRectPtr( ) const;

	SDL_Rect GetPlussRect( ) const;

	SDL_Rect GetMinusRect( ) const;

	void GeneratePlussMinus();

	void SetValue( uint32_t value_ );

	uint32_t GetValue();

	bool HasChanged();

    void MoveDown( int32_t amount );

	private:
	bool hasChanged;
	uint32_t value;
	SDL_Texture* valueTexture;

	SDL_Rect valueRect;
	SDL_Rect plussRect;
	SDL_Rect minusRect;
};

