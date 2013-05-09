#include "Renderer.h"

Renderer::Renderer()
	:	SCREEN_WIDTH( 1920 / 2 )
	,	SCREEN_HEIGHT( 1280 / 2 )
	,	SCREEN_BPP ( 32 )

	,	tile(  NULL )
	,	backgroundArea(  NULL )
	,	background(  NULL )
	,	gameArea( NULL )
	,	screen(  NULL )
	,	gamePieceList( { } )
	,	tileSize( 0 )
{

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
	SDL_WM_SetCaption( "Hello World", NULL );

	LoadAllTextures();
	BlitBackground();

	return true;
}

bool Renderer::Render( )
{
	BlitBackground();
	BlitForeground();

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
	tile = LoadImage( "media/tiles/blue_30x30.png" );
	background= LoadImage( "media/background.bmp" );

	backgroundArea = SDL_CreateRGBSurface( 0, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, 0x00ff0000, 0, 0, 0);

	tileSize = tile->clip_rect.h;

	// Create game area surfae. ( Set to size of 10 * tile width, 30 * tile height )
	gameArea = SDL_CreateRGBSurface( 0, tileSize * 10, tileSize * 20, SCREEN_BPP, 0x00, 0x00, 0x0, 0x00 );
	FillSurface ( gameArea, 0x66, 0x66, 0x66);
	//Uint32 colorKey = SDL_MapRGB( gameArea->format, 0x77, 0x77, 0x77 );
	//SDL_SetColorKey( gameArea, SDL_SRCCOLORKEY, colorKey );

	//SDL_SetAlpha( gameArea, SDL_SRCALPHA, 0 );
}

void Renderer::BlitBackground()
{
	ApplySurface( 0, 0, background, backgroundArea );
	ApplySurface( 320, 0, background, backgroundArea );
	ApplySurface( 0, 240, background, backgroundArea );
	ApplySurface( 320, 240, background, backgroundArea );

	ApplySurface( 0, 0, backgroundArea, screen );
}

void Renderer::BlitForeground()
{
	for ( std::shared_ptr< GamePiece > gp : gamePieceList )
	{

		ApplySurface( gp->posX, gp->posY, tile, gameArea );
	}

	// Apply game area ( with tiles ) to screen.
	ApplySurface( 50, 50, gameArea, screen );

	// Clear background. TODO : replace with pic.
	FillSurface ( gameArea, 0x66, 0x66, 0x66 );
}
