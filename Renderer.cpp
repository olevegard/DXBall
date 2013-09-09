#include "Renderer.h"

#include "Ball.h"
#include "Tile.h"
#include "Paddle.h"

#include <sstream>
#include <iostream>
#include <algorithm>

#include <SDL2/SDL.h>

	Renderer::Renderer()
	:	window( nullptr )
	,	renderer( nullptr )
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	,	rmask( 0xff000000 )
	,	gmask( 0x00ff0000 )
	,	bmask( 0x0000ff00 )
	,	amask( 0x000000ff )
#else
	,	rmask( 0x000000ff )
	,	gmask( 0x0000ff00 )
	,	bmask( 0x00ff0000 )
	,	amask( 0xff000000 )
#endif

	,	background{ 0, 0, 1920 / 2, 1080 / 2 }
	,	SCREEN_BPP ( 32 )
	,	screenFlags( SDL_WINDOW_OPENGL  )
	,	fullscreen( false )

	,	tileColors{ {48, 9, 178, 255}, {255, 55, 13, 255}, {0, 0, 0, 255}, {255, 183, 13, 255} }
	,	tileTextures{ nullptr, nullptr, nullptr, nullptr }
	,	hardTileColors{ { 255, 243, 233, 255}, { 222, 212, 203, 255}, { 191, 183, 175, 255},{ 127, 122, 117, 255}, { 64, 61, 58, 255} }
	,	hardTileTextures{ nullptr, nullptr, nullptr, nullptr, nullptr }

	,	ballList(  )
	,	tileList(  )
	,	localPaddle( )

	,	textures()

	,	font()
	,	bigFont()
	,	textColor{ 0, 140, 0, 255 }

	,	localPlayerTextTexture( )
	,	localPlayerTextRect( )
	,	localPlayerTextValue( "" )

	,	localPlayerCaptionTexture( )
	,	localPlayerCaptionRect( )
	,	localPlayerCaptionValue( "")

	,	localPlayerLivesTexture( )
	,	localPlayerLivesRect()
	,	localPlayerLivesValue( 0 )

	,	localPlayerPointsTexture( )
	,	localPlayerPointsRect( )
	,	localPlayerPointsValue( 0 )
{
}

Renderer::~Renderer()
{
	CleanUp();

	QuitSDL();
}
bool Renderer::Init( const SDL_Rect &rect, bool startFS )

{
	fullscreen = startFS;
	background = rect;

	if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
	{
		std::cout << "Failed to initialize SDL : " << SDL_GetError() << std::endl;
		return false;
	}

	// Set up screen
	if ( !CreateWindow() )
	{
		std::cout << "Failed to apply video mode\n";
		return false;
	}

	if ( !CreateRenderer() )
	{
		std::cout << "Failed to apply video mode\n";
		return false;
	}

	Setup();

	LoadImages();

	
	if ( TTF_Init( ) == -1 )
	{
		std::cout << "Failed to initialize TTF : " << TTF_GetError() << std::endl;
		return false;
	}

	if ( !LoadFontAndText() )
		return false;


	std::cout << "Init done\n";
	return true;
}
bool Renderer::LoadImages()
{
	textures[ GamePiece::Ball ] = InitSurface( background.w, background.h, 0, 0, 0);//SDL_CreateTextureFromSurface( renderer, surf );

	std::cout << "Adding tile surfaces : " << std::endl;
	for ( size_t i = 0; i < tileTextures.size() ; ++i )
		SetTileColorSurface( i, tileColors[ i ], tileTextures );

	std::cout << "Adding hard tile surfaces : " << std::endl;
	for ( size_t i = 0; i < hardTileTextures.size() ; ++i )
		SetTileColorSurface( i, hardTileColors[ i ], hardTileTextures );

	return true;
}
// ============================================================================================
// ===================================== Setup ================================================
// ============================================================================================
bool Renderer::CreateRenderer()
{
	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );//SDL_RENDERER_PREVENTVSYNC 

	if ( renderer == nullptr )
	{
		std::cout << "Could not create renderer!";
		return false;
	}

	SDL_RenderSetLogicalSize( renderer, background.w, background.h );

	SDL_SetRenderDrawColor( renderer, 70, 20, 70, 255 );
	SDL_RenderClear( renderer );
	SDL_RenderPresent( renderer );

	return true;
}
void Renderer::Setup()
{
	SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	// Hide cursor
	SDL_ShowCursor( SDL_DISABLE );
}
bool Renderer::CreateWindow()
{
	window = SDL_CreateWindow( "DX Ball", 0, 0, background.w, background.h, screenFlags );

	if ( window == nullptr )
	{
		std::cout << "Failed to apply video mode\n";
		return false;
	}

	return true;
}
void Renderer::ToggleFullscreen()
{
	SetFullscreen( !fullscreen );
}
bool Renderer::SetFullscreen( bool fullscreenOn )
{

	std::cout << "Settting fullscreen...\n";
	fullscreen = fullscreenOn;

	if ( SDL_SetWindowFullscreen( window, (fullscreen ) ? SDL_WINDOW_FULLSCREEN : 0 ) )
	{
		std::cout << "Failed to set fullscreen mode to " << std::boolalpha << fullscreen << std::endl;
		return false;
	}
	std::cout << "Error : " << SDL_GetError() << std::endl;
	return true;
}
// ============================================================================================
// ================================ Texture helpers ===========================================
// ============================================================================================

