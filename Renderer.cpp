#include "Renderer.h"

#include "structs/game_objects/Ball.h"
#include "structs/game_objects/Tile.h"
#include "structs/game_objects/Paddle.h"

#include "enums/MainMenuItemType.h"
#include "enums/PauseMenuItemType.h"
#include "enums/LobbyMenuItem.h"
#include "enums/BonusType.h"

#include "tools/RenderTools.h"

#include "ColorConfigLoader.h"

#include <sstream>
#include <iostream>
#include <algorithm>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <csignal>

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

	,	background({ 0, 0, 1920 / 2, 1080 / 2 })
	,	SCREEN_BPP ( 32 )
	,	screenFlags( SDL_WINDOW_OPENGL  )
	,	fullscreen( false )
	,	backgroundColor({ 0, 0, 0, 255 })// 40, 20, 40, 255
#if defined ( WIN32 )
#else
	,	tileColors{ {102, 0, 0, 255}, {255, 55, 13, 255}, {140, 140, 140, 255}, {255, 183, 13, 255} }
	,	tileTextures{ nullptr, nullptr, nullptr, nullptr }
	,	hardTileColors{ { 255, 145, 0, 255}, { 200, 100, 0, 255}, { 150, 60, 0, 255},{ 50, 40, 0, 255}, { 20, 15, 0, 255} }
	,	hardTileTextures{ nullptr, nullptr, nullptr, nullptr, nullptr }
	#endif

	,	ballList(  )
	,	tileList(  )
	,	localPaddle( nullptr )
	,	remotePaddle( nullptr )

	,	isTwoPlayerMode( false )
	,	localPlayerColor({ 0, 140, 0,255 })
	,	localPlayerBallTexture( nullptr )
	,	localPlayerPaddle( nullptr )

	,	remotePlayerColor({ 140, 0, 0, 255 })
	,	remotePlayerBallTexture( nullptr )
	,	remotePlayerPaddle( nullptr )

	,	tinyFont()
	,	font()
	,	mediumFont()
	,	bigFont()

	,	textColor({ 0, 140, 140, 255 })
	,	greyAreaColor({ 0, 0, 128, 255})

	,	localPlayerTextTexture( )
	,	localPlayerTextRect( )
	,	localPlayerTextValue( "" )
	,	localPlayerTextAlpha( 1.0 )
	,	localPlayerTextFade( false )

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

	,	lobbyNewGameButton( "New Game" )
	,	lobbyUpdateButton( "Update" )
	,	lobbyBackButton( "Back" )
	,	lobbyMenuListRect( { 0, 0, 0, 0 })
{
}
Renderer::~Renderer()
{
	CleanUp();

	QuitSDL();
}
bool Renderer::Init( const SDL_Rect &rect, bool startFS, bool server )
{
#if defined(_WIN32)
	tileTextures.resize( 4 );
	hardTileTextures.resize( 5 );
#endif

	fullscreen = startFS;
	background = rect;

	if ( fullscreen )
		screenFlags = SDL_WINDOW_FULLSCREEN;
	else
		screenFlags = SDL_WINDOW_OPENGL;

	if ( !InitSDLSubSystems())
		return false;

	// Set up screen
	if ( !CreateWindow( server ) )
		return false;

	if ( !CreateRenderer() )
		return false;

	Setup();

	if ( !LoadAssets() )
		return false;

	return true;
}

