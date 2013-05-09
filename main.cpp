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

	for  ( int i = 0; i < 5; ++i )
	{
		//renderer.AddObject( i * 5,  5) ;
		//renderer.AddObject( i * 5,  60 );
		gamePiece->posY += 20;
		renderer.Render( );

		std::cout << "count : " << i << std::endl;
		SDL_Delay( 1000 );
	}

}
