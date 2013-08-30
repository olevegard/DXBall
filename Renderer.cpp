#include "Renderer.h"


#include "Ball.h"
#include "Tile.h"
#include "Paddle.h"

#include <algorithm>

	Renderer::Renderer()
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	:	rmask( 0xff000000 )
	,	gmask( 0x00ff0000 )
	,	bmask( 0x0000ff00 )
	,	amask( 0x000000ff )
#else
	:	rmask( 0x000000ff )
	,	gmask( 0x0000ff00 )
	,	bmask( 0x00ff0000 )
	,	amask( 0xff000000 )
#endif

	,	SCREEN_WIDTH ( 1920 / 2 )
	,	SCREEN_HEIGHT( 1280 / 2 )
	,	SCREEN_BPP ( 32 )

	,	textures()

	,	backgroundArea(  NULL )
	,	backgroundImage(  NULL )
	,	screen(  NULL )
	,	ballList(  )
	,	font()
	,	bigFont()
	,	textColor{ 0, 140, 0, 255 }

	,	localPlayerText()
	,	localPlayerLives()
	,	localPlayerPoints()
	,	localPlayerCaption()

	,	remotePlayerText()
	,	remotePlayerLives()
	,	remotePlayerPoints()
	,	remotePlayerCaption()

	,	tileColors{ {48, 9, 178, 255}, {255, 55, 13, 255}, {0, 0, 0, 255}, {255, 183, 13, 255} }
	,	tileSurfaces{ nullptr, nullptr, nullptr, nullptr }
	,	hardTileColors{ { 255, 243, 233, 255}, { 222, 212, 203, 255}, { 191, 183, 175, 255},{ 127, 122, 117, 255}, { 64, 61, 58, 255} }
	,	hardTileSurfaces{ nullptr, nullptr, nullptr, nullptr, nullptr }
{
	background.x = 0.0;
	background.y = 0.0;
	background.w = SCREEN_WIDTH;
	background.h = SCREEN_HEIGHT;
}

Renderer::~Renderer()
{
	CleanUp();
	QuitSDL();
}
bool Renderer::Init()
{
	// Set up screen
	screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, 0, SDL_HWSURFACE | SDL_DOUBLEBUF );

	SDL_WM_SetCaption( "DX Ball", nullptr );

	if ( screen == NULL )
		return false;

	if ( TTF_Init( ) == -1 )
		return false;

	if ( !LoadImages() )
		return false;

	if ( !LoadFontAndText() )
		return false;

	BlitBackground();

	return true;
}
void Renderer::SetLocalPaddle( std::shared_ptr< Paddle >  &paddle )
{
	localPaddle = paddle;
	textures[GamePiece::Paddle] = InitSurface( localPaddle->rect );
	FillSurface( textures[GamePiece::Paddle], 120, 0, 120 );
}

void Renderer::AddBall( const std::shared_ptr< Ball > &ball )
{
	ballList.push_back( ball );
}

void Renderer::RemoveBall(  const std::shared_ptr< Ball > &ball )
{
	ballList.erase( std::find( ballList.begin(), ballList.end(), ball ) );
}

void Renderer::AddTile( const std::shared_ptr< Tile > &tile )
{
	tileList.push_back( tile );
}
void Renderer::RemoveTile( const std::shared_ptr< Tile >  &tile )
{
	tileList.erase( std::find( tileList.begin(), tileList.end(), tile) );
}
SDL_Surface* Renderer::LoadImage( const std::string &filename, GamePiece::TextureType textureType )
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
		// Create an optimized image
		optimizedImage = SDL_DisplayFormat( loadedImage );

		SetArrayData( textureType, optimizedImage );

		// Free the old image
		SDL_FreeSurface( loadedImage );
	}

	// Return the optimized image
	return optimizedImage;
}

TTF_Font* Renderer::LoadFont( const std::string &fontName, int fontSize ) const
{
	TTF_Font* tempFont = TTF_OpenFont( fontName.c_str(), fontSize );

	if ( tempFont == nullptr )
	{
		std::cout << "Failed to open font : " << fontName << std::endl;
	}

	return tempFont;

}
void Renderer::SetColorKey( SDL_Surface* source, unsigned char r, unsigned char g, unsigned char b )
{
	if ( source )
	{
		Uint32 colorKey = SDL_MapRGB( source->format, r, g, b );

		SDL_SetColorKey( source, SDL_SRCCOLORKEY, colorKey );
	}
}

void Renderer::SetColorKey( GamePiece::TextureType textureID, unsigned char r, unsigned char g, unsigned char b )
{
	SetColorKey( textures[ textureID ], r, g, b );
}

void Renderer::FillSurface( SDL_Surface* source, unsigned char r, unsigned char g, unsigned char b )
{
	SDL_FillRect( source, NULL, SDL_MapRGBA( source->format, r, g, b, 255 )  );
}
	
void Renderer::FillSurface( SDL_Surface* source, const SDL_Color &color )
{
	FillSurface( source, color.r, color.g, color.b );
}

