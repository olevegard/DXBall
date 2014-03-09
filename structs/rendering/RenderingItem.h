#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "../../tools/RenderTools.h"

template < class Value > class RenderingItem
{
	public :
		RenderingItem()
			:	texture( nullptr )
			,	rect{ 0, 0, 0, 0 }
			,	value( )
			,	doFade( false )
			,	alpha( 255 )
			{
				texture = nullptr;
			}

		bool CheckTexture()
		{
			return ( texture != nullptr );
		}
		void DestroyTexture()
		{
			SDL_DestroyTexture( texture );
			texture = nullptr;
		}
		void Reset( SDL_Renderer* renderer, TTF_Font* font, const SDL_Color &color )
		{
			Reset( renderer, value, font, color );
		}
		void Reset( SDL_Renderer* renderer, const std::string &str, TTF_Font* font, const SDL_Color &color )
		{
			DestroyTexture();
			texture = RenderHelpers::RenderTextTexture_Solid( font, str, color, rect, renderer );
		}
		void Init( SDL_Renderer* renderer, const SDL_Color &clr )
		{
			texture = RenderHelpers::InitSurface( rect.w, rect.h, clr, renderer );
		}
		void Rescale( uint32_t windowWidth, uint32_t windowHeight )
		{
			rect.x = ( windowWidth  / 2 ) - ( rect.w / 2 );
			rect.y = ( windowHeight / 2 ) - ( rect.h / 2 );
		}
		void Update( double delta )
		{
			if ( !doFade || alpha <= 0 )
				return;

			alpha -= ( delta * 255 );

			if ( alpha <= 5 )
			{
				alpha = 255;
				doFade = false;
			} else
			{
				SDL_SetTextureAlphaMod( texture, alpha );
			}
		}
		void ResetAlpha()
		{
			alpha = 255;
			SDL_SetTextureAlphaMod( texture, alpha );
		}
		void StartFade()
		{
			doFade = true;
		}

		template < class T >
		bool CheckValue( const T &object )
		{
			return ( object != value );
		}

		template < class T >
		bool Check( const T &object )
		{
			return CheckValue( object ) && CheckTexture();
		}

		template < class T >
		bool NeedsUpdate( const T &object )
		{
			return CheckValue( object ) || !CheckTexture();
		}

		SDL_Texture* texture;
		SDL_Rect rect;
		Value value;

		bool doFade;
		uint8_t alpha;
};