void Renderer::FillSurface( SDL_Surface* source, unsigned char r, unsigned char g, unsigned char b ) const
{
	SDL_FillRect( source, NULL, SDL_MapRGBA( source->format, r, g, b, 255 )  );
}

void Renderer::FillSurface( SDL_Surface* source, const SDL_Color &color ) const
{
	FillSurface( source, color.r, color.g, color.b );
}

SDL_Texture* Renderer::InitSurface( const Rect &rect, unsigned char r, unsigned char g, unsigned char b) const
{
	return InitSurface( static_cast< int > ( rect.w ), static_cast< int > ( rect.h ), r, g, b);
}

SDL_Texture* Renderer::InitSurface( int width, int height, unsigned char r, unsigned char g, unsigned char b ) const
{
	SDL_Surface* surface = SDL_CreateRGBSurface( 0, width, height, SCREEN_BPP, rmask, gmask, bmask, amask);

	FillSurface( surface, r, g, b );

	SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, surface );
	SDL_FreeSurface( surface );

	return texture;
}
SDL_Texture* Renderer::LoadImage( const std::string &filename, GamePiece::TextureType textureType )
{
	// Temporary stoare for the iamge that's loaded
	SDL_Surface* loadedImage = nullptr;

	// Load the image
	//loadedImage = IMG_Load( filename.c_str() );

	SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, loadedImage );

	// If the image loaded
	if ( loadedImage != nullptr )
	{
		SetArrayData( textureType, texture );

	} else
	{
		std::cout << "Failed to load " << filename << std::endl;
	}

	// Free the old image
	SDL_FreeSurface( loadedImage );

	// Return the optimized image
	return texture;
}
void Renderer::SetArrayData( GamePiece::TextureType textureType, SDL_Texture* texture )
{
	textures[ textureType ] = texture;
}
SDL_Surface* Renderer::SetDisplayFormat( SDL_Surface* surface ) const
{
	if ( surface )
	{
		//surface = SDL_DisplayFormat( surface );
	} else 
	{
		std::cout << "Cannot set display format : nullptr\n";
	}

	return surface;
}
void Renderer::SetTileColorSurface( size_t index, const SDL_Color &color, std::vector< SDL_Texture* > &list  )
{
	SDL_Texture *text = InitSurface(  60, 20, color.r, color.g, color.b );//SDL_CreateTextureFromSurface( renderer, surf );
	list.at( index ) = text;
}
// ============================================================================================
// ============================= Add / Rmove objects ==========================================
// ============================================================================================
void Renderer::AddTile( const std::shared_ptr< Tile > &tile )
{
	tileList.push_back( tile );
}
void Renderer::RemoveTile( const std::shared_ptr< Tile >  &tile )
{
	tileList.erase( std::find( tileList.begin(), tileList.end(), tile) );
}
void Renderer::AddBall( const std::shared_ptr< Ball > &ball )
{
	ballList.push_back( ball );
}

