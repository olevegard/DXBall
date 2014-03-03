#include "Renderer.h"

#include "structs/game_objects/Ball.h"
#include "structs/game_objects/Tile.h"
#include "structs/game_objects/Paddle.h"

#include "enums/MainMenuItemType.h"
#include "enums/PauseMenuItemType.h"
#include "enums/LobbyMenuItem.h"
#include "enums/BonusType.h"

#include "tools/RenderTools.h"

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

	,	background({ 0, 0, 1920 / 2, 1080 / 2 })
	,	screenFlags( SDL_WINDOW_OPENGL  )
	,	fullscreen( false )
#if defined ( WIN32 )
#else
	,	tileTextures{ nullptr, nullptr, nullptr, nullptr }
	,	hardTileTextures{ nullptr, nullptr, nullptr, nullptr, nullptr }
#endif

	,	ballList(  )
	,	tileList(  )
	,	localPaddle( nullptr )
	,	remotePaddle( nullptr )

	,	isTwoPlayerMode( false )
	,	localPlayerBallTexture( nullptr )
	,	localPlayerPaddle( nullptr )

	,	remotePlayerBallTexture( nullptr )
	,	remotePlayerPaddle( nullptr )

	,	tinyFont()
	,	font()
	,	mediumFont()
	,	bigFont()

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
	RenderHelpers::SetDrawColor( renderer, colorConfig.backgroundColor);

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

	RenderHelpers::HideMouseCursor( false );
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
		RenderHelpers::HideMouseCursor( true );
	else
		RenderHelpers::HideMouseCursor( false );

	if ( gameState == GameState::Paused )
		RenderText("Paused", Player::Local);
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
	localPlayerBallTexture  = RenderHelpers::InitSurface( background.w, background.h, colorConfig.localPlayerColor , renderer );
	remotePlayerBallTexture = RenderHelpers::InitSurface( background.w, background.h, colorConfig.remotePlayerColor, renderer );

	// Menu mode
	mainMenuBackground = RenderHelpers::InitSurface( background.w, background.h, 0, 0, 0, renderer );

	mainMenuCaptionTexture = RenderHelpers::RenderTextTexture_Blended( hugeFont, "DX Ball", colorConfig.textColor, mainMenuCaptionRect, renderer );
	mainMenuCaptionRect.x = ( background.w / 2 ) - ( mainMenuCaptionRect.w / 2 );
	mainMenuCaptionRect.y = 0;

	mainMenuSubCaptionTexture = RenderHelpers::RenderTextTexture_Blended(
			mediumFont,
			"A quite simple clone made by a weird guy",
			colorConfig.textColor,
			mainMenuSubCaptionRect,
			renderer
		);
	mainMenuSubCaptionRect.x = ( background.w / 2 ) - ( mainMenuSubCaptionRect.w / 2 );
	mainMenuSubCaptionRect.y = mainMenuCaptionRect.y  + mainMenuCaptionRect.h;//+ margin;

	InitGreyAreaRect();

	for ( uint64_t i = 0; i < tileTextures.size() ; ++i )
		RenderHelpers::SetTileColorSurface( renderer, i, GetTileColor( i ), tileTextures );

	for ( uint64_t i = 0; i < hardTileTextures.size() ; ++i )
		RenderHelpers::SetTileColorSurface( renderer, i,  GetHardTileColor( i ) , hardTileTextures );

	return true;
}
void Renderer::LoadColors()
{
	colorConfig.LoadConfig();

	RenderHelpers::SetDrawColor( renderer, colorConfig.backgroundColor);

	bonusTypeColors = colorConfig.GetBonusColorMap();
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
	tileList.erase( tileList.begin(), tileList.end() );
}
void Renderer::AddBall( const std::shared_ptr< Ball > &ball )
{
	ballList.push_back( ball );
}

