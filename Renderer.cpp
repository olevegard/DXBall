#include "Renderer.h"

Renderer::Renderer()
	:	SCREEN_WIDTH( 1920 / 2 )
	,	SCREEN_HEIGHT( 1280 / 2 )
	,	SCREEN_BPP ( 32 )

	,	tile(  NULL )
	,	ball( NULL )
	,	backgroundArea(  NULL )
	,	background(  NULL )
	,	gameArea( NULL )
	,	screen(  NULL )
	,	gamePieceList( { } )
	,	tileSize(  )
	,	screenSize()
{
	screenSize.x = 0;
	screenSize.y = 0;
	screenSize.w = 1920 / 2;
	screenSize.h = 1280 / 2;
}

Renderer::~Renderer()
{

}
bool Renderer::Init()
{
	// Set up screen
	screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

	if ( screen == NULL )
		return false;

	// Set window title
	LoadAllTextures();

	BlitBackground();

	return true;
}

bool Renderer::Render( )
{
	BlitBackground();
	BlitForeground();

	ApplySurface( 0, 0, backgroundArea, screen );

	if ( SDL_Flip( screen ) == -1 )
		return false;
	else
		return true;
}

void Renderer::AddObject( std::shared_ptr< GamePiece >  &gamePiece )

{
	gamePieceList.push_back( gamePiece );

}

SDL_Surface* Renderer::LoadImage( const std::string &filename )
{
	// Temporary stoare for the iamge that's loaded
	SDL_Surface* loadedImage = NULL;

	// The optimized image that will be used
	SDL_Surface* optimizedImage = NULL;

	// Load the image
	loadedImage = IMG_Load( filename.c_str() );

	// If the image loaded
	if ( loadedImage != NULL )
	{
		//
		// Create an optimized image
		optimizedImage = SDL_DisplayFormat( loadedImage );

		// Free the old image
		SDL_FreeSurface( loadedImage );
	}

	// Return the optimized image
	return optimizedImage;
}

void Renderer::SetColorKey( SDL_Surface* source, int r, int g, int b )
{
	if ( source )
	{
		Uint32 colorKey = SDL_MapRGB( source->format, r, g, b );

		SDL_SetColorKey( source, SDL_SRCCOLORKEY, colorKey );
	}
}

void Renderer::FillSurface( SDL_Surface* source, int r, int g, int b )
{

	SDL_FillRect( source, NULL, SDL_MapRGBA( source->format, r, g, b, 0 ) );
}

void Renderer::ApplySurface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{
	// Make a temp rect to hold the offsets
	SDL_Rect offset;

	// Give the offsets of the rectangle
	offset.x = x;
	offset.y = y;

	// Blit the surface
	SDL_BlitSurface( source, NULL, destination, &offset );
}

void Renderer::LoadAllTextures( )
{
	//tile = LoadImage( "hello.bmp" );
	tile = LoadImage( "media/paddles/paddle30x120.png" );
	SetColorKey( tile, 0xff,0xff,0xff );
	tileSize = tile->clip_rect;

	ball = LoadImage( "media/ball.png" );
	SetColorKey( ball, 0xff,0xff,0xff );

	background= LoadImage( "media/background.png" );

	backgroundArea = SDL_CreateRGBSurface( 0, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, 0x00ff0000, 0, 0, 0);

	// Create game area surfae. ( Set to size of 10 * tile width, 30 * tile height )
	gameArea = SDL_CreateRGBSurface( 0, SCREEN_WIDTH, tileSize.h, SCREEN_BPP, 0x00, 0x00, 0x0, 0x00 );
	FillSurface ( gameArea, 0x66, 0x66, 0x66);
	//Uint32 colorKey = SDL_MapRGB( gameArea->format, 0x77, 0x77, 0x77 );
	//SDL_SetColorKey( gameArea, SDL_SRCCOLORKEY, colorKey );

	//SDL_SetAlpha( gameArea, SDL_SRCALPHA, 0 );
}

void Renderer::BlitBackground()
{
	FillSurface ( backgroundArea, 0x66, 0x66, 0x66 );
	ApplySurface( 0, 0, background, backgroundArea );
}

void Renderer::BlitForeground()
{
	for ( std::shared_ptr< GamePiece > gp : gamePieceList )
	{
		if ( gp->textureType != 0 )
		{
			ApplySurface( gp->rect.x, gp->rect.y, tile, backgroundArea );
		}
		else
		{
			ApplySurface( gp->rect.x, gp->rect.y, ball, backgroundArea );
		}

	}

	// Apply game area ( with tiles ) to screen.
	//ApplySurface( 0, SCREEN_HEIGHT - gameArea->h, gameArea, screen );

	// Clear background. TODO : replace with pic.
	//FillSurface ( gameArea, 0x66, 0x66, 0x66 );
}