void Renderer::RemoveBall(  const std::shared_ptr< Ball > &ball )
{
	ballList.erase( std::find( ballList.begin(), ballList.end(), ball ) );
}

void Renderer::SetLocalPaddle( std::shared_ptr< Paddle >  &paddle )
{
	localPaddle = paddle;

	SDL_Texture* surf = InitSurface( localPaddle->rect, 120, 0, 120 );
	textures[ GamePiece::Paddle ] = surf;
}
// ============================================================================================
// ================================= Renderering ==============================================
// ============================================================================================
void Renderer::Render( )
{
	SDL_RenderClear( renderer );

	RenderForeground();
	RenderText();

	SDL_RenderPresent( renderer );
}
void Renderer::RenderForeground()
{

	// Draw balls
	for ( std::shared_ptr< Ball > gp : ballList )
	{
		SDL_Rect r = gp->rect.ToSDLRect( );
		SDL_RenderCopy( renderer, textures[ GamePiece::Ball ], nullptr, &r );
	}

	// Draw tiles
	for ( std::shared_ptr< Tile > gp : tileList)
	{
		SDL_Rect r = gp->rect.ToSDLRect();
		if ( gp->GetTileType() == TileTypes::Hard )
			SDL_RenderCopy( renderer, hardTileTextures[ 5 - gp->GetHitsLeft()], nullptr, &r );
		else
			SDL_RenderCopy( renderer, tileTextures[ gp->GetTileTypeAsIndex() ], nullptr, &r );
	}

	// Draw paddles
	SDL_Rect r = localPaddle->rect.ToSDLRect();
	SDL_RenderCopy( renderer, textures[ GamePiece::Ball ], nullptr, &r );

}
void Renderer::RenderText()
{
	if ( localPlayerTextTexture )
		SDL_RenderCopy( renderer, localPlayerTextTexture, nullptr, &localPlayerTextRect  );

	if ( localPlayerCaptionTexture )
		SDL_RenderCopy( renderer, localPlayerCaptionTexture , nullptr, &localPlayerCaptionRect  );

	if ( localPlayerLivesTexture  )
		SDL_RenderCopy( renderer, localPlayerLivesTexture, nullptr, &localPlayerLivesRect );

	if ( localPlayerPointsTexture  )
		SDL_RenderCopy( renderer, localPlayerPointsTexture, nullptr, &localPlayerPointsRect);
}
// ==============================================================================================
// ================================= Text handling ==============================================
// ==============================================================================================
TTF_Font* Renderer::LoadFont( const std::string &fontName, int fontSize ) const
{
	TTF_Font* tempFont = TTF_OpenFont( fontName.c_str(), fontSize );

	if ( tempFont == nullptr )
	{
		std::cout << "Failed to open font : " << fontName << " : " << TTF_GetError() << std::endl;
	}

	return tempFont;

}

