#pragma once

#include <SDL2/SDL.h>
#include <string>

struct MenuItem
{
	MenuItem( std::string name);
	MenuItem( const MenuItem &item );
	MenuItem() = delete;

	virtual ~MenuItem();

	virtual void SetRect( SDL_Rect r );
	virtual void SetRectXY( int x, int y );
	virtual void SetRectX( int x );
	virtual void SetRectY( int y );

	virtual void MoveDown( int32_t y );
	virtual void MoveUp( int32_t y );
	virtual void MoveRight( int32_t x );

	virtual int32_t GetLeft( ) const;
	virtual int32_t GetRight() const;
	virtual int32_t GetTop( ) const;
	virtual int32_t GetBottom( ) const;
	virtual int32_t GetWidth( ) const;
	virtual int32_t GetHeight( ) const;

	SDL_Rect GetRect() const;
	const SDL_Rect* GetRectPtr() const;

	void SetTexture( SDL_Texture* text );
	SDL_Texture* GetTexture( ) const;

	bool IsSelected() const;
	void SetSelcted( bool selected );

	bool HasValidTexture() const;
	bool HasUnderlineChanged();

	void SetName( std::string str );
	std::string GetName( ) const;

	void SetBackgroundColor( SDL_Color clr )
	{
		backgroundColor = clr;
	}
	SDL_Color GetBackgroundColor( ) const
	{
		return backgroundColor;
	}

	private:
	std::string itemName;
	SDL_Rect itemRect;
	SDL_Texture* texture;
	SDL_Color backgroundColor;

	bool isSelected;
	bool hasChanged;
};

