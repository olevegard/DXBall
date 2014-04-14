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

	int32_t GetLeft() const;
	int32_t GetRight() const;
	int32_t GetTop() const;
	int32_t GetBottom() const;

	int32_t GetTotalHeight();

	bool IsBool() const
	{
		return isBool;
	}
	void SetIsBool( bool isBool_ )
	{
		 isBool = isBool_;
	}

	bool GetBool() const
	{
		return boolValue;
	}

	private:
	bool hasChanged;
	bool isBool;

	uint32_t value;
	bool boolValue;

	SDL_Texture* valueTexture;

	SDL_Rect valueRect;
	SDL_Rect plussRect;
	SDL_Rect minusRect;
};