SDL_Surface* Renderer::InitSurface( const Rect &rect ) const
{
	return InitSurface( 0, static_cast< int > ( rect.w ), static_cast< int > ( rect.h ) );
}
SDL_Surface* Renderer::InitSurface( unsigned int flags, int width, int height ) const
{
	SDL_Surface* surface = SDL_CreateRGBSurface( flags, width, height, SCREEN_BPP, rmask, gmask, bmask, amask);

	surface = SetDisplayFormat( surface );

	return surface;
}
SDL_Surface* Renderer::InitSurface(GamePiece::TextureType textureType, unsigned int flags, int width, int height )
{
	SDL_Surface* surface = InitSurface( flags, width, height );

	SetArrayData( textureType, surface );

	return surface;
}

SDL_Surface* Renderer::RenderTextSurface_Solid(  TTF_Font* textFont, const std::string &textToRender, const SDL_Color &color )
{
	SDL_Surface* surface = TTF_RenderText_Solid( textFont, textToRender.c_str(), color );

	if ( surface != nullptr )
	{
		SetDisplayFormat( surface );
	} else
	{
		std::cout << "Failed to create text surface...\n";
	}

	return surface;
}

void Renderer::SetArrayData( GamePiece::TextureType textureType, SDL_Surface* surface )
{
	textures[ textureType ] = surface;
}

SDL_Surface* Renderer::SetDisplayFormat( SDL_Surface* surface ) const
{
	if ( surface )
	{
		surface = SDL_DisplayFormat( surface );
	} else 
	{
		std::cout << "Cannot set display format : nullptr\n";
	}

	return surface;
}
void Renderer::ApplySurface( short x, short y, SDL_Surface* source, SDL_Surface* destination ) const
{
	// Make a temp rect to hold the offsets
	SDL_Rect offset;

	// Give the offsets of the rectangle
	offset.x = x;
	offset.y = y;

	// Blit the surface
	SDL_BlitSurface( source, NULL, destination, &offset );
}
void Renderer::ApplySurface( const Rect &r, SDL_Surface* source, SDL_Surface* destination ) const
{
	ApplySurface( static_cast<short> ( r.x ), static_cast<short> ( r.y ), source, destination );
}