// ============================================================================================
// ===================================== Setup ================================================
// ============================================================================================
bool Renderer::InitSDLSubSystems() const
{
	if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
	{
		std::cout << "Renderer@" << __LINE__  << " Failed to initialize SDL : " << SDL_GetError() << std::endl;
		return false;
	}

	if ( TTF_Init( ) == -1 )
	{
		std::cout << "Renderer@" << __LINE__  << " Failed to initialize TTF : " << TTF_GetError() << std::endl;
		return false;
	}

	return true;
}
bool Renderer::CreateRenderer()
{
	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );//SDL_RENDERER_PREVENTVSYNC

	if ( renderer == nullptr )
	{
		std::cout << "Renderer@" << __LINE__  << " Could not create renderer!";
		return false;
	}

	SDL_RenderSetLogicalSize( renderer, background.w, background.h );

	// Sets rendering color ( background color )
	SetDrawColor( backgroundColor);

	// Set how alpha is blended, need to set this to be able use Render colors with alpha values
	SDL_SetRenderDrawBlendMode( renderer, SDL_BLENDMODE_BLEND );

	// Render an empty frame
	SDL_RenderClear( renderer );
	SDL_RenderPresent( renderer );

	return true;
}
void Renderer::Setup()
{
	SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	HideMouseCursor( false );
}
bool Renderer::CreateWindow(bool server )
{
	if ( !server )
		window = SDL_CreateWindow( "Client", background.w, 50, background.w, background.h, screenFlags );
	else
		window = SDL_CreateWindow( "Server", 0, 50, background.w, background.h, screenFlags );

	if ( window == nullptr )
	{
		std::cout << "Renderer@" << __LINE__  << " Failed to apply video mode : " << SDL_GetError();
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

	if ( gameState == GameState::Paused )
		RenderText("Paused", Player::Local);
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
	std::cout << "Renderer@" << __LINE__  << " Settting fullscreen...\n";
	fullscreen = fullscreenOn;

	if ( SDL_SetWindowFullscreen( window, (fullscreen ) ? SDL_WINDOW_FULLSCREEN : 0 ) )
	{
		std::cout << "Renderer@" << __LINE__  << " Failed to set fullscreen mode to " << std::boolalpha << fullscreen << std::endl;
		return false;
	}
	std::cout << "Renderer@" << __LINE__  << " Error : " << SDL_GetError() << std::endl;
	return true;
}
// ============================================================================================
// ================================ Texture helpers ===========================================
// ============================================================================================
bool Renderer::LoadAssets()
{
	if ( !LoadFontAndText() )
		return false;

	LoadColors();

	if( !LoadImages() )
		return false;

	return true;
}
bool Renderer::LoadImages()
{
	localPlayerBallTexture  = RenderHelpers::InitSurface( background.w, background.h, localPlayerColor , renderer );
	remotePlayerBallTexture = RenderHelpers::InitSurface( background.w, background.h, remotePlayerColor, renderer );

	// Menu mode
	mainMenuBackground = RenderHelpers::InitSurface( background.w, background.h, 0, 0, 0, renderer );

	mainMenuCaptionTexture = RenderHelpers::RenderTextTexture_Blended( hugeFont, "DX Ball", textColor, mainMenuCaptionRect, renderer );
	mainMenuCaptionRect.x = ( background.w / 2 ) - ( mainMenuCaptionRect.w / 2 );
	mainMenuCaptionRect.y = 0;

	mainMenuSubCaptionTexture = RenderHelpers::RenderTextTexture_Blended(
			mediumFont,
			"A quite simple clone made by a weird guy",
			textColor,
			mainMenuSubCaptionRect,
			renderer
		);
	mainMenuSubCaptionRect.x = ( background.w / 2 ) - ( mainMenuSubCaptionRect.w / 2 );
	mainMenuSubCaptionRect.y = mainMenuCaptionRect.y  + mainMenuCaptionRect.h;//+ margin;

	InitGreyAreaRect();

	for ( size_t i = 0; i < tileTextures.size() ; ++i )
		SetTileColorSurface( i, tileColors[ i ], tileTextures );

	for ( size_t i = 0; i < hardTileTextures.size() ; ++i )
		SetTileColorSurface( i, hardTileColors[ i ], hardTileTextures );

	return true;
}
void Renderer::LoadColors()
{
	ColorConfigLoader cfgldr;

	cfgldr.LoadConfig();

	textColor = cfgldr.GetTextColor();
	backgroundColor = cfgldr.GetBackgroundColor();
	SetDrawColor( backgroundColor);

	localPlayerColor = cfgldr.GetLocalPlayerColor();
	remotePlayerColor = cfgldr.GetRemotePlayerColor();
	greyAreaColor = cfgldr.GetGreyArea();

	tileColors[0] = cfgldr.GetTileColor( TileType::Regular );
	tileColors[1] = cfgldr.GetTileColor( TileType::Explosive );
	tileColors[2] = cfgldr.GetTileColor( TileType::Unbreakable );

	hardTileColors[0] = cfgldr.GetTileColor( TileType::Hard, 0 );
	hardTileColors[1] = cfgldr.GetTileColor( TileType::Hard, 1 );
	hardTileColors[2] = cfgldr.GetTileColor( TileType::Hard, 2 );
	hardTileColors[3] = cfgldr.GetTileColor( TileType::Hard, 3 );
	hardTileColors[4] = cfgldr.GetTileColor( TileType::Hard, 4 );

	bonusTypeColors = cfgldr.GetBonusColorMap();
}
SDL_Surface* Renderer::SetDisplayFormat( SDL_Surface* surface ) const
{
	if ( !surface )
	{
		std::cout << "Renderer@" << __LINE__  << " Cannot set display format : nullptr\n";
	}

	return surface;
}
void Renderer::SetTileColorSurface( size_t index, const SDL_Color &color, std::vector< SDL_Texture* > &list  )
{
	SDL_Texture *text = RenderHelpers::InitSurface(  60, 20, color.r, color.g, color.b, renderer );
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
	bulletList.erase( bulletList.begin(), bulletList.end() );
	ballList.erase( ballList.begin(), ballList.end() );
}
void Renderer::AddBall( const std::shared_ptr< Ball > &ball )
{
	ballList.push_back( ball );
}

void Renderer::RemoveBall(  const std::shared_ptr< Ball > &ball )
{
	auto p = std::find( ballList.begin(), ballList.end(), ball );

	if ( p == ballList.end() )
	{
		std::cout << "Renderer.cpp@" << __LINE__ << " Ball not found" << std::endl;
		std::cin.ignore();
	}
	else
		ballList.erase( p  );
}

void Renderer::AddBonusBox( const std::shared_ptr< BonusBox > &bonusBox )
{
	bonusBoxRect          = bonusBox->rect.ToSDLRect( );

	// Background
	SDL_Surface* bonus = SDL_CreateRGBSurface( 0, bonusBoxRect.w, bonusBoxRect.h, SCREEN_BPP, rmask, gmask, bmask, amask);
	if ( bonusBox->GetOwner() == Player::Local )
		SDL_FillRect(
				bonus,
				NULL,
				SDL_MapRGBA( bonus->format, localPlayerColor.r, localPlayerColor.g, localPlayerColor.b, localPlayerColor.a ) );
	else
		SDL_FillRect(
				bonus,
				NULL,
				SDL_MapRGBA( bonus->format, remotePlayerColor.r, remotePlayerColor.g, remotePlayerColor.b, remotePlayerColor.a ) );


	bonusBoxRect = bonus->clip_rect;
	SetBonusBoxIcon( bonusBoxRect.w, bonus, bonusBox->GetBonusType()  );

	bonusBoxTexture = SDL_CreateTextureFromSurface( renderer, bonus );

	SDL_FreeSurface( bonus );

	bonusBox->SetTexture( bonusBoxTexture );

	bonusBoxRect.x = 200;
	bonusBoxRect.y = 100;

	bonusBoxList.push_back( bonusBox );

}
void Renderer::SetBonusBoxIcon( int32_t width, SDL_Surface* bonusBox, const BonusType &bonusType )
{
	int32_t bbMargin = width / 8;
	int32_t doubleMargin = bbMargin * 2;

	SDL_Rect logoPosition;
	logoPosition.x = bbMargin;
	logoPosition.y = bbMargin;
	logoPosition.w = width - doubleMargin;
	logoPosition.h = width - doubleMargin;

	SDL_Color logoColor = GetBonusBoxColor( bonusType );

	SDL_Surface* logo = SDL_CreateRGBSurface( 0, logoPosition.w, logoPosition.h, SCREEN_BPP, rmask, gmask, bmask, amask);
	SDL_FillRect( logo, NULL, SDL_MapRGBA( bonusBox->format, logoColor.r, logoColor.g,  logoColor.b, logoColor.a ) );

	SDL_BlitSurface( logo, NULL, bonusBox, &logoPosition);
	SDL_FreeSurface( logo );
}
SDL_Color Renderer::GetBonusBoxColor( const BonusType &bonusType )
{
	/*
	SDL_Color logoColor = { 0, 128, 128, 255 };
	if ( bonusType == BonusType::ExtraLife )
	{
		logoColor.r = 255;
		logoColor.g = 255;
		logoColor.b = 255;
	}
	else
	{
		logoColor.r = 0;
		logoColor.g = 0;
		logoColor.b = 0;
	}
	*/

	return bonusTypeColors[bonusType];
}
void Renderer::RemoveBonusBox( const std::shared_ptr< BonusBox >  &bonusBox )
{
	bonusBoxList.erase( std::find( bonusBoxList.begin(), bonusBoxList.end(), bonusBox ) );
}

void Renderer::AddBullet( const std::shared_ptr< Bullet > &bb )
{
	bulletList.push_back( bb );
}
void Renderer::RemoveBullet( const std::shared_ptr< Bullet >  &bullet )
{
	bulletList.erase( std::find( bulletList.begin(), bulletList.end(), bullet ) );
}
void Renderer::SetLocalPaddle( std::shared_ptr< Paddle >  &paddle )
{
	localPaddle = paddle;

	localPlayerPaddle = RenderHelpers::InitSurface( localPaddle->rect, localPlayerColor, renderer );
}
void Renderer::SetRemotePaddle( std::shared_ptr< Paddle >  &paddle )
{
	remotePaddle = paddle;

	remotePlayerPaddle = RenderHelpers::InitSurface( localPaddle->rect, remotePlayerColor, renderer );
}

// ============================================================================================
// ================================= Renderering ==============================================
// ============================================================================================
void Renderer::Render( )
{
	if ( renderer == nullptr )
	{
		std::cout << "ERRO! Renderer is NULL : " << SDL_GetError() << std::endl;
		raise( SIGABRT );
	}

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
			//RenderLobby();
			RenderMainMenuImage();
			RenderMenuItem( lobbyNewGameButton );
			RenderMenuItem( lobbyUpdateButton );
			RenderMenuItem( lobbyBackButton );
			ml->Render( renderer );
			break;
		case GameState::InGame:
		case GameState::InGameWait:
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
		case GameState::GameOver:
			RenderText();
			break;
		default:
			break;
	}
	for ( const auto &p : particles )
	{
		if ( p.isAlive )
		{
			SDL_Rect r = p.rect.ToSDLRect();
			SetDrawColor( p.color );
			SDL_RenderFillRect( renderer, &r );
		}
	}
	SetDrawColor( backgroundColor);

	SDL_RenderPresent( renderer );
}
void Renderer::RenderForeground()
{
	RenderText();
	RenderBalls();
	RenderTiles();
	RenderPaddles();
	RenderBullets();
	RenderBonusBoxes();
}
void Renderer::RenderBalls()
{
	for ( std::shared_ptr< Ball > gp : ballList )
	{
		SDL_Rect r = gp->rect.ToSDLRect( );
		if ( gp->GetOwner() == Player::Local )
			SDL_RenderCopy( renderer, localPlayerBallTexture, nullptr, &r );
		else if ( gp->GetOwner() == Player::Remote )
			SDL_RenderCopy( renderer, remotePlayerBallTexture, nullptr, &r );
	}
}
void Renderer::RenderTiles()
{
	for ( std::shared_ptr< Tile > gp : tileList)
	{
		SDL_Rect r = gp->rect.ToSDLRect();
		if ( gp->GetTileType() == TileType::Hard )
			SDL_RenderCopy( renderer, hardTileTextures[ 5 - gp->GetHitsLeft()], nullptr, &r );
		else
			SDL_RenderCopy( renderer, tileTextures[ gp->GetTileTypeAsIndex() ], nullptr, &r );
	}
}
void Renderer::RenderPaddles()
{
	if ( localPaddle )
	{
		SDL_Rect localPaddleRect = localPaddle->rect.ToSDLRect();
		SDL_RenderCopy( renderer, localPlayerPaddle, nullptr, &localPaddleRect  );
	}

	if ( isTwoPlayerMode && remotePaddle )
	{
		SDL_Rect remotePaddleRect = remotePaddle->rect.ToSDLRect();
		SDL_RenderCopy( renderer, remotePlayerPaddle, nullptr, &remotePaddleRect  );
	}
}
void Renderer::RenderBullets()
{
	for ( std::shared_ptr< Bullet > gp : bulletList)
	{
		SDL_Rect r = gp->rect.ToSDLRect( );
		if ( gp->GetOwner() == Player::Local )
			SDL_RenderCopy( renderer, localPlayerBallTexture, nullptr, &r );
		else if ( gp->GetOwner() == Player::Remote )
			SDL_RenderCopy( renderer, remotePlayerBallTexture, nullptr, &r );
	}
}
void Renderer::RenderBonusBoxes()
{
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

	RenderTextItem( localPlayerCaption);
	RenderTextItem( localPlayerPoints);
	RenderTextItem( localPlayerLives );
	RenderTextItem( localPlayerBalls );

	// Remote
	if ( !isTwoPlayerMode )
		return;

	RenderTextItem( remotePlayerCaption );
	RenderTextItem( remotePlayerLives );
	RenderTextItem( remotePlayerPoints );
	RenderTextItem( remotePlayerBalls );
}
void Renderer::RenderLobby()
{
	if ( greyAreaTexture )
		SDL_RenderCopy( renderer, greyAreaTexture, nullptr, &greyAreaRect );
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
void Renderer::RenderTextItem( const RenderingItem< std::string >  &item ) const
{
	if ( item.texture != nullptr  )
		SDL_RenderCopy( renderer, item.texture, nullptr, &item.rect );
}
void Renderer::RenderTextItem( const RenderingItem< uint64_t >  &item ) const
{
	if ( item.texture != nullptr  )
		SDL_RenderCopy( renderer, item.texture, nullptr, &item.rect );
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
		std::cout << "Renderer@" << __LINE__  << " Failed to open font : " << fontName << " : " << TTF_GetError() << std::endl;
	}
	else
		std::cout << "Renderer@" << __LINE__  << " Font opened : " << fontName << " : " << TTF_GetError() << std::endl;

	return tempFont;
}
bool Renderer::LoadFontAndText()
{
	//tinyFont = TTF_OpenFont( "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSansMono.ttf", 20 );
	font = LoadFont( "media/fonts/sketchy.ttf", 28 );
	mediumFont = TTF_OpenFont( "media/fonts/sketchy.ttf", 41 );
	bigFont = TTF_OpenFont( "media/fonts/sketchy.ttf", 57 );
	hugeFont = TTF_OpenFont( "media/fonts/sketchy.ttf", 100 );

	if ( bigFont == nullptr || font == nullptr ||/* tinyFont == nullptr ||*/ mediumFont == nullptr || hugeFont == nullptr )
	{
		std::cout << "Loading failed" << std::endl;
		std::cin.ignore();
		return false;
	}

	return true;
}

