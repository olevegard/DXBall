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

// This macro enalea falltrhough in switch-case statements
// Two or more consecute case wihtout break triggers a warning
// Adding FALLTHROUGH before the case makes clang ignore this
// The warning will be triggered as normal if FALLTHROUGH is not used
#ifdef __clang__
#define FALLTHROUGH  [[clang::fallthrough]];
#else
#define FALLTHROUGH
#endif

	Renderer::Renderer()
	:	window( nullptr )
	,	renderer( nullptr )
	,	gameState( GameState::MainMenu )

	,	background({ 0, 0, 1920 / 2, 1080 / 2 })
	,	screenFlags( SDL_WINDOW_OPENGL  )
	,	fullscreen( false )

	,	ballList(  )
	,	tileList(  )
	,	localPaddle( nullptr )
	,	remotePaddle( nullptr )

	,	isTwoPlayerMode( false )
	,	localPlayerBallTexture( nullptr )
	,	localPlayerPaddle( nullptr )

	,	remotePlayerBallTexture( nullptr )
	,	remotePlayerPaddle( nullptr )

#if defined ( WIN32 )
#else
	,	tileTextures{ nullptr, nullptr, nullptr, nullptr }
	,	hardTileTextures{ nullptr, nullptr, nullptr, nullptr, nullptr }