bool Renderer::LoadImages()
{
	//LoadImage( "media/paddles/paddle30x120.png", GamePiece::Paddle );
	//SetColorKey( GamePiece::Paddle, 0xff,0xff,0xff );

	backgroundImage = InitSurface( GamePiece::Background, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
	FillSurface( backgroundImage, 0, 0, 0 );

	LoadImage( "media/ball.png", GamePiece::Ball );
	SetColorKey( GamePiece::Ball, 0xff,0xff,0xff );

	backgroundImage = InitSurface( GamePiece::Background, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
	FillSurface( backgroundImage, 0, 0, 0 );

	backgroundArea = InitSurface( 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	std::cout << "Adding tile surfaces : " << std::endl;
	for ( size_t i = 0; i < tileSurfaces.size() ; ++i )
		SetTileColorSurface( i, tileColors[ i ], tileSurfaces );

	std::cout << "Adding hard tile surfaces : " << std::endl;
	for ( size_t i = 0; i < hardTileSurfaces.size() ; ++i )
		SetTileColorSurface( i, hardTileColors[ i ], hardTileSurfaces );

	return true;
}

bool Renderer::LoadFontAndText()
{
	font = LoadFont( "media/fonts/sketchy.ttf", 28 );

	bigFont = LoadFont( "media/fonts/sketchy.ttf", 57 );

	if ( bigFont == nullptr || font == nullptr )
	{
		std::cout << "Fonts not initialized properly\n";
		return false;
	}

	localPlayerCaption = RenderTextSurface_Solid( font, "Local player : ", textColor );

	return true;
}

void Renderer::SetTileColorSurface( size_t index, const SDL_Color &color, std::vector< SDL_Surface* > &list  )
{
	SDL_Surface* surf = InitSurface( 0, 60, 20 );

	FillSurface( surf, color );
	list.at( index ) = surf;
}

bool Renderer::Render( )
{
	BlitBackground();
	BlitForeground();
	BlitText();

	ApplySurface( 0, 0, backgroundArea, screen );

	if ( SDL_Flip( screen ) == -1 )

		return false;
	else
		return true;
}

void Renderer::BlitBackground() const
{
	ApplySurface( 0, 0, backgroundImage, backgroundArea );
}

void Renderer::BlitForeground()
{
	// Draw balls
	for ( std::shared_ptr< Ball > gp : ballList )
	{
		ApplySurface( gp->rect, textures[GamePiece::Ball], backgroundArea );
	}

	// Draw tiles
	for ( std::shared_ptr< Tile > gp : tileList)
	{
		if ( gp->GetTileType() == TileTypes::Hard )
			ApplySurface( gp->rect, hardTileSurfaces[ 5 - gp->GetHitsLeft()], backgroundArea );
		else
			ApplySurface( gp->rect, tileSurfaces[ gp->GetTileTypeAsIndex() ], backgroundArea );
	}

	// Draw paddles
	ApplySurface( localPaddle->rect, textures[GamePiece::Paddle], backgroundArea );
}

void Renderer::BlitText()
{
	Rect screenSize = GetWindowSize();
	short yPosLives  = static_cast<short> ( localPlayerCaption->h ) + 5;
	short yPosPoints = yPosLives + static_cast<short> ( localPlayerLives->h + 5 );

	short xPos = 0;

	if ( localPlayerLives && localPlayerPoints )
	{
		ApplySurface( 10, yPosLives , localPlayerLives, backgroundArea );
		ApplySurface( 10, yPosPoints, localPlayerPoints, backgroundArea );
	}

	if ( remotePlayerLives && remotePlayerPoints )
	{
		xPos = static_cast< short > ( screenSize.w  - remotePlayerPoints->w - 10 );
		ApplySurface( xPos, yPosLives, remotePlayerLives, backgroundArea );
		ApplySurface( xPos, yPosPoints, remotePlayerPoints, backgroundArea );
	}

	if ( localPlayerText )
	{
		screenSize.x = ( ( screenSize.w / 2 ) - ( localPlayerText->w / 2 ) );
		screenSize.y =  screenSize.h / 2;
		ApplySurface( screenSize, localPlayerText, backgroundArea );
	}

	if ( localPlayerCaption )
		ApplySurface( 0, 0, localPlayerCaption, backgroundArea );

	if ( remotePlayerCaption )
	{
		xPos = static_cast< short > ( screenSize.w - remotePlayerCaption->w );
		ApplySurface( xPos, 0, remotePlayerCaption, backgroundArea );
	}
}
void Renderer::RenderText( const std::string &textToRender, const Player &player  )
{
	if ( player == Player::Local )
	{
		SDL_FreeSurface( localPlayerText );
		localPlayerText = RenderTextSurface_Solid( bigFont, textToRender.c_str(), textColor );
	}
}
void Renderer::RenderLives( unsigned long lifeCount, const Player &player  )
{
	std::stringstream ss;
	ss << "Lives : " << lifeCount;

	if ( player == Player::Local )
	{
		SDL_FreeSurface( localPlayerLives );
		localPlayerLives = RenderTextSurface_Solid( font, ss.str().c_str(), textColor );
	} else if ( player == Player::Remote )
	{
		SDL_FreeSurface( remotePlayerLives );
		remotePlayerLives = RenderTextSurface_Solid( font, ss.str().c_str(), textColor );
	}
}

void Renderer::RenderPoints( unsigned int pointCount, const Player &player  )
{
	std::stringstream ss;
	ss << "Points : " << pointCount;

	if ( player == Player::Local )
	{
		SDL_FreeSurface( localPlayerPoints );
		localPlayerPoints = RenderTextSurface_Solid( font, ss.str().c_str(), textColor );
	} else if ( player == Player::Remote )
	{
		SDL_FreeSurface( remotePlayerPoints );
		remotePlayerPoints = RenderTextSurface_Solid( font, ss.str().c_str(), textColor );
	}
}
void Renderer::RenderPlayerCaption( const std::string textToRender, const Player &player  )
{
	if ( player == Player::Local )
	{
		SDL_FreeSurface( localPlayerCaption );
		localPlayerCaption = RenderTextSurface_Solid( bigFont, textToRender.c_str(), textColor );
	} else if ( player == Player::Remote )
	{
		SDL_FreeSurface( remotePlayerCaption );
		remotePlayerCaption = RenderTextSurface_Solid( bigFont, textToRender.c_str(), textColor );
	}
}
void Renderer::RemoveText()
{
	localPlayerText = nullptr;
}
Rect Renderer::GetTileSize()
{
	return localPaddle->rect;
}
Rect Renderer::GetWindowSize()
{
	return background;
}
void Renderer::CleanUp()
{
	CleanUpSurfaces();
	CleanUpLists();
	CleanUpTTF();
}
void Renderer::CleanUpSurfaces()
{
	// Free tile surfaces
	for ( auto p : tileSurfaces )
	{
		SDL_FreeSurface( p );
	}

	for ( auto p : hardTileSurfaces )
	{
		SDL_FreeSurface( p );
	}

	// Free ball/paddle
	SDL_FreeSurface( textures[GamePiece::Paddle] );
	SDL_FreeSurface( textures[GamePiece::Ball] );

	// Free background/screen surfaces
	SDL_FreeSurface( backgroundArea );
	SDL_FreeSurface( backgroundImage );
	SDL_FreeSurface (screen );

	// Free text surfaces
	SDL_FreeSurface( localPlayerText );
	SDL_FreeSurface( localPlayerLives );
	SDL_FreeSurface( localPlayerPoints );
	SDL_FreeSurface( localPlayerCaption );
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