void Renderer::RenderText( const std::string &textToRender, const Player &player, bool fade   )
{
	if ( player == Player::Local )
	{
		if ( fade )
		{
			RemoveText();
			localPlayerTextFade = true;
		}

		ResetAlpha();

		if (  localPlayerTextValue != textToRender || localPlayerTextTexture == nullptr )
		{
			localPlayerTextValue = textToRender;

			SDL_DestroyTexture( localPlayerTextTexture );
			localPlayerTextTexture =
				RenderHelpers::RenderTextTexture_Solid( bigFont, textToRender.c_str(), textColor, localPlayerTextRect, renderer );

			localPlayerTextRect.x = ( background.w / 2 ) - ( localPlayerTextRect.w / 2 );
			localPlayerTextRect.y = ( background.h / 2 ) - ( localPlayerTextRect.h / 2 );

			localPlayerTextFade = fade;
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
		//if ( localPlayerCaptionValue != textToRender ||   localPlayerCaptionTexture == nullptr )
		//if (
		{
			localPlayerCaption.value = textToRender;

			SDL_DestroyTexture( localPlayerCaption.texture );
			localPlayerCaption.texture =
				RenderHelpers::RenderTextTexture_Solid( bigFont, textToRender.c_str(), localPlayerColor, localPlayerCaption.rect , renderer );

			localPlayerCaption.rect.x = 0;
			localPlayerCaption.rect.y = 0;

			// Set remaning text rects based on caption rect
			localPlayerLives.rect.x = 40;
			localPlayerLives.rect.y =  localPlayerCaption.rect.h - 10;

			localPlayerPoints.rect.x = localPlayerLives.rect.x;
			localPlayerPoints.rect.y = localPlayerLives.rect.y + localPlayerLives.rect.h ;

			localPlayerBalls.rect.x = localPlayerLives.rect.x;
			localPlayerBalls.rect.y = localPlayerPoints.rect.y + localPlayerLives.rect.h ;
		}
	} else if ( player == Player::Remote )
	{
		//if ( remotePlayerCaption.value != textToRender ||   remotePlayerCaption.texture = nullptr )
		{
			remotePlayerCaption.value = textToRender;

			SDL_DestroyTexture( remotePlayerCaption.texture );
			remotePlayerCaption.texture =
				RenderHelpers::RenderTextTexture_Solid( bigFont, textToRender.c_str(), remotePlayerColor, remotePlayerCaption.rect, renderer );

			remotePlayerCaption.rect.x = background.w - remotePlayerCaption.rect.w;
			remotePlayerCaption.rect.y = 0;

			CalculateRemotePlayerTextureRects();
		}
	}
}
void Renderer::RenderLives( unsigned long lifeCount, const Player &player )
{
	if ( player == Player::Local )
	{
		//if ( localPlayerLivesValue != lifeCount ||   localPlayerLivesTexture == nullptr || force )
		{
			localPlayerLives.value = lifeCount;

			std::stringstream ss("");
			ss << "Lives : " << lifeCount;

			SDL_DestroyTexture( localPlayerLives.texture );
			localPlayerLives.texture =
				RenderHelpers::RenderTextTexture_Solid( font, ss.str().c_str(), localPlayerColor, localPlayerLives.rect , renderer );
		}
	} else if ( player == Player::Remote )
	{
		//if ( remotePlayerLives.value remotePlayerLivesValue != lifeCount ||   remotePlayerLivesTexture == nullptr || force )
		{
			remotePlayerLives.value = lifeCount;

			std::stringstream ss;
			ss << "Lives : " << lifeCount;

			SDL_DestroyTexture( remotePlayerLives.texture );
			remotePlayerLives.texture
				= RenderHelpers::RenderTextTexture_Solid( font, ss.str().c_str(), remotePlayerColor, remotePlayerLives.rect, renderer  );
		}
	}
}
void Renderer::RenderPoints( unsigned long pointCount, const Player &player )
{
	if ( player == Player::Local )
	{
		//if ( localPlayerPoints.CheckTexture() && ( localPlayerPoints.CheckValue( pointCount ) || force ) )
		{
			localPlayerPoints.value = pointCount;

			std::stringstream ss("");
			ss << "Points : " << pointCount;

			SDL_DestroyTexture( localPlayerPoints.texture  );

			localPlayerPoints.texture =
				RenderHelpers::RenderTextTexture_Solid( font, ss.str().c_str(), localPlayerColor, localPlayerPoints.rect, renderer  );
		}
	}
	else if ( player == Player::Remote )
	{
		//if ( remotePlayerPoints.CheckTexture() && ( remotePlayerPoints.CheckValue( pointCount ) || force ) )
		{
			remotePlayerPoints.value  = pointCount;

			std::stringstream ss;
			ss << "Points : " << pointCount;

			SDL_DestroyTexture( remotePlayerPoints.texture  );
			remotePlayerPoints.texture  =
				RenderHelpers::RenderTextTexture_Solid( font, ss.str().c_str(), remotePlayerColor, remotePlayerPoints.rect, renderer  );

			CalculateRemotePlayerTextureRects();
		}
	}
}

void Renderer::RenderBallCount( unsigned long ballCount, const Player &player )
{
	if ( player == Player::Local )
	{
		//if ( localPlayerBallsValue != ballCount || localPlayerBallsTexture == nullptr  || force )
		if ( localPlayerBalls.value != ballCount )
		{
			localPlayerBalls.value = ballCount;

			std::stringstream ss;
			ss << "Balls : " <<  ballCount;

			if ( localPlayerBalls.texture  != nullptr )
				SDL_DestroyTexture( localPlayerBalls.texture );

			localPlayerBalls.texture
				= RenderHelpers::RenderTextTexture_Solid( font, ss.str().c_str(), localPlayerColor, localPlayerBalls.rect, renderer  );

			localPlayerBalls.rect.x = localPlayerLives.rect.x;
			localPlayerBalls.rect.y = localPlayerPoints.rect.y + localPlayerLives.rect.h ;
		}
	} else if ( player == Player::Remote )
	{
		//if ( remotePlayerBallsValue != ballCount || remotePlayerBallsTexture == nullptr || force )
		{
			remotePlayerBalls.value =  ballCount;

			std::stringstream ss;
			ss << "Balls : " << ballCount;

			SDL_DestroyTexture( remotePlayerBalls.texture );
			remotePlayerBalls.texture =
				RenderHelpers::RenderTextTexture_Solid( font, ss.str().c_str(), remotePlayerColor, remotePlayerBalls.rect, renderer  );

			CalculateRemotePlayerTextureRects();
		}
	}
}
void Renderer::ResetText()
{
	RenderPoints( 0, Player::Local );
	RenderPoints( 0, Player::Remote );
	RenderLives( 0, Player::Local );
	RenderLives( 0, Player::Remote );
	RenderBallCount( 0, Player::Local );
	RenderBallCount( 0, Player::Remote );
	ResetAlpha();
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
	menuItem.SetTexture( RenderHelpers::RenderTextTexture_Blended( mediumFont, menuItemString, textColor, r, renderer ) );
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
	menuItem.SetTexture( RenderHelpers::RenderTextTexture_Blended( mediumFont, menuItem.GetName(), clr, r, renderer, style ) );
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

	greyAreaTexture = RenderHelpers::InitSurface( background.w, background.h, greyAreaColor, renderer );
}
void Renderer::AddPauseMenuButtons( const std::string &resumeString, const std::string &mainMenuString, const std::string &quitString )
{
	pauseResumeButton = AddMenuButtonHelper( pauseResumeButton, resumeString, { 0, 0, 0, 0 }  );
	pauseResumeButton.SetRectXY( margin / 2, background.h - pauseResumeButton.GetRectH() );
	std::cout << "Adding button : " << pauseResumeButton.GetRectX() << std::endl;

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
void Renderer::AddLobbyMenuButtons( const std::string &newGame, const std::string &update, const std::string &back )
{
	int32_t xPos =
		static_cast< int32_t > ( ( background.w * 0.5 ) -
		( ( lobbyNewGameButton.GetRectW()  +  lobbyUpdateButton.GetRectW() + lobbyBackButton.GetRectW() )  * 0.5 ) );
	int32_t yPos = singlePlayerText.GetRectY();

	lobbyNewGameButton = AddMenuButtonHelper( lobbyNewGameButton, newGame, { xPos, yPos, 0, 0 } );
	xPos = lobbyNewGameButton.GetEndX()  + 20;
	lobbyUpdateButton = AddMenuButtonHelper( lobbyUpdateButton, update, { xPos, yPos, 0, 0 } );
	xPos = lobbyUpdateButton.GetEndX() +  20;
	lobbyBackButton = AddMenuButtonHelper( lobbyBackButton, back, { xPos, yPos, 0, 0 } );

	CenterLobbyButtons();
}
void Renderer::CenterLobbyButtons( )
{
	int32_t width = 20 + lobbyBackButton.GetRectW() + lobbyNewGameButton.GetRectW() + lobbyUpdateButton.GetRectW();
	//int32_t width = 20 + lobbyBackButton.GetEndX() - lobbyNewGameButton.GetRectX();
	int32_t newX = lobbyMenuListRect.x + static_cast< int32_t > ( ( lobbyMenuListRect.w * 0.5 ) - ( width * 0.5 ) );

	lobbyNewGameButton.SetRectX( newX );
	newX = lobbyNewGameButton.GetEndX() + 20;
	lobbyUpdateButton.SetRectX( newX );
	newX = lobbyUpdateButton.GetEndX() + 20;
	lobbyBackButton.SetRectX( newX );
}
void Renderer::SetLobbyItemUnderline( bool setUnderline, const LobbyMenuItem &mit  )
{
	switch ( mit )
	{
		case LobbyMenuItem::NewGame:
			lobbyNewGameButton = SetUnderlineHelper( lobbyNewGameButton, setUnderline );
			break;
		case LobbyMenuItem::Update:
			lobbyUpdateButton = SetUnderlineHelper( lobbyUpdateButton, setUnderline );
			break;
		case LobbyMenuItem::Back:
			lobbyBackButton = SetUnderlineHelper( lobbyBackButton, setUnderline );
			break;
		default:
			break;
	}
}
SDL_Rect Renderer::GetLobbyNewGameRect() const
{
	return lobbyNewGameButton.GetRect();
}
SDL_Rect Renderer::GetLobbyUpdateRect() const
{
	return lobbyUpdateButton.GetRect();
}
SDL_Rect Renderer::GetLobbyBackRect() const
{
	return lobbyBackButton.GetRect();
}
void Renderer::CalculateRemotePlayerTextureRects()
{
	// Set remaning text rects based on caption rect
	remotePlayerPoints.rect.x = background.w - remotePlayerPoints.rect.w - 20;
	remotePlayerPoints.rect.y = localPlayerPoints.rect.y;

	remotePlayerLives.rect.x = remotePlayerPoints.rect.x;
	remotePlayerLives.rect.y = localPlayerLives.rect.y;

	remotePlayerBalls.rect.x = remotePlayerPoints.rect.x;
	remotePlayerBalls.rect.y = remotePlayerPoints.rect.y + remotePlayerPoints.rect.h;
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
	SDL_DestroyTexture( localPlayerCaption.texture );
	SDL_DestroyTexture( localPlayerLives.texture );
	SDL_DestroyTexture( localPlayerPoints.texture );

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
SDL_Rect Renderer::CalcMenuListRect()
{
	lobbyMenuListRect.w = static_cast< int32_t > ( greyAreaRect.w * 0.6 );
	lobbyMenuListRect.h = static_cast< int32_t > ( greyAreaRect.h * 0.9 );

	lobbyMenuListRect.x = static_cast< int32_t > ( ( background.w * 0.5   ) - ( lobbyMenuListRect.w * 0.5 ) );
	lobbyMenuListRect.y = static_cast< int32_t > ( ( greyAreaRect.h * 0.5 ) - ( lobbyMenuListRect.h * 0.5 ) ) + greyAreaRect.y;

	std::cout << "AddLobbyMenuButtons.w set to : " << lobbyMenuListRect.w << std::endl;
	return lobbyMenuListRect;
}
void Renderer::Update( double delta )
{
	for ( auto p = particles.begin(); p != particles.end();  )
	{
		p->Updated( delta );
		if ( !p->isAlive)
			particles.erase( p );
		else
			++p;
	}

	if ( !localPlayerTextFade )
		return;

	if ( localPlayerTextAlpha  > 0.0 )
	{
		localPlayerTextAlpha -= ( delta * 0.888 );

		if ( localPlayerTextAlpha < 0.0 )
		{
			localPlayerTextAlpha = 1.0;
			localPlayerTextFade = false;
		} else
		{
			Uint8 alpha  =  static_cast< Uint8 > ( 255 * localPlayerTextAlpha );
			SDL_SetTextureAlphaMod( localPlayerTextTexture, alpha );
		}
	}
}
void Renderer::ResetAlpha()
{
	localPlayerTextAlpha = 1.0;
	SDL_SetTextureAlphaMod( localPlayerTextTexture, 255 );
}
void Renderer::StartFade()
{
	localPlayerTextFade = true;
}
void Renderer::GenerateParticleEffect( std::shared_ptr< Tile > tile )
{
	Rect r( 0,0,10,10 );
	Vector2f pos = RectHelpers::CenterInRect( tile->rect, r );
	SDL_Color color{ 255, 255, 255, 255 };

	if ( tile->GetTileType() == TileType::Hard )
		color = hardTileColors[ 5 - tile->GetHitsLeft()  ];
	else
		color = tileColors[ tile->GetTileTypeAsIndex()  ];

	for ( int i = 0; i < 5 ; ++i )
		particles.push_back( Particle( Rect( pos.x, pos.y, 10, 10 ), color  ) );
}
