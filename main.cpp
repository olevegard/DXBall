#include <iostream>
#include <string>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "Renderer.h"

Renderer renderer;

struct GamePiece;
int main( int argc, char* args[] )
{


	if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
		return 1;

	if ( !renderer.Init() )
		return 1;

	std::shared_ptr< GamePiece > gamePiece( new GamePiece() );

	renderer.AddObject( gamePiece );

	bool quit = false;
	SDL_Event event;

	while ( !quit )
	{
		while ( SDL_PollEvent( &event ) )
		{
			if ( event.type == SDL_QUIT )
				quit = true;

			if ( event.type == SDL_KEYDOWN )
			{
				switch  ( event.key.keysym.sym )
				{
					case SDLK_RIGHT:
						gamePiece->posX += 20;
						break;
					case SDLK_q:
					case SDLK_ESCAPE:
						quit = true;
						break;
					default:
						break;
				}
			}

		}
		renderer.Render( );

		gamePiece->posY += 20;

		if ( gamePiece->posY > 540 )
			gamePiece->posY = 0;
		SDL_Delay( 1000 );
	}
}
