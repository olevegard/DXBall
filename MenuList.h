#pragma once

#include <vector>

#include "structs/menu_items/MenuItem.h"
#include "GameInfo.h"

#include "math/Rect.h"
class Renderer;
struct MenuList
{
	MenuList( );

	void Init( std::string name, SDL_Rect mainRect_, Renderer &renderer_ );

	void AddItem( GameInfo gameInfo, Renderer &renderer_ );

	void Render( SDL_Renderer* renderer ) const;

	int32_t FindIntersectedItem( int32_t x, int32_t y ) const;
	GameInfo GetGameInfoForIndex( int32_t index ) const
	{
		return hostInfoList[index];
	}

	SDL_Rect GetRect() const;

	void ClearList()
	{
		height = captionRect.y + captionRect.h + 10;
		gameList.clear();
		hostInfoList.clear();
	}

	SDL_Rect GetMainRect()
	{
		return mainRect;
	}

	private:
	SDL_Texture* mainTexture;
	SDL_Rect mainRect;

	std::string caption;
	SDL_Texture* captionTexture;
	SDL_Rect captionRect;

	int32_t height;
	std::vector< MenuItem > gameList;
	std::vector< GameInfo > hostInfoList;
};
