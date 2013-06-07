#include <iostream>
#include <string>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "Renderer.h"
#include "GamePiece.h"
#include "GameManager.h"
#include "Timer.h"
#include "Ball.h"
#include "Paddle.h"

GameManager gameMan;
int main( int argc, char* args[] )
{

	gameMan.Init();

	gameMan.Run();
/*
	if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
		return 1;

	if ( !renderer.Init() )
		return 1;

	std::shared_ptr< GamePiece > gamePiece( new GamePiece() );
	gamePiece->textureType = GamePiece::Paddle;
	gamePiece->rect.y = 610;
	gamePiece->rect.w = 120;
	gamePiece->rect.h = 30;
	

	std::shared_ptr< GamePiece > ball( new Ball() );

	ball->textureType = GamePiece::Ball;

	Ball* pBall = dynamic_cast< Ball* > (ball.get() );

	bool quit = false;
	SDL_Event event;
	
	SDL_Rect tileSize = renderer.GetTileSize();
	SDL_Rect windowSize = renderer.GetWindowSize();

	//SDL_WM_GrabInput( SDL_GRAB_FULLSCREEN );

	int halfTileWidth = tileSize.w / 2;
	int lives = 5;

	renderer.RenderLives( lives );
	renderer.RenderPoints( 123 );
	renderer.RenderText( "Press enter to start");
	renderer.AddObject( gamePiece );
	renderer.AddObject( ball );

	bool started = false;

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
					case SDLK_RETURN:
						started = true;
						break;
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
		}

		float delta = timer.GetDelta( );

		if ( started )
		{
			renderer.RemoveText();
			pBall->Update( delta );
			pBall->BoundCheck( windowSize );
			pBall->PaddleCheck( gamePiece->rect );
		} else
		{
			renderer.RenderText( "Press enter to start");
		}

		if ( pBall->DeathCheck( windowSize ) )
		{
			started = false;
			renderer.RenderLives( --lives );
			pBall->rect.x = 200;
			pBall->rect.y = 20;
		}

		renderer.Render( );
	}
*/
}
