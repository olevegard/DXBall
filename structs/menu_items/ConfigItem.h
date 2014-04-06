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
	void MoveUp( int32_t amount );
	void MoveRight( int32_t amount );

	void SetRectX( int32_t x );
	void SetRectY( int32_t y );
	int32_t GetTop()
	{
		return plussRect.y;
	}
	int32_t GetBottom()
	{
		return minusRect.y + minusRect.h;
	}
	int32_t GetTotalHeight()
	{
		return ( minusRect.y + minusRect.h ) - plussRect.y;
	}

	private:
	bool hasChanged;
	uint32_t value;
	SDL_Texture* valueTexture;

	SDL_Rect valueRect;
	SDL_Rect plussRect;
	SDL_Rect minusRect;
};