#endif
	,	tinyFont()
	,	font()
	,	mediumFont()
	,	bigFont()

	,	margin( 30 )
	,	singlePlayerText     ( "Single Player" )
	,	multiplayerPlayerText( "Multiplayer"   )
	,	optionsButton        ( "Options"       )
	,	quitButton           ( "Quit"          )

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
	fullscreen = fullscreenOn;

	if ( SDL_SetWindowFullscreen( window, (fullscreen ) ? SDL_WINDOW_FULLSCREEN : 0 ) )
	{
		std::cout << "Renderer@" << __LINE__  << " Failed to set fullscreen mode to " << std::boolalpha << fullscreen << std::endl;
		std::cout << "Renderer@" << __LINE__  << " Error : " << SDL_GetError() << std::endl;
		return false;
	}
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

	if( !InitializeTextures() )
		return false;

	return true;
}
bool Renderer::InitializeTextures()
{
	localPlayerBallTexture  = RenderHelpers::InitSurface( 20, 20, colorConfig.localPlayerColor , renderer );
	remotePlayerBallTexture = RenderHelpers::InitSurface( 20, 20, colorConfig.remotePlayerColor, renderer );

	InitializeMainMenuTextures();
	InitGreyAreaRect();

	for ( uint64_t i = 0; i < tileTextures.size() ; ++i )
		RenderHelpers::SetTileColorSurface( renderer, i, GetTileColor( i ), tileTextures );

	for ( uint64_t i = 0; i < hardTileTextures.size() ; ++i )
		RenderHelpers::SetTileColorSurface( renderer, i,  GetHardTileColor( i ) , hardTileTextures );

	return true;
}
void Renderer::InitializeMainMenuTextures()
{
	mainMenuBackground = RenderHelpers::InitSurface( background.w, background.h, 0, 0, 0, renderer );

	mainMenuCaption.Reset( renderer, "DX Ball",  hugeFont, colorConfig.textColor );

	mainMenuCaption.rect.x = ( background.w / 2 ) - ( mainMenuCaption.rect.w / 2 );
	mainMenuCaption.rect.y = 0;

	mainMenuSubCaption.Reset( renderer, "A quite simple clone made by a weird guy", mediumFont, colorConfig.textColor );

	mainMenuSubCaption.rect.x = ( background.w / 2 ) - ( mainMenuSubCaption.rect.w / 2 );
	mainMenuSubCaption.rect.y = mainMenuCaption.rect.y  + mainMenuCaption.rect.h;
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
	ballList.erase( std::find( ballList.begin(), ballList.end(), ball ) );
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
	SDL_RenderClear( renderer );

	switch ( gameState )
	{
		case GameState::MainMenu:
		case GameState::Lobby:
			RenderMenu();
			break;

		case GameState::InGame:
			RenderGameObjects();
		FALLTHROUGH
		case GameState::InGameWait:
			RenderText();
			break;

		case GameState::Paused:
			RenderPause();
		FALLTHROUGH
		case GameState::GameOver:
			RenderText();
			break;
		default:
			break;
	}

	SDL_RenderPresent( renderer );
}
void Renderer::RenderMenu()
{
	RenderHelpers::RenderTextItem( renderer, mainMenuCaption  );
	RenderHelpers::RenderTextItem( renderer, mainMenuSubCaption  );
	RenderHelpers::RenderTextItem( renderer, greyArea  );

	if ( gameState == GameState::Lobby )
		RenderLobbyFooter();
	else
		RenderMainMenuFooter();
}
void Renderer::RenderLobbyFooter()
{
	RenderHelpers::RenderMenuItem( renderer, lobbyNewGameButton );
	RenderHelpers::RenderMenuItem( renderer, lobbyUpdateButton );
	RenderHelpers::RenderMenuItem( renderer, lobbyBackButton );
	gameList->Render( renderer );
}
void Renderer::RenderMainMenuFooter()
{
	RenderHelpers::RenderMenuItem( renderer, singlePlayerText );
	RenderHelpers::RenderMenuItem( renderer, multiplayerPlayerText );
	RenderHelpers::RenderMenuItem( renderer, optionsButton );
	RenderHelpers::RenderMenuItem( renderer, quitButton );
}
void Renderer::RenderPause()
{
	RenderGameObjects();
	RenderHelpers::RenderMenuItem( renderer, pauseResumeButton );
	RenderHelpers::RenderMenuItem( renderer, pauseMainMenuButton );
	RenderHelpers::RenderMenuItem( renderer, pauseQuitButton );
}
void Renderer::RenderGameObjects()
{
	RenderBalls();
	RenderTiles();
	RenderPaddles();
	RenderBullets();
	RenderBonusBoxes();

	RenderParticles();
}
void Renderer::RenderParticles()
{
	for ( const auto &p : particles )
	{
		RenderHelpers::RenderParticle( renderer, p );
	}

	RenderHelpers::SetDrawColor( renderer, colorConfig.backgroundColor);
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
	RenderHelpers::RenderTextItem( renderer, localPlayerText );

	RenderHelpers::RenderTextItem( renderer, localPlayerCaption);
	RenderHelpers::RenderTextItem( renderer, localPlayerPoints);
	RenderHelpers::RenderTextItem( renderer, localPlayerLives );
	RenderHelpers::RenderTextItem( renderer, localPlayerBalls );

	if ( !isTwoPlayerMode )
		return;

	RenderHelpers::RenderTextItem( renderer, remotePlayerCaption );
	RenderHelpers::RenderTextItem( renderer, remotePlayerLives );
	RenderHelpers::RenderTextItem( renderer, remotePlayerPoints );
	RenderHelpers::RenderTextItem( renderer, remotePlayerBalls );
}

// ==============================================================================================
// ================================= Text handling ==============================================
// ==============================================================================================