void Renderer::RemoveBall(  const std::shared_ptr< Ball > &ball )
{
	const auto &p = std::find( ballList.begin(), ballList.end(), ball );

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
	auto texture = CreateBonusBoxTexture( bonusBox );

	bonusBox->SetTexture( texture ) ;

	bonusBoxList.push_back( bonusBox );
}
SDL_Texture* Renderer::CreateBonusBoxTexture( const std::shared_ptr< BonusBox >  &bb )
{
	SDL_Rect bonusBoxRect = bb->rect.ToSDLRect( );

	SDL_Color color;
	if ( bb->GetOwner() == Player::Local )
		color = colorConfig.localPlayerColor;
	else
		color =  colorConfig.remotePlayerColor;

	SDL_Color bonusColor = bonusTypeColors[ bb->GetBonusType() ];

	return RenderHelpers::CreateBonusBoxTexture( renderer, bonusBoxRect, color, bonusColor );
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

	localPlayerPaddle = RenderHelpers::InitSurface( localPaddle->rect, colorConfig.localPlayerColor, renderer );
}
void Renderer::SetRemotePaddle( std::shared_ptr< Paddle >  &paddle )
{
	remotePaddle = paddle;

	remotePlayerPaddle = RenderHelpers::InitSurface( localPaddle->rect, colorConfig.remotePlayerColor, renderer );
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
			RenderHelpers::RenderMenuItem( renderer, lobbyNewGameButton );
			RenderHelpers::RenderMenuItem( renderer, lobbyUpdateButton );
			RenderHelpers::RenderMenuItem( renderer, lobbyBackButton );
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
			RenderHelpers::RenderMenuItem( renderer, pauseResumeButton );
			RenderHelpers::RenderMenuItem( renderer, pauseMainMenuButton );
			RenderHelpers::RenderMenuItem( renderer, pauseQuitButton );
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
			RenderHelpers::SetDrawColor( renderer, p.color );
			SDL_RenderFillRect( renderer, &r );
		}
	}
	RenderHelpers::SetDrawColor( renderer, colorConfig.backgroundColor);

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

	RenderHelpers::RenderTextItem( renderer, localPlayerCaption);
	RenderHelpers::RenderTextItem( renderer, localPlayerPoints);
	RenderHelpers::RenderTextItem( renderer, localPlayerLives );
	RenderHelpers::RenderTextItem( renderer, localPlayerBalls );

	// Remote
	if ( !isTwoPlayerMode )
		return;

	RenderHelpers::RenderTextItem( renderer, remotePlayerCaption );
	RenderHelpers::RenderTextItem( renderer, remotePlayerLives );
	RenderHelpers::RenderTextItem( renderer, remotePlayerPoints );
	RenderHelpers::RenderTextItem( renderer, remotePlayerBalls );
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
	RenderHelpers::RenderMenuItem( renderer, singlePlayerText );
	RenderHelpers::RenderMenuItem( renderer, multiplayerPlayerText );
	RenderHelpers::RenderMenuItem( renderer, optionsButton );
	RenderHelpers::RenderMenuItem( renderer, quitButton );
}
// ==============================================================================================
// ================================= Text handling ==============================================
// ==============================================================================================

bool Renderer::LoadFontAndText()
{
	//tinyFont = TTF_OpenFont( "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSansMono.ttf", 20 );
	font = RenderHelpers::LoadFont( "media/fonts/sketchy.ttf", 28 );
	mediumFont = RenderHelpers::LoadFont( "media/fonts/sketchy.ttf", 41 );
	bigFont = RenderHelpers::LoadFont( "media/fonts/sketchy.ttf", 57 );
	hugeFont = RenderHelpers::LoadFont( "media/fonts/sketchy.ttf", 100 );

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
				RenderHelpers::RenderTextTexture_Solid( bigFont, textToRender.c_str(), colorConfig.textColor, localPlayerTextRect, renderer );

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
				RenderHelpers::RenderTextTexture_Solid( bigFont, textToRender.c_str(), colorConfig.localPlayerColor, localPlayerCaption.rect , renderer );

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
				RenderHelpers::RenderTextTexture_Solid( bigFont, textToRender.c_str(), colorConfig.remotePlayerColor, remotePlayerCaption.rect, renderer );

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
				RenderHelpers::RenderTextTexture_Solid( font, ss.str().c_str(), colorConfig.localPlayerColor, localPlayerLives.rect , renderer );
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
				= RenderHelpers::RenderTextTexture_Solid( font, ss.str().c_str(), colorConfig.remotePlayerColor, remotePlayerLives.rect, renderer  );
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
				RenderHelpers::RenderTextTexture_Solid( font, ss.str().c_str(), colorConfig.localPlayerColor, localPlayerPoints.rect, renderer  );
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
				RenderHelpers::RenderTextTexture_Solid( font, ss.str().c_str(), colorConfig.remotePlayerColor, remotePlayerPoints.rect, renderer  );

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
				= RenderHelpers::RenderTextTexture_Solid( font, ss.str().c_str(), colorConfig.localPlayerColor, localPlayerBalls.rect, renderer  );

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
				RenderHelpers::RenderTextTexture_Solid( font, ss.str().c_str(), colorConfig.remotePlayerColor, remotePlayerBalls.rect, renderer  );

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
	menuItem.SetTexture( RenderHelpers::RenderTextTexture_Blended( mediumFont, menuItemString, colorConfig.textColor, r, renderer ) );
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

	SDL_Color clr = colorConfig.textColor;
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

	greyAreaTexture = RenderHelpers::InitSurface( background.w, background.h, colorConfig.greyAreaColor, renderer );
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
	for ( const auto &p : tileTextures )
	{
		SDL_DestroyTexture( p );
	}

	for ( const auto &p : hardTileTextures )
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
		color = GetHardTileColor( 5 - tile->GetHitsLeft() );
	else
		color = GetTileColor( tile->GetTileTypeAsIndex() );

	for ( int i = 0; i < 50 ; ++i )
		particles.push_back( Particle( Rect( pos.x, pos.y, 10, 10 ), color  ) );
}
