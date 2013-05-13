#include <iostream>
#include <string>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "Renderer.h"
#include "Timer.h"
#include "Ball.h"

Renderer renderer;
Timer timer;

struct GamePiece;
int main( int argc, char* args[] )
{

	timer.Restart();

	if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
		return 1;

	if ( !renderer.Init() )
		return 1;

	renderer.RenderText( "DX Ball" );
	std::shared_ptr< GamePiece > gamePiece( new GamePiece() );
	gamePiece->textureType = 1;
	gamePiece->rect.y = 610;
	gamePiece->rect.w = 120;
	gamePiece->rect.h = 30;
	

	renderer.AddObject( gamePiece );

	std::shared_ptr< GamePiece > ball( new Ball() );
	ball->textureType = 0;
	renderer.AddObject( ball );

	Ball* pBall = dynamic_cast< Ball* > (ball.get() );

	bool quit = false;
	SDL_Event event;
	
	SDL_Rect tileSize = renderer.GetTileSize();
	SDL_Rect windowSize = renderer.GetWindowSize();

	SDL_WM_GrabInput( SDL_GRAB_FULLSCREEN );
	SDL_MouseMotionEvent prevMotion;
	int halfTileWidth = tileSize.w / 2;
	int deathCounter = 0;
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
						gamePiece->rect.x += tileSize.w;
						break;
					case SDLK_LEFT:
						gamePiece->rect.x -= tileSize.w;
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
				gamePiece->rect.x = event.motion.x - halfTileWidth;

			if ( ( gamePiece->rect.x + tileSize.w) > windowSize.w )
				gamePiece->rect.x = windowSize.w - tileSize.w;

			if ( gamePiece->rect.x  <= 0  )
				gamePiece->rect.x = 0;

			prevMotion = event.motion;
		}

		pBall->update( timer.GetDelta( ));
		pBall->BoundCheck( windowSize );
		pBall->PaddleCheck( gamePiece->rect );

		if ( pBall->DeathCheck( windowSize ) )
		{
			std::cout << "Death : " << ++deathCounter << std::endl;
		}
		renderer.Render( );
	}
}
