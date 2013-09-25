#include "Renderer.h"

#include "Ball.h"
#include "Tile.h"
#include "Paddle.h"
#include "enums/MainMenuItemType.h"
#include "enums/PauseMenuItemType.h"

#include <sstream>
#include <iostream>
#include <algorithm>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

	Renderer::Renderer()
	:	window( nullptr )
	,	renderer( nullptr )
	,	gameState( GameState::MainMenu )

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
	,	backgroundColor{ 0, 0, 0, 255 }// 40, 20, 40, 255
								/*uuuuunnuuuuuusesssd*/
	,	tileColors{ {48, 9, 178, 255}, {255, 55, 13, 255}, {140, 140, 140, 255}, {255, 183, 13, 255} }
	,	tileTextures{ nullptr, nullptr, nullptr, nullptr }
	,	hardTileColors{ { 255, 243, 233, 255}, { 222, 212, 203, 255}, { 191, 183, 175, 255},{ 127, 122, 117, 255}, { 64, 61, 58, 255} }
	,	hardTileTextures{ nullptr, nullptr, nullptr, nullptr, nullptr }

	,	ballList(  )
	,	tileList(  )
	,	localPaddle( nullptr )
	,	remotePaddle( nullptr )

	,	localPlayerColor{ 0, 140, 0,255 }
	,	localPlayerBallTexture( nullptr )
	,	localPlayerPaddle( nullptr )

	,	remotePlayerColor{ 140, 0, 0, 255 }
	,	remotePlayerBallTexture( nullptr )
	,	remotePlayerPaddle( nullptr )

	,	tinyFont()
	,	font()
	,	mediumFont()
	,	bigFont()

	,	textColor{ 0, 140, 140, 255 }

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

	,	localPlayerBallsTexture( )
	,	localPlayerBallsRect( )
	,	localPlayerBallsValue( 0 )

	,	remotePlayerCaptionTexture( )
	,	remotePlayerCaptionRect( )
	,	remotePlayerCaptionValue( "" )

	,	remotePlayerLivesTexture()
	,	remotePlayerLivesRect()
	,	remotePlayerLivesValue( 0 )

	,	remotePlayerPointsTexture( )
	,	remotePlayerPointsRect( )
	,	remotePlayerPointsValue( 0 )

	,	remotePlayerBallsTexture( )
	,	remotePlayerBallsRect( )
	,	remotePlayerBallsValue( 0 )

	,	margin( 30 )
	,	singlePlayerText     ( "Single Player" )
	,	multiplayerPlayerText( "Multiplayer"   )
	,	optionsButton        ( "Options"       )
	,	quitButton           ( "Quit"          )

	,	greyAreaTexture( nullptr )
	,	greyAreaRect( )

	,	pauseResumeButton( "Resume" )
	,	pauseMainMenuButton( "Main Menu" )
	,	pauseQuitButton( "Quit" )
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

	if ( TTF_Init( ) == -1 )
	{
		std::cout << "Failed to initialize TTF : " << TTF_GetError() << std::endl;
		return false;
	}

	if ( !LoadFontAndText() )
		return false;


	LoadImages();
	std::cout << "Init done\n";
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


	SDL_SetRenderDrawColor( renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a );
	SDL_RenderClear( renderer );
	SDL_RenderPresent( renderer );

	return true;
}
void Renderer::Setup()
{
	SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	HideMouseCursor( false );
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
void Renderer::SetGameState( const GameState &gs )
{
	gameState = gs;
	if ( gameState == GameState::InGame )
		HideMouseCursor( true );
	else
		HideMouseCursor( false );
}
void Renderer::HideMouseCursor( bool hideCursor )
{
	if ( hideCursor )
		SDL_ShowCursor( SDL_DISABLE );
	else
		SDL_ShowCursor( SDL_ENABLE );
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
bool Renderer::LoadImages()
{
	localPlayerBallTexture  = InitSurface( background.w, background.h, localPlayerColor );
	remotePlayerBallTexture = InitSurface( background.w, background.h, remotePlayerColor );

	// Menu mode
	mainMenuBackground = InitSurface( background.w, background.h, 0, 0, 0 );
	//mainMenuBackground = LoadImage( "media/background.png" );

	mainMenuCaptionTexture = RenderTextTexture_Blended( hugeFont, "DX Ball", textColor, mainMenuCaptionRect );
	mainMenuCaptionRect.x = ( background.w / 2 ) - ( mainMenuCaptionRect.w / 2 );
	mainMenuCaptionRect.y = 0;

	mainMenuSubCaptionTexture = RenderTextTexture_Blended( mediumFont, "A quite simple clone made by a weird guy", textColor, mainMenuSubCaptionRect );
	mainMenuSubCaptionRect.x = ( background.w / 2 ) - ( mainMenuSubCaptionRect.w / 2 );
	mainMenuSubCaptionRect.y = mainMenuCaptionRect.y  + mainMenuCaptionRect.h;//+ margin;

	InitGreyAreaRect();

	std::cout << "Adding tile surfaces : " << std::endl;
	for ( size_t i = 0; i < tileTextures.size() ; ++i )
		SetTileColorSurface( i, tileColors[ i ], tileTextures );

	std::cout << "Adding hard tile surfaces : " << std::endl;
	for ( size_t i = 0; i < hardTileTextures.size() ; ++i )
		SetTileColorSurface( i, hardTileColors[ i ], hardTileTextures );

	return true;
}
void Renderer::FillSurface( SDL_Surface* source, unsigned char r, unsigned char g, unsigned char b ) const
{
	SDL_FillRect( source, NULL, SDL_MapRGBA( source->format, r, g, b, 255 )  );
}

void Renderer::FillSurface( SDL_Surface* source, const SDL_Color &color ) const
{
	FillSurface( source, color.r, color.g, color.b );
}

SDL_Texture* Renderer::InitSurface( const Rect &rect     , const SDL_Color &clr ) const
{
	return InitSurface( static_cast< int > ( rect.w ), static_cast< int > ( rect.h ), clr.r, clr.g, clr.b );
}
SDL_Texture* Renderer::InitSurface( int width, int height, const SDL_Color &clr ) const
{
	return InitSurface( width, height, clr.r, clr.g, clr.b );
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
SDL_Texture* Renderer::LoadImage( const std::string &filename )
{
	// Temporary stoare for the iamge that's loaded
	SDL_Surface* loadedImage = nullptr;

	// Load the image
	loadedImage = IMG_Load( filename.c_str() );


	SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, loadedImage );

	// Free the old image
	SDL_FreeSurface( loadedImage );

	// If the image loaded
	if ( texture == nullptr )
	{
		std::cout << "Failed to load " << filename << std::endl;
	}

		return texture;
}
SDL_Surface* Renderer::SetDisplayFormat( SDL_Surface* surface ) const
{
	if ( !surface )
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
void Renderer::ClearBoard( )
{
	tileList.clear();
	ballList.clear();
}
void Renderer::AddBall( const std::shared_ptr< Ball > &ball )
{
	ballList.push_back( ball );
}

void Renderer::RemoveBall(  const std::shared_ptr< Ball > &ball )
{
	ballList.erase( std::find( ballList.begin(), ballList.end(), ball ) );
}

void Renderer::AddBonusBox( const std::shared_ptr< BonusBox > &bonusBox )
{
	bonusBoxRect          = bonusBox->rect.ToSDLRect( );
	//std::string bonusText = bonusBox->GetName();

	int bonusBoxMargin = bonusBoxRect.w / 10;
	int doubleMargin = bonusBoxMargin * 2;

	// Background
	SDL_Surface* bonus = SDL_CreateRGBSurface( 0, bonusBoxRect.w, bonusBoxRect.h, SCREEN_BPP, rmask, gmask, bmask, amask);
	if ( bonusBox->GetOwner() == Player::Local )
		SDL_FillRect( bonus, NULL, SDL_MapRGBA( bonus->format, localPlayerColor.r, localPlayerColor.g, localPlayerColor.b, localPlayerColor.a ) );
	else
		SDL_FillRect( bonus, NULL, SDL_MapRGBA( bonus->format, remotePlayerColor.r, remotePlayerColor.g, remotePlayerColor.b, remotePlayerColor.a ) );

	/*
	// Text
	SDL_Surface* surface = TTF_RenderText_Solid( tinyFont, bonusText.c_str(), localPlayerColor );
	SDL_Rect textPosition;
	textPosition.w = surface->clip_rect.w;
	textPosition.h = surface->clip_rect.h;
	textPosition.x = bonusBoxMargin + ( ( bonusBoxRect.w - doubleMargin  ) /  2 ) - ( textPosition.w / 2  );
	textPosition.y = bonusBoxRect.h - surface->clip_rect.h;
	
	// Add tect rect to the surface
	SDL_BlitSurface( surface, NULL, bonus, &textPosition);
	SDL_FreeSurface( surface );
	*/

	// Icon
	SDL_Rect logoPosition;
	logoPosition.x = bonusBoxMargin;
	logoPosition.y = bonusBoxMargin;
	logoPosition.w = bonusBoxRect.w - doubleMargin;
	logoPosition.h = bonusBoxRect.w - doubleMargin;
	SDL_Surface* logo = SDL_CreateRGBSurface( 0, logoPosition.w, logoPosition.h, SCREEN_BPP, rmask, gmask, bmask, amask);
	SDL_FillRect( logo, NULL, SDL_MapRGBA( bonus->format, tileColors[0].r, tileColors[0].g, tileColors[0].b, 255 ) );

	// Add icon to the surface
	SDL_BlitSurface( logo   , NULL, bonus, &logoPosition);
	SDL_FreeSurface( logo );

	bonusBoxTexture = SDL_CreateTextureFromSurface( renderer, bonus );

	bonusBoxRect = bonus->clip_rect;
	SDL_FreeSurface( bonus );

	bonusBox->SetTexture( bonusBoxTexture );

	bonusBoxRect.x = 200;
	bonusBoxRect.y = 100;

	bonusBoxList.push_back( bonusBox );

}
void Renderer::RemoveBonusBox( const std::shared_ptr< BonusBox >  &bonusBox )
{
	bonusBoxList.erase( std::find( bonusBoxList.begin(), bonusBoxList.end(), bonusBox ) );
}

void Renderer::SetLocalPaddle( std::shared_ptr< Paddle >  &paddle )
{
	localPaddle = paddle;

	localPlayerPaddle = InitSurface( localPaddle->rect, localPlayerColor );
}
void Renderer::SetRemotePaddle( std::shared_ptr< Paddle >  &paddle )
{
	remotePaddle = paddle;

	remotePlayerPaddle = InitSurface( localPaddle->rect, remotePlayerColor );
}

// ============================================================================================
// ================================= Renderering ==============================================
// ============================================================================================
void Renderer::Render( )
{
	SDL_RenderClear( renderer );

	switch ( gameState )
	{
		case GameState::MainMenu:
			RenderMainMenuHeader();
			RenderMainMenuImage();
			RenderMainMenuFooter();
			break;
		case GameState::Lobby:
			RenderMainMenuHeader();
			RenderLobby();
			break;
		case GameState::InGame:
			RenderForeground();
			RenderText();
			break;
		case GameState::Paused:
			RenderForeground();
			RenderText();
			RenderMenuItem( pauseResumeButton );
			RenderMenuItem( pauseMainMenuButton );
			RenderMenuItem( pauseQuitButton );
			break;
		default:
			break;
	}
		
	SDL_RenderPresent( renderer );
}
void Renderer::RenderForeground()
{

	// Draw balls
	for ( std::shared_ptr< Ball > gp : ballList )
	{
		SDL_Rect r = gp->rect.ToSDLRect( );
		if ( gp->GetOwner() == Player::Local )
			SDL_RenderCopy( renderer, localPlayerBallTexture, nullptr, &r );
		else if ( gp->GetOwner() == Player::Remote )
			SDL_RenderCopy( renderer, remotePlayerBallTexture, nullptr, &r );
	}

	// Draw tiles
	for ( std::shared_ptr< Tile > gp : tileList)
	{
		SDL_Rect r = gp->rect.ToSDLRect();
		if ( gp->GetTileType() == TileType::Hard )
			SDL_RenderCopy( renderer, hardTileTextures[ 5 - gp->GetHitsLeft()], nullptr, &r );
		else
			SDL_RenderCopy( renderer, tileTextures[ gp->GetTileTypeAsIndex() ], nullptr, &r );
	}

	// Draw paddles
	if ( localPaddle )
	{
		//SDL_Rect localPaddleRect = localPaddle->rect.ToSDLRect();
		//SDL_RenderCopy( renderer, localPlayerPaddle, nullptr, &localPaddleRect  );
	}

	if ( remotePaddle )
	{
		SDL_Rect remotePaddleRect = remotePaddle->rect.ToSDLRect();
		SDL_RenderCopy( renderer, remotePlayerPaddle, nullptr, &remotePaddleRect  );
	}

	for ( std::shared_ptr< BonusBox > gp : bonusBoxList)
	{
		SDL_Rect boxRect = gp->rect.ToSDLRect();//bonusBox.ToSDLRect();//bonusBoxRect.ToSDLRect();
		SDL_Texture* texture = gp->GetTexture();
		SDL_RenderCopy( renderer, texture, nullptr, &boxRect );
	}
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

	if ( localPlayerBallsTexture  )
		SDL_RenderCopy( renderer, localPlayerBallsTexture, nullptr, &localPlayerBallsRect);

	// Remote
	if ( remotePlayerCaptionTexture )
		SDL_RenderCopy( renderer, remotePlayerCaptionTexture , nullptr, &remotePlayerCaptionRect );

	if ( remotePlayerLivesTexture  )
		SDL_RenderCopy( renderer, remotePlayerLivesTexture, nullptr, &remotePlayerLivesRect );

	if ( remotePlayerPointsTexture  )
		SDL_RenderCopy( renderer, remotePlayerPointsTexture, nullptr, &remotePlayerPointsRect);

	if ( remotePlayerBallsTexture  )
		SDL_RenderCopy( renderer, remotePlayerBallsTexture, nullptr, &remotePlayerBallsRect);
}
void Renderer::RenderLobby()
{
	if ( greyAreaTexture )
		SDL_RenderCopy( renderer, greyAreaTexture, nullptr, &greyAreaRect );

	SDL_Rect r{ 200, 400, 0, 0 };
	SDL_Texture* text = nullptr;

	// Render text
	text = RenderTextTexture_Blended( mediumFont, "Available games", textColor, r );
	r.x = ( greyAreaRect.w - r.w) / 2 ;
	r.y = greyAreaRect.y + 10;
	SDL_RenderCopy( renderer, text, nullptr, &r);
	SDL_DestroyTexture( text );

	// Render game list box
	r.y += r.h;
	r.h = greyAreaRect.h - 150;
	r.w = static_cast< int > ( r.h * 1.777 );
	r.x = ( greyAreaRect.w - r.w) / 2 ;
	text = InitSurface( r.w, r.h,  0, 0, 0 );
	SDL_SetTextureAlphaMod( text, 255 );
	SDL_RenderCopy( renderer, text, nullptr, &r);
	SDL_DestroyTexture( text );

	r.y += r.h + 10;
	text = RenderTextTexture_Blended( mediumFont, "Update", textColor, r );
	SDL_RenderCopy( renderer, text, nullptr, &r);
	SDL_DestroyTexture( text );
}

void Renderer::RenderMainMenuHeader()
{
	if( mainMenuCaptionTexture )
		SDL_RenderCopy( renderer, mainMenuCaptionTexture   , nullptr, &mainMenuCaptionRect );

	if( mainMenuSubCaptionTexture )
		SDL_RenderCopy( renderer, mainMenuSubCaptionTexture, nullptr, &mainMenuSubCaptionRect );
}
void Renderer::RenderMainMenuImage()
{
	if ( greyAreaTexture )
		SDL_RenderCopy( renderer, greyAreaTexture, nullptr, &greyAreaRect );
}
void Renderer::RenderMainMenuFooter()
{
	RenderMenuItem( singlePlayerText );
	RenderMenuItem( multiplayerPlayerText );
	RenderMenuItem( optionsButton );
	RenderMenuItem( quitButton );
}
void Renderer::RenderMenuItem( const MenuItem &menuItem ) const
{
	if( menuItem.GetTexture() != nullptr )
	{
		SDL_Rect r = menuItem.GetRect();
		SDL_RenderCopy( renderer, menuItem.GetTexture(), nullptr, &r );
	}
}
// ==============================================================================================
// ================================= Text handling ==============================================
// ==============================================================================================
//
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
	tinyFont = TTF_OpenFont( "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSansMono.ttf", 12 );

	font = LoadFont( "media/fonts/sketchy.ttf", 28 );

	mediumFont = TTF_OpenFont( "media/fonts/sketchy.ttf", 41 );

	bigFont = TTF_OpenFont( "media/fonts/sketchy.ttf", 57 );

	hugeFont = TTF_OpenFont( "media/fonts/sketchy.ttf", 100 );

	if ( bigFont == nullptr || font == nullptr || tinyFont == nullptr || mediumFont == nullptr || hugeFont == nullptr )
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
SDL_Texture* Renderer::RenderTextTexture_Blended(  TTF_Font* textFont, const std::string &textToRender, const SDL_Color &color, SDL_Rect &rect, int style /* = 0 */ )
{
	if ( style != 0 )
		TTF_SetFontStyle( textFont, style );

	SDL_Surface* surface = TTF_RenderText_Blended( textFont, textToRender.c_str(), color);

	if ( style != 0 )
		TTF_SetFontStyle( textFont, TTF_STYLE_NORMAL );

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
	SDL_DestroyTexture( localPlayerTextTexture );
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
			localPlayerCaptionTexture = RenderTextTexture_Solid( bigFont, textToRender.c_str(), localPlayerColor, localPlayerCaptionRect );

			localPlayerCaptionRect.x = 0;
			localPlayerCaptionRect.y = 0;

			// Set remaning text rects based on caption rect
			localPlayerLivesRect.x = 40;
			localPlayerLivesRect.y = localPlayerCaptionRect.h - 10;

			localPlayerPointsRect.x = localPlayerLivesRect.x;
			localPlayerPointsRect.y = localPlayerLivesRect.y + localPlayerLivesRect.h ;

			localPlayerBallsRect.x = localPlayerLivesRect.x;
			localPlayerBallsRect.y = localPlayerPointsRect.y + localPlayerLivesRect.h ;
		}
	} else if ( player == Player::Remote )
	{
		if ( remotePlayerCaptionValue != textToRender ||   remotePlayerCaptionTexture == nullptr )
		{
			remotePlayerCaptionValue  = textToRender;

			SDL_DestroyTexture( remotePlayerCaptionTexture );
			remotePlayerCaptionTexture = RenderTextTexture_Solid( bigFont, textToRender.c_str(), remotePlayerColor, remotePlayerCaptionRect );

			remotePlayerCaptionRect.x = background.w - remotePlayerCaptionRect.w;
			remotePlayerCaptionRect.y = 0;

			CalculateRemotePlayerTextureRects();
		}
	}
}
void Renderer::RenderLives( unsigned long lifeCount, const Player &player, bool force /* = false */ )
{

	if ( player == Player::Local )
	{
		if ( localPlayerLivesValue != lifeCount ||   localPlayerLivesTexture == nullptr || force )
		{
			localPlayerLivesValue  = lifeCount;

			std::stringstream ss;
			ss << "Lives : " << lifeCount;

			SDL_DestroyTexture( localPlayerLivesTexture );
			localPlayerLivesTexture = RenderTextTexture_Solid( font, ss.str().c_str(), localPlayerColor, localPlayerLivesRect );
		}
	} else if ( player == Player::Remote )
	{
		if ( remotePlayerLivesValue != lifeCount ||   remotePlayerLivesTexture == nullptr || force )
		{
			remotePlayerLivesValue  = lifeCount;

			std::stringstream ss;
			ss << "Lives : " << lifeCount;

			SDL_DestroyTexture( remotePlayerLivesTexture );
			remotePlayerLivesTexture = RenderTextTexture_Solid( font, ss.str().c_str(), remotePlayerColor, remotePlayerLivesRect );
		}
	}
}
void Renderer::RenderPoints( unsigned long pointCount, const Player &player, bool force /* = false */   )
{
	if ( player == Player::Local )
	{
		if ( localPlayerPointsValue != pointCount || localPlayerPointsTexture == nullptr || force )
		{
			localPlayerPointsValue  = pointCount;

			std::stringstream ss;
			ss << "Points : " << pointCount;

			SDL_DestroyTexture( localPlayerPointsTexture  );
			localPlayerPointsTexture = RenderTextTexture_Solid( font, ss.str().c_str(), localPlayerColor, localPlayerPointsRect  );
		}
	} else if ( player == Player::Remote )
	{
		if ( remotePlayerPointsValue != pointCount || remotePlayerPointsTexture == nullptr || force )
		{
			remotePlayerPointsValue  = pointCount;

			std::stringstream ss;
			ss << "Points : " << pointCount;

			SDL_DestroyTexture( remotePlayerPointsTexture  );
			remotePlayerPointsTexture = RenderTextTexture_Solid( font, ss.str().c_str(), remotePlayerColor, remotePlayerPointsRect  );

			CalculateRemotePlayerTextureRects();
		}
	}
}

void Renderer::RenderBallCount( unsigned long ballCount, const Player &player, bool force /* = false */   )
{
	if ( player == Player::Local )
	{
		if ( localPlayerBallsValue != ballCount || localPlayerBallsTexture == nullptr  || force )
		{
			localPlayerBallsValue = ballCount;

			std::stringstream ss;
			ss << "Balls : " <<  ballCount;

			SDL_DestroyTexture( localPlayerBallsTexture  );
			localPlayerBallsTexture  = RenderTextTexture_Solid( font, ss.str().c_str(), localPlayerColor, localPlayerBallsRect  );

			localPlayerBallsRect.x = localPlayerLivesRect.x;
			localPlayerBallsRect.y = localPlayerPointsRect.y + localPlayerLivesRect.h ;
		}
	} else if ( player == Player::Remote )
	{
		if ( remotePlayerPointsValue != ballCount || remotePlayerBallsTexture == nullptr || force )
		{
			remotePlayerPointsValue = ballCount;

			std::stringstream ss;
			ss << "Balls : " << ballCount;

			SDL_DestroyTexture( remotePlayerBallsTexture );
			remotePlayerBallsTexture = RenderTextTexture_Solid( font, ss.str().c_str(), remotePlayerColor, remotePlayerBallsRect  );

			CalculateRemotePlayerTextureRects();
		}
	}
}
void Renderer::ResetText()
{
	RenderPoints( 0, Player::Local, true );
	RenderPoints( 0, Player::Remote, true );
	RenderLives( 0, Player::Local, true );
	RenderLives( 0, Player::Remote, true );
	RenderBallCount( 0, Player::Local, true );
	RenderBallCount( 0, Player::Remote, true );
}
void Renderer::AddMainMenuButtons( const std::string &singlePlayerString, const std::string &multiplayerString, const std::string &optionsString, const std::string &quitString )
{
	AddMainMenuButton( singlePlayerString, MainMenuItemType::SinglePlayer );
	AddMainMenuButton( multiplayerString, MainMenuItemType::MultiPlayer );
	AddMainMenuButton( optionsString, MainMenuItemType::Options );
	AddMainMenuButton( quitString, MainMenuItemType::Quit );

	CenterMainMenuButtons( );
}
void Renderer::AddMainMenuButton( const std::string &menuItemString, const MainMenuItemType &mit )
{
	switch ( mit )
	{
		case MainMenuItemType::SinglePlayer:
			singlePlayerText = AddMenuButtonHelper( singlePlayerText, menuItemString, { 0, 0, 0, 0 }  );
			singlePlayerText.SetRectXY( margin / 2,background.h - ( ( background.h - greyAreaRect.h ) / 2)  + ( singlePlayerText.GetRectH( )) );
			break;
		case MainMenuItemType::MultiPlayer:
			multiplayerPlayerText = AddMenuButtonHelper( multiplayerPlayerText, menuItemString, singlePlayerText.GetRect() );
			break;
		case MainMenuItemType::Options:
			optionsButton = AddMenuButtonHelper( optionsButton, menuItemString, multiplayerPlayerText.GetRect());
			break;
		case MainMenuItemType::Quit:
			quitButton = AddMenuButtonHelper( quitButton, menuItemString, optionsButton.GetRect());
			break;
		case MainMenuItemType::Unknown:
			break;
	}
}
MenuItem Renderer::AddMenuButtonHelper( MenuItem menuItem, std::string menuItemString, const SDL_Rect &singlePlayerRect )

{
	SDL_Rect r;
	menuItem.SetTexture( RenderTextTexture_Blended( mediumFont, menuItemString, textColor, r ) );
	r.x = singlePlayerRect.x + singlePlayerRect.w + margin;
	r.y = singlePlayerRect.y;

	menuItem.SetRect( r );
	menuItem.SetName( menuItemString );

	return menuItem;
}
void Renderer::SetMainMenuItemUnderline( bool setUnderline, const MainMenuItemType &mit  )
{
	switch ( mit )
	{
		case MainMenuItemType::SinglePlayer:
			singlePlayerText = SetUnderlineHelper( singlePlayerText, setUnderline );
			break;
		case MainMenuItemType::MultiPlayer:
			multiplayerPlayerText = SetUnderlineHelper( multiplayerPlayerText, setUnderline );
			break;
		case MainMenuItemType::Options:
			optionsButton = SetUnderlineHelper( optionsButton, setUnderline );
			break;
		case MainMenuItemType::Quit:
			quitButton = SetUnderlineHelper( quitButton, setUnderline );
			break;
		case MainMenuItemType::Unknown:
			break;
	}
}
MenuItem Renderer::SetUnderlineHelper( MenuItem menuItem, bool setUnderline )
{
	if ( menuItem.HasValidTexture() && ( setUnderline == menuItem.IsSelected() ) )
		return menuItem;

	SDL_Color clr = textColor;
	int style = 0;

	if ( setUnderline )
	{
		clr = SDL_Color{ 0, 200, 200, 255};
		style = TTF_STYLE_UNDERLINE | TTF_STYLE_ITALIC;
	}

	SDL_Rect r = menuItem.GetRect();	
	menuItem.SetTexture( RenderTextTexture_Blended( mediumFont, menuItem.GetName(), clr, r, style ) );
	menuItem.SetRect( r );
	menuItem.SetSelcted( setUnderline );

	return menuItem;
}
void Renderer::RemoveMainMenuItemsUnderlines( )
{
	SetMainMenuItemUnderline( false, MainMenuItemType::SinglePlayer );
	SetMainMenuItemUnderline( false, MainMenuItemType::MultiPlayer );
	SetMainMenuItemUnderline( false, MainMenuItemType::Options );
	SetMainMenuItemUnderline( false, MainMenuItemType::Quit );
}
SDL_Rect Renderer::GetSinglePlayerRect() const
{
	return singlePlayerText.GetRect();
}
SDL_Rect Renderer::GetMultiplayerPlayerRect() const
{
	return multiplayerPlayerText.GetRect();
}
SDL_Rect Renderer::GetOptionsPlayerRect() const
{
	return optionsButton.GetRect();
}
SDL_Rect Renderer::GetQuitPlayerRect() const
{
	return quitButton.GetRect();
}
void Renderer::CenterMainMenuButtons( )
{
	int totoalWidth = quitButton.GetEndX() - singlePlayerText.GetRectX();
	int freeSpace = background.w - totoalWidth;
	int startingPoint = freeSpace / 2;

	singlePlayerText.SetRectX( startingPoint );
	multiplayerPlayerText.SetRectX( singlePlayerText.GetEndX() + margin );
	optionsButton.SetRectX( multiplayerPlayerText.GetEndX() + margin );
	quitButton.SetRectX( optionsButton.GetEndX() + margin );
}
void Renderer::InitGreyAreaRect( )
{
	greyAreaRect.w = background.w;
	int heightCutoff = mainMenuSubCaptionRect.y + mainMenuSubCaptionRect.h;

	greyAreaRect.h = background.h - heightCutoff * 2;
	greyAreaRect.x = 0;
	greyAreaRect.y = ( background.h - greyAreaRect.h ) / 2;

	greyAreaTexture = InitSurface( background.w, background.h, 120, 120, 120 );
}
void Renderer::AddPauseMenuButtons( const std::string &resumeString, const std::string &mainMenuString, const std::string &quitString )
{
	pauseResumeButton = AddMenuButtonHelper( pauseResumeButton, resumeString, { 0, 0, 0, 0 }  );
	pauseResumeButton .SetRectXY( margin / 2, background.h - pauseResumeButton.GetRectH() );

	pauseMainMenuButton = AddMenuButtonHelper( pauseMainMenuButton, mainMenuString, pauseResumeButton.GetRect()  );

	pauseQuitButton = AddMenuButtonHelper( pauseQuitButton, quitString, pauseMainMenuButton.GetRect()  );

	CenterPauseButtons();
}
void Renderer::CenterPauseButtons( )
{
	int totoalWidth = pauseQuitButton.GetEndX() - pauseResumeButton.GetRectX();
	int freeSpace = background.w - totoalWidth;
	int startingPoint = freeSpace / 2;

	pauseResumeButton.SetRectX( startingPoint );
	pauseMainMenuButton.SetRectX( pauseResumeButton.GetEndX() + margin );
	pauseQuitButton.SetRectX( pauseMainMenuButton.GetEndX() + margin );
}

void Renderer::SetMainMenuItemUnderline( bool setUnderline, const PauseMenuItemType &mit  )
{
	switch ( mit )
	{
		case PauseMenuItemType::Resume:
			pauseResumeButton = SetUnderlineHelper( pauseResumeButton, setUnderline );
			break;
		case PauseMenuItemType::MainMenu:
			pauseMainMenuButton = SetUnderlineHelper( pauseMainMenuButton, setUnderline );
			break;
		case PauseMenuItemType::Quit:
			pauseQuitButton = SetUnderlineHelper( pauseQuitButton, setUnderline );
			break;
		case PauseMenuItemType::Unknown:
			break;
	}
}
SDL_Rect Renderer::GetPauseResumeRect() const
{
	return pauseResumeButton.GetRect();
}

SDL_Rect Renderer::GetPauseMainMenuRect() const
{
	return pauseMainMenuButton.GetRect();
}

SDL_Rect Renderer::GetPauseQuitRect() const
{
	return pauseQuitButton.GetRect();
}

void Renderer::CalculateRemotePlayerTextureRects()
{
	// Set remaning text rects based on caption rect
	remotePlayerPointsRect.x = background.w - remotePlayerPointsRect.w - 20;
	remotePlayerPointsRect.y = localPlayerPointsRect.y;// remotePlayerLivesRect.y + remotePlayerLivesRect.h  + 20;

	remotePlayerLivesRect.x = remotePlayerPointsRect.x;
	remotePlayerLivesRect.y = localPlayerLivesRect.y; //remotePlayerCaptionRect.h - 10;

	remotePlayerBallsRect.x = remotePlayerPointsRect.x;
	remotePlayerBallsRect.y = remotePlayerPointsRect.y + remotePlayerPointsRect.h ; //remotePlayerCaptionRect.h - 10;
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
	SDL_DestroyTexture( localPlayerPaddle       );
	SDL_DestroyTexture( remotePlayerPaddle      );
	SDL_DestroyTexture( localPlayerBallTexture  );
	SDL_DestroyTexture( remotePlayerBallTexture );

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
void Renderer::ForceMouseFocus()
{
	SDL_SetWindowGrab( window, SDL_TRUE );
}