bool Renderer::LoadFontAndText()
{
	font = RenderHelpers::LoadFont( "media/fonts/sketchy.ttf", 28 );
	mediumFont = RenderHelpers::LoadFont( "media/fonts/sketchy.ttf", 41 );
	bigFont = RenderHelpers::LoadFont( "media/fonts/sketchy.ttf", 57 );
	hugeFont = RenderHelpers::LoadFont( "media/fonts/sketchy.ttf", 100 );

	if ( bigFont == nullptr || font == nullptr || mediumFont == nullptr || hugeFont == nullptr )
	{
		std::cout << "Loading failed" << std::endl;
		std::cin.ignore();
		return false;
	}

	return true;
}
void Renderer::RenderText( const std::string &textToRender, const Player &player, bool fade   )
{
	if ( player != Player::Local )
		return;

	if ( fade )
	{
		localPlayerText.DestroyTexture();
		localPlayerText.StartFade();
	}

	localPlayerText.ReasetAlpha();

	if (  localPlayerText.NeedsUpdate( textToRender  ) )
	{
		localPlayerText.value = textToRender;
		localPlayerText.doFade = fade;

		localPlayerText.Reset( renderer, bigFont, colorConfig.textColor );
		localPlayerText.Rescale( background.w, background.h );
	}
}
void Renderer::RenderPlayerCaption( const std::string textToRender, const Player &player  )
{
	if ( player == Player::Local && localPlayerCaption.NeedsUpdate( textToRender ))
	{
		localPlayerCaption.value = textToRender;
		localPlayerCaption.Reset( renderer, bigFont, colorConfig.localPlayerColor );

		localPlayerLives.rect.x = 40;
		localPlayerLives.rect.y =  localPlayerCaption.rect.h - 10;

		localPlayerPoints.rect.x = localPlayerLives.rect.x;
		localPlayerPoints.rect.y = localPlayerLives.rect.y + localPlayerLives.rect.h ;

		localPlayerBalls.rect.x = localPlayerLives.rect.x;
	}
	else if ( player == Player::Remote && remotePlayerCaption.NeedsUpdate( textToRender ) )
	{
		remotePlayerCaption.value = textToRender;

		remotePlayerCaption.Reset( renderer, bigFont, colorConfig.remotePlayerColor );

		remotePlayerCaption.rect.x = background.w - remotePlayerCaption.rect.w;

		CalculateRemotePlayerTextureRects();
	}
}
void Renderer::RenderLives( uint64_t lifeCount, const Player &player )
{
	if ( player == Player::Local && localPlayerLives.NeedsUpdate( lifeCount ) )
	{
		localPlayerLives.value = lifeCount;

		std::stringstream ss("");
		ss << "Lives : " << lifeCount;

		localPlayerLives.Reset( renderer, ss.str(), font, colorConfig.localPlayerColor );
	}
	else if ( player == Player::Remote && remotePlayerLives.NeedsUpdate( lifeCount ) )
	{
		remotePlayerLives.value = lifeCount;

		std::stringstream ss;
		ss << "Lives : " << lifeCount;

		remotePlayerLives.Reset( renderer, ss.str(), font, colorConfig.remotePlayerColor );
	}
}
void Renderer::RenderPoints( uint64_t pointCount, const Player &player )
{
	if ( player == Player::Local && localPlayerPoints.NeedsUpdate( pointCount ) )
	{
		localPlayerPoints.value = pointCount;

		std::stringstream ss("");
		ss << "Points : " << pointCount;

		localPlayerPoints.Reset( renderer, ss.str(), font, colorConfig.localPlayerColor );
	}
	else if ( player == Player::Remote && remotePlayerPoints.NeedsUpdate( pointCount ) )
	{
		remotePlayerPoints.value  = pointCount;

		std::stringstream ss("");
		ss << "Points : " << pointCount;

		remotePlayerPoints.Reset( renderer, ss.str(), font, colorConfig.remotePlayerColor );

		CalculateRemotePlayerTextureRects();
	}
}
void Renderer::RenderBallCount( uint64_t  ballCount, const Player &player )
{
	if ( player == Player::Local && localPlayerBalls.NeedsUpdate( ballCount ) )
	{
		localPlayerBalls.value = ballCount;

		std::stringstream ss("");
		ss << "Balls : " <<  ballCount;

		localPlayerBalls.Reset( renderer, ss.str(), font, colorConfig.localPlayerColor );

		localPlayerBalls.rect.x = localPlayerLives.rect.x;
		localPlayerBalls.rect.y = localPlayerPoints.rect.y + localPlayerLives.rect.h ;
	} else if ( player == Player::Remote && remotePlayerBalls.NeedsUpdate( ballCount ))
	{
		remotePlayerBalls.value =  ballCount;

		std::stringstream ss("");
		ss << "Balls : " << ballCount;

		remotePlayerBalls.Reset( renderer, ss.str(), font, colorConfig.remotePlayerColor );

		CalculateRemotePlayerTextureRects();
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
	localPlayerText.ReasetAlpha();
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
			singlePlayerText.SetRectXY( margin / 2,background.h - ( ( background.h - greyArea.rect.h ) / 2)  + ( singlePlayerText.GetRectH( )) );
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
	// The distance from the top of grey area to top of window
	int heightCutoff = mainMenuSubCaption.rect.y + mainMenuSubCaption.rect.h;

	greyArea.rect.x = 0;
	greyArea.rect.y = heightCutoff;

	greyArea.rect.w = background.w;
	greyArea.rect.h = background.h - heightCutoff * 2;

	greyArea.Init( renderer, colorConfig.greyAreaColor );
}
void Renderer::AddPauseMenuButtons( const std::string &resumeString, const std::string &mainMenuString, const std::string &quitString )
{
	pauseResumeButton = AddMenuButtonHelper( pauseResumeButton, resumeString, { 0, 0, 0, 0 }  );
	pauseResumeButton.SetRectXY( margin / 2, background.h - pauseResumeButton.GetRectH() );

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
	for ( const auto &p : tileTextures )
		SDL_DestroyTexture( p );

	for ( const auto &p : hardTileTextures )
		SDL_DestroyTexture( p );

	// Free ball/paddle
	SDL_DestroyTexture( localPlayerPaddle       );
	SDL_DestroyTexture( remotePlayerPaddle      );
	SDL_DestroyTexture( localPlayerBallTexture  );
	SDL_DestroyTexture( remotePlayerBallTexture );

	// Free text surfaces
	localPlayerText.DestroyTexture();
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
	lobbyMenuListRect.w = static_cast< int32_t > ( greyArea.rect.w * 0.6 );
	lobbyMenuListRect.h = static_cast< int32_t > ( greyArea.rect.h * 0.9 );

	lobbyMenuListRect.x = static_cast< int32_t > ( ( background.w * 0.5   ) - ( lobbyMenuListRect.w * 0.5 ) );
	lobbyMenuListRect.y = static_cast< int32_t > ( ( greyArea.rect.h * 0.5 ) - ( lobbyMenuListRect.h * 0.5 ) ) + greyArea.rect.y;

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

	localPlayerText.Update( delta );
}
void Renderer::GenerateParticleEffect( std::shared_ptr< Tile > tile )
{
	Rect r( 0,0,10,10 );
	Vector2f pos = RectHelpers::CenterInRect( tile->rect, r );
	SDL_Color color = GetTileColor( tile );

	for ( int i = 0; i < colorConfig.particleFireCount ; ++i )
	{
		auto p = Particle( Rect( pos.x, pos.y, 10, 10 ), color  );
		p.SetDecay( colorConfig.particleDecayMin, colorConfig.particleDecayMax );
		p.SetSpeed( colorConfig.particleSpeedMin, colorConfig.particleSpeedMax );
		particles.push_back( p );
	}
}
SDL_Color Renderer::GetTileColor( std::shared_ptr< Tile > tile  )
{
	if ( tile->GetTileType() == TileType::Hard )
		return GetHardTileColor( 5 - tile->GetHitsLeft() );
	else
		return GetTileColor( tile->GetTileTypeAsIndex() );
}
void Renderer::SetIsTwoPlayerMode( bool isTwoPlayerMode_ )
{
	isTwoPlayerMode = isTwoPlayerMode_;
}
SDL_Renderer* Renderer::GetRenderer() const
{
	return renderer;
}
TTF_Font* Renderer::GetFont() const
{
	return font;
}
SDL_Color Renderer::GetBackgroundColor() const
{
	return colorConfig.backgroundColor;
}
SDL_Color Renderer::GetTextColor() const
{
	return colorConfig.textColor;
}
void Renderer::AddMenuList( MenuList* mitem )
{
	ml = mitem;
}
SDL_Color Renderer::GetTileColor( uint64_t type )
{
	return colorConfig.GetTileColor( static_cast < TileType > ( type ));
}
SDL_Color Renderer::GetHardTileColor( uint64_t index )
{
	return colorConfig.GetTileColor( TileType::Hard, index );
}
