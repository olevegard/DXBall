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

	int GetRectX( ) const;

	int GetRectY( ) const;
	int GetRectW( ) const;
	int GetRectH( ) const;
	int GetEndX() const;

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

	private:
	std::string itemName;
	SDL_Rect itemRect;
	SDL_Texture* texture;
	bool isSelected;
	bool hasChanged;
};

