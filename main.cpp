#include <iostream>
#include <string>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "Renderer.h"
#include "Timer.h"

Renderer renderer;
Timer timer;

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
	
	SDL_Rect tileSize = renderer.GetTileSize();
	int windowWidth = renderer.GetWindowWidth();

	float delta = 0;

	SDL_WM_GrabInput( SDL_GRAB_FULLSCREEN );
	SDL_MouseMotionEvent prevMotion;
	int halfTileWidth = tileSize.w / 2;
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
						gamePiece->posX += tileSize.w;
						break;
					case SDLK_LEFT:
						gamePiece->posX -= tileSize.w;
						break;
					case SDLK_q:
					case SDLK_ESCAPE:
						quit = true;
						break;
					default:
						break;
				}
			}

			if ( event.motion.x != 0 && event.motion.y != 0 )
				gamePiece->posX = event.motion.x - halfTileWidth;

			if ( ( gamePiece->posX + tileSize.w) > windowWidth )
				gamePiece->posX = windowWidth - tileSize.w;

			if ( gamePiece->posX  <= 0  )
				gamePiece->posX = 0;

			prevMotion = event.motion;
		}

		renderer.Render( );
		timer.GetDelta( delta );

		if ( timer.IsUpdateTime() )
		{
			//gamePiece->posY += tileSize;

			//if ( gamePiece->posY > 540 )	gamePiece->posY = 0;
		}

	}
}