bool Renderer::LoadFontAndText()
{
	font = LoadFont( "media/fonts/sketchy.ttf", 28 );

	bigFont = TTF_OpenFont( "media/fonts/sketchy.ttf", 57 );

	if ( bigFont == nullptr || font == nullptr )
	{
		std::cout << "Fonts not initialized properly\n";
		return false;
	}

	return true;
}
SDL_Texture* Renderer::RenderTextTexture_Solid(  TTF_Font* textFont, const std::string &textToRender, const SDL_Color &color, SDL_Rect &rect )
{
	SDL_Surface* surface = TTF_RenderText_Solid( textFont, textToRender.c_str(), color );

	if ( surface != nullptr )
	{
		rect.w = surface->clip_rect.w;
		rect.h = surface->clip_rect.h;

		SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, surface );

		SDL_FreeSurface( surface );

		return texture;
	} else
	{
		std::cout << "Failed to create text surface..." << textFont << " " << textToRender << " \n";
		return nullptr;
	}
}
void Renderer::RenderText( const std::string &textToRender, const Player &player  )
{
	if ( player == Player::Local )
	{
		if (  localPlayerTextValue != textToRender || localPlayerTextTexture == nullptr )
		{
			localPlayerTextValue = textToRender;

			SDL_DestroyTexture( localPlayerTextTexture );
			localPlayerTextTexture = RenderTextTexture_Solid( bigFont, textToRender.c_str(), textColor, localPlayerTextRect );

			localPlayerTextRect.x = ( background.w / 2 ) - ( localPlayerTextRect.w / 2 );
			localPlayerTextRect.y = ( background.h / 2 ) - ( localPlayerTextRect.h / 2 );
		}
	}
}
void Renderer::RemoveText()
{
	localPlayerTextTexture = nullptr;
}
void Renderer::RenderPlayerCaption( const std::string textToRender, const Player &player  )
{
	if ( player == Player::Local )
	{
		if ( localPlayerCaptionValue != textToRender ||   localPlayerCaptionTexture == nullptr )
		{
			localPlayerCaptionValue  = textToRender;

			SDL_DestroyTexture( localPlayerCaptionTexture );
			localPlayerCaptionTexture = RenderTextTexture_Solid( bigFont, textToRender.c_str(), textColor, localPlayerCaptionRect );

			localPlayerCaptionRect.x = 0;
			localPlayerCaptionRect.y = 0;

			// Set remaning text rects based on caption rect
			localPlayerLivesRect.x = 40;
			localPlayerLivesRect.y = localPlayerCaptionRect.h - 10;

			localPlayerPointsRect.x = localPlayerLivesRect.x;
			localPlayerPointsRect.y = localPlayerLivesRect.y + localPlayerLivesRect.h ;
		}
	}
}
void Renderer::RenderLives( unsigned long lifeCount, const Player &player  )
{
	if ( player == Player::Local )
	{
		if ( localPlayerLivesValue != lifeCount ||   localPlayerLivesTexture == nullptr )
		{
			localPlayerLivesValue  = lifeCount;

			std::stringstream ss;
			ss << "Lives : " << lifeCount;

			SDL_DestroyTexture( localPlayerLivesTexture );
			localPlayerLivesTexture = RenderTextTexture_Solid( font, ss.str().c_str(), textColor, localPlayerLivesRect );
		}
	}
}
void Renderer::RenderPoints( unsigned long pointCount, const Player &player  )
{
	if ( player == Player::Local )
	{
		if ( localPlayerPointsValue != pointCount || localPlayerPointsTexture == nullptr )
		{
			localPlayerPointsValue  = pointCount;

			std::stringstream ss;
			ss << "Points : " << pointCount;

			SDL_DestroyTexture( localPlayerPointsTexture  );
			localPlayerPointsTexture = RenderTextTexture_Solid( font, ss.str().c_str(), textColor, localPlayerPointsRect  );
		}
	}
}

void Renderer::CleanUp()
{
	CleanUpSurfaces();
	CleanUpLists();
	CleanUpTTF();
}
void Renderer::CleanUpSurfaces()
{
	// Free tile 
	for ( auto p : tileTextures )
	{
		SDL_DestroyTexture( p );
	}

	for ( auto p : hardTileTextures )
	{
		SDL_DestroyTexture( p );
	}

	// Free ball/paddle
	SDL_DestroyTexture( textures[GamePiece::Paddle] );
	SDL_DestroyTexture( textures[GamePiece::Ball] );

	// Free text surfaces
	SDL_DestroyTexture( localPlayerTextTexture);
	SDL_DestroyTexture( localPlayerCaptionTexture);
	SDL_DestroyTexture( localPlayerLivesTexture);
	SDL_DestroyTexture( localPlayerPointsTexture);
}
void Renderer::CleanUpLists()
{
	ballList.empty();
	tileList.empty();
	textures.empty();
}
void Renderer::CleanUpTTF()
{
	TTF_CloseFont( font );
	TTF_CloseFont( bigFont );
}
void Renderer::QuitSDL()
{
	TTF_Quit();
	SDL_Quit();
}
void Renderer::PrintSDL_TTFVersion()
{
	SDL_version compile_version;
	SDL_TTF_VERSION(&compile_version);

	const SDL_version *link_version = TTF_Linked_Version();

	printf("Compiled with SDL_ttf version: %d.%d.%d\n", 
			compile_version.major,
			compile_version.minor,
			compile_version.patch);
	printf("Running with SDL_ttf version: %d.%d.%d\n", 
			link_version->major,
			link_version->minor,
			link_version->patch);
}
