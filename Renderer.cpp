#include "Renderer.h"

#include "structs/game_objects/Ball.h"
#include "structs/game_objects/Tile.h"
#include "structs/game_objects/Bullet.h"
#include "structs/game_objects/Paddle.h"
#include "structs/game_objects/BonusBox.h"

#include "structs/menu_items/MenuList.h"

#include "enums/MainMenuItemType.h"
#include "enums/PauseMenuItemType.h"
#include "enums/LobbyMenuItem.h"
#include "enums/BonusType.h"

#include "structs/menu_items/MainMenuItem.h"
#include "structs/menu_items/PauseMenuItem.h"

#include "math/RectHelpers.h"

#include "tools/RenderTools.h"

#include <sstream>
#include <iostream>
#include <algorithm>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <csignal>

// This macro enables falltrhough in switch-case statements
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

	,	isFullscreen( false )
	,	isTwoPlayerMode( false )

	,	ballList(  )
	,	tileList(  )
	,	localPaddle( nullptr )
	,	remotePaddle( nullptr )

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

	isFullscreen= startFS;
	background = rect;

	if ( isFullscreen )
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
	SetFullscreen( !isFullscreen );
}
bool Renderer::SetFullscreen( bool fullscreenOn )
{
	isFullscreen = fullscreenOn;

	if ( SDL_SetWindowFullscreen( window, (isFullscreen ) ? SDL_WINDOW_FULLSCREEN : 0 ) )
	{
		std::cout << "Renderer@" << __LINE__  << " Failed to set isFullscreen mode to " << std::boolalpha << isFullscreen << std::endl;
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
	SDL_Texture* tileTexture = nullptr;

	if ( tile->GetTileType() == TileType::Hard )
		tileTexture = hardTileTextures[ 5 - tile->GetHitsLeft()];
	else
		tileTexture = tileTextures[ tile->GetTileTypeAsIndex() ];

	tile->SetTexture( tileTexture );

	tileList.push_back( tile );
}
void Renderer::RemoveTile( const std::shared_ptr< Tile >  &tile )
{
	tileList.erase( std::find( tileList.begin(), tileList.end(), tile) );
}
void Renderer::UpdateTileHit( const std::shared_ptr< Tile >  &tile ) const
{
	if ( tile->GetTileType() != TileType::Hard )
		return;

	SDL_Texture* texture = hardTileTextures[ 5 - tile->GetHitsLeft()];
	tile->SetTexture( texture );
}
void Renderer::ClearBoard( )
{
	bulletList.erase( bulletList.begin(), bulletList.end() );
	ballList.erase( ballList.begin(), ballList.end() );
	tileList.erase( tileList.begin(), tileList.end() );
}
void Renderer::AddBall( const std::shared_ptr< Ball > &ball )
{
	if ( ball->GetOwner() == Player::Local )
		ball->SetTexture( localPlayerBallTexture );
	else
		ball->SetTexture( remotePlayerBallTexture );

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
void Renderer::AddBullet( const std::shared_ptr< Bullet > &bullet )
{
	if ( bullet->GetOwner() == Player::Local )
		bullet->SetTexture( localPlayerBallTexture );
	else
		bullet->SetTexture( remotePlayerBallTexture );

	bulletList.push_back( bullet );
}
void Renderer::RemoveBullet( const std::shared_ptr< Bullet >  &bullet )
{
	bulletList.erase( std::find( bulletList.begin(), bulletList.end(), bullet ) );
}
void Renderer::SetLocalPaddle( std::shared_ptr< Paddle >  &paddle )
{
	localPaddle = paddle;

	localPlayerPaddle = RenderHelpers::InitSurface( localPaddle->rect, colorConfig.localPlayerColor, renderer );
	localPaddle->SetTexture( localPlayerPaddle  );
}
void Renderer::SetRemotePaddle( std::shared_ptr< Paddle >  &paddle )
{
	remotePaddle = paddle;

	remotePlayerPaddle = RenderHelpers::InitSurface( localPaddle->rect, colorConfig.remotePlayerColor, renderer );
	remotePaddle->SetTexture( remotePlayerPaddle );
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
		case GameState::Options:
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
	else if ( gameState == GameState::Options )
	{
		RenderHelpers::RenderOptionsItem( renderer, configItems[ ConfigValueType::BallSpeed ] );
		RenderHelpers::RenderOptionsItem( renderer, configItems[ ConfigValueType::BulletSpeed ] );
		RenderHelpers::RenderMenuItem( renderer, backToMenuButton);
	}
	else
		RenderMainMenuFooter();

	RenderHelpers::SetDrawColor( renderer, colorConfig.backgroundColor  );
}
void Renderer::RenderLobbyFooter()
{
	RenderHelpers::RenderMenuItem( renderer, lobbyNewGameButton );
	RenderHelpers::RenderMenuItem( renderer, lobbyUpdateButton );
	RenderHelpers::RenderMenuItem( renderer, lobbyBackButton );
	RenderHelpers::RenderMenuList( renderer, *gameList );
}
void Renderer::RenderMainMenuFooter()
{
	RenderHelpers::RenderMenuItem( renderer, singlePlayerButton);
	RenderHelpers::RenderMenuItem( renderer, multiPlayerButton);
	RenderHelpers::RenderMenuItem( renderer, optionsButton);
	RenderHelpers::RenderMenuItem( renderer, quitButton);
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
		RenderHelpers::RenderParticle( renderer, p );

	RenderHelpers::SetDrawColor( renderer, colorConfig.backgroundColor);
}
void Renderer::RenderBalls()
{
	for ( std::shared_ptr< Ball > ball : ballList )
		RenderHelpers::RenderGamePiece( renderer, ball );
}
void Renderer::RenderTiles()
{
	for ( std::shared_ptr< Tile > tile : tileList)
		RenderHelpers::RenderGamePiece( renderer, tile );
}
void Renderer::RenderPaddles()
{
	if ( localPaddle )
		RenderHelpers::RenderGamePiece( renderer, localPaddle );

	if ( isTwoPlayerMode && remotePaddle )
		RenderHelpers::RenderGamePiece( renderer, remotePaddle );
}
void Renderer::RenderBullets()
{
	for ( std::shared_ptr< Bullet > bullet : bulletList)
		RenderHelpers::RenderGamePiece( renderer, bullet );
}
void Renderer::RenderBonusBoxes()
{
	for ( std::shared_ptr< BonusBox > bb : bonusBoxList)
		RenderHelpers::RenderGamePiece( renderer, bb );
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
	tinyFont = RenderHelpers::LoadFont( "media/fonts/sketchy.ttf", 22 );
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

	localPlayerText.ResetAlpha();

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
	localPlayerText.ResetAlpha();
}
void Renderer::StartFade()
{
	localPlayerText.StartFade();
}
void Renderer::AddMainMenuButtons( const std::string &singlePlayerString, const std::string &multiplayerString, const std::string &optionsString, const std::string &quitString )
{
	AddMainMenuButton( singlePlayerString, MainMenuItemType::SinglePlayer );
	AddMainMenuButton( multiplayerString, MainMenuItemType::MultiPlayer );
	AddMainMenuButton( optionsString, MainMenuItemType::Options );
	AddMainMenuButton( quitString, MainMenuItemType::Quit );

	CenterMainMenuButtons( );
	CenterOptionsButtons( );
}
void Renderer::AddMainMenuButton( const std::string &menuItemString, const MainMenuItemType &mit )
{
	switch ( mit )
	{
		case MainMenuItemType::SinglePlayer:
			singlePlayerButton = AddMenuButtonHelper( menuItemString, { 0, 0, 0, 0 }, mediumFont  );
			singlePlayerButton->SetRectXY(
				margin / 2,
				background.h - ( ( background.h - greyArea.rect.h ) / 2)  + ( singlePlayerButton->GetRectH( ))
			);
			break;
		case MainMenuItemType::MultiPlayer:
			multiPlayerButton = AddMenuButtonHelper( menuItemString, singlePlayerButton->GetRect(), mediumFont );
			break;
		case MainMenuItemType::Options:
			optionsButton = AddMenuButtonHelper( menuItemString, multiPlayerButton->GetRect(), mediumFont );
			break;
		case MainMenuItemType::Quit:
			quitButton = AddMenuButtonHelper( menuItemString, optionsButton->GetRect(), mediumFont );
			break;
		case MainMenuItemType::Unknown:
			break;
	}
}
std::shared_ptr< MenuItem > Renderer::AddMenuButtonHelper( std::string menuItemString, const SDL_Rect &singlePlayerRect, TTF_Font* textFont )
{
	SDL_Rect r;
	std::shared_ptr< MenuItem > menuItem = std::make_shared< MenuItem >( menuItemString );

	menuItem->SetTexture( RenderHelpers::RenderTextTexture_Blended( textFont, menuItemString, colorConfig.textColor, r, renderer ) );
	r.x = singlePlayerRect.x + singlePlayerRect.w + margin;
	r.y = singlePlayerRect.y;

	menuItem->SetRect( r );
	menuItem->SetName( menuItemString );

	return menuItem;
}
std::shared_ptr< ConfigItem > Renderer::AddOptionsButtonHelper( std::string caption, std::string value, const SDL_Rect &rect, TTF_Font* font_ )
{
	SDL_Rect captionRect = rect;
	auto optionsItem = std::make_shared< ConfigItem >( caption );

	// Caption
	SDL_Texture* text = RenderHelpers::RenderTextTexture_Blended( font_, caption, colorConfig.textColor, captionRect, renderer );
	captionRect.x += margin;
	captionRect.y = greyArea.rect.y + margin;

	optionsItem->SetTexture( text );
	optionsItem->SetRect( captionRect );

	// Value
	SDL_Rect valueRect;
	SDL_Texture* valueTexture = RenderHelpers::RenderTextTexture_Blended( font_, value, colorConfig.textColor, valueRect, renderer );
	valueRect.x = captionRect.x + captionRect.w + ( margin / 2);
	valueRect.y = captionRect.y;

	optionsItem->SetValueRect( valueRect );
	optionsItem->SetValueTexture( valueTexture );

	return optionsItem;
}
void Renderer::SetUnderlineHelper( const std::shared_ptr< MenuItem > &menuItem )
{
	if ( !menuItem || !menuItem->HasValidTexture() || !menuItem->HasUnderlineChanged()  )
		return;

	SDL_Color clr = colorConfig.textColor;
	int style = 0;

	if ( menuItem->IsSelected()  )
	{
		clr = SDL_Color{ 0, 200, 200, 255};
		style = TTF_STYLE_UNDERLINE | TTF_STYLE_ITALIC;
	}

	SDL_Rect r = menuItem->GetRect();
	menuItem->SetTexture( RenderHelpers::RenderTextTexture_Blended( mediumFont, menuItem->GetName(), clr, r, renderer, style ) );
	menuItem->SetRect( r );
}
void Renderer::UpdateConfigValue( const std::shared_ptr< ConfigItem > &item )
{
	if ( !item->HasChanged() )
		return;

	SDL_Rect r = item->GetValueRect();
	std::stringstream ss("");
	ss << item->GetValue();
	item->SetValueTexture
	(
		RenderHelpers::RenderTextTexture_Blended( tinyFont, ss.str(), colorConfig.GetTextColor(), r, renderer, 0 )
	);
	item->SetValueRect( r );
}
void Renderer::CenterMainMenuButtons( )
{
	if ( singlePlayerButton == nullptr )
		return;

	int totoalWidth = quitButton->GetEndX() - singlePlayerButton->GetRectX();
	int freeSpace = background.w - totoalWidth;
	int startingPoint = freeSpace / 2;

	singlePlayerButton->SetRectX( startingPoint );
	multiPlayerButton->SetRectX( singlePlayerButton->GetEndX() + margin );
	optionsButton->SetRectX( multiPlayerButton->GetEndX() + margin );
	quitButton->SetRectX( optionsButton->GetEndX() + margin );

}
void Renderer::CenterOptionsButtons( )
{
	configItems[ ConfigValueType::BallSpeed ] = AddOptionsButtonHelper( "Ball Speed", "666", {0,0,0,0}, tinyFont );
	configItems[ ConfigValueType::BulletSpeed ]= AddOptionsButtonHelper( "Bullet Speed", "666", {0,0,0,0}, tinyFont );

    SDL_Rect r = configItems[ ConfigValueType::BulletSpeed ]->GetRect();
    int32_t moveDown = r.h + 20;
    configItems[ ConfigValueType::BulletSpeed ]->MoveDown( moveDown );

	backToMenuButton = AddMenuButtonHelper( "Main Menu", {0,0,0,0}, mediumFont );
	int32_t xPos = ( background.w / 2) - ( backToMenuButton->GetRectW() / 2 );
	int32_t yPos = singlePlayerButton->GetRectY();
	backToMenuButton->SetRectXY( xPos, yPos );
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
	pauseResumeButton = AddMenuButtonHelper( resumeString, { 0, 0, 0, 0 } , mediumFont );
	pauseResumeButton->SetRectXY( margin / 2, background.h - pauseResumeButton->GetRectH() );

	pauseMainMenuButton = AddMenuButtonHelper( mainMenuString, pauseResumeButton->GetRect(), mediumFont  );

	pauseQuitButton = AddMenuButtonHelper( quitString, pauseMainMenuButton->GetRect(), mediumFont  );

	CenterPauseButtons();
}
void Renderer::CenterPauseButtons( )
{
	int totalWidth = pauseQuitButton->GetEndX() - pauseResumeButton->GetRectX();
	int freeSpace = background.w - totalWidth;
	int startingPoint = freeSpace / 2;

	pauseResumeButton->SetRectX( startingPoint );
	pauseMainMenuButton->SetRectX( pauseResumeButton->GetEndX() + margin );
	pauseQuitButton->SetRectX( pauseMainMenuButton->GetEndX() + margin );
}
void Renderer::AddLobbyMenuButtons( const std::string &newGame, const std::string &update, const std::string &back )
{
	int32_t xPos = 0;
	int32_t yPos = singlePlayerButton->GetRectY();

	lobbyNewGameButton = AddMenuButtonHelper( newGame, { xPos, yPos, 0, 0 }, mediumFont );
	xPos = lobbyNewGameButton->GetEndX()  + 20;
	lobbyUpdateButton = AddMenuButtonHelper( update, { xPos, yPos, 0, 0 }, mediumFont );
	xPos = lobbyUpdateButton->GetEndX() +  20;
	lobbyBackButton = AddMenuButtonHelper( back, { xPos, yPos, 0, 0 }, mediumFont );

	CenterLobbyButtons();
}
void Renderer::CenterLobbyButtons( )
{
	int32_t width = 20 + lobbyBackButton->GetRectW() + lobbyNewGameButton->GetRectW() + lobbyUpdateButton->GetRectW();
	int32_t newX = lobbyMenuListRect.x + static_cast< int32_t > ( ( lobbyMenuListRect.w * 0.5 ) - ( width * 0.5 ) );

	lobbyNewGameButton->SetRectX( newX );
	newX = lobbyNewGameButton->GetEndX() + 20;
	lobbyUpdateButton->SetRectX( newX );
	newX = lobbyUpdateButton->GetEndX() + 20;
	lobbyBackButton->SetRectX( newX );
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
SDL_Rect Renderer::CalcMenuListRect()
{
	lobbyMenuListRect.w = static_cast< int32_t > ( greyArea.rect.w * 0.6 );
	lobbyMenuListRect.h = static_cast< int32_t > ( greyArea.rect.h * 0.9 );

	lobbyMenuListRect.x = static_cast< int32_t > ( ( background.w * 0.5   ) - ( lobbyMenuListRect.w * 0.5 ) );
	lobbyMenuListRect.y = static_cast< int32_t > ( ( greyArea.rect.h * 0.5 ) - ( lobbyMenuListRect.h * 0.5 ) ) + greyArea.rect.y;

	return lobbyMenuListRect;
}
void Renderer::InitGameList()
{
	CalcMenuListRect();

	gameList = std::make_shared< MenuList >();
	gameList->Init( renderer, lobbyMenuListRect, colorConfig.GetBackgroundColor() );
	gameList->InitTexture( renderer, "Available Games : ", font, colorConfig.GetTextColor() );
}
const std::shared_ptr< MenuList >  &Renderer::GetGameList()
{
	return gameList;
}
void Renderer::AddGameToList( GameInfo gameInfo )
{
	gameList->AddItem( gameInfo, renderer, tinyFont, colorConfig.GetTextColor()  );
}
void Renderer::ClearGameList()
{
	gameList->ClearList();
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

	UpdateConfigValue( configItems[ ConfigValueType::BallSpeed ]);
	UpdateConfigValue( configItems[ ConfigValueType::BulletSpeed ]);
	localPlayerText.Update( delta );

	SetUnderlineHelper( backToMenuButton );

	// Main menu mode
	// =============================================
	SetUnderlineHelper( singlePlayerButton  );
	SetUnderlineHelper( multiPlayerButton  );
	SetUnderlineHelper( optionsButton  );
	SetUnderlineHelper( quitButton  );

	// Pause menu mode
	// =============================================
	SetUnderlineHelper( pauseResumeButton );
	SetUnderlineHelper( pauseMainMenuButton );
	SetUnderlineHelper( pauseQuitButton );

	// MultiplayerButton menu item
	// =============================================
	SetUnderlineHelper( lobbyNewGameButton );
	SetUnderlineHelper( lobbyUpdateButton );
	SetUnderlineHelper( lobbyBackButton );
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
// ==============================================================================================
// =================================== Clean Up  ================================================
// ==============================================================================================
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
// ==============================================================================================
// =================================== Getters  ================================================
// ==============================================================================================
void Renderer::SetIsTwoPlayerMode( bool isTwoPlayerMode_ )
{
	isTwoPlayerMode = isTwoPlayerMode_;
}
SDL_Color Renderer::GetTileColor( std::shared_ptr< Tile > tile  ) const
{
	if ( tile->GetTileType() == TileType::Hard )
		return GetHardTileColor( 5 - tile->GetHitsLeft() );
	else
		return GetTileColor( tile->GetTileTypeAsIndex() );
}
SDL_Color Renderer::GetTileColor( uint64_t type ) const
{
	return colorConfig.GetTileColor( static_cast < TileType > ( type ));
}

SDL_Color Renderer::GetHardTileColor( uint64_t index ) const
{
	return colorConfig.GetTileColor( TileType::Hard, index );
}
const std::shared_ptr< MenuItem > &Renderer::GetMainMenuItem( const MainMenuItemType &type ) const
{
	switch ( type )
	{
		case MainMenuItemType::SinglePlayer:
			return singlePlayerButton;
		case MainMenuItemType::MultiPlayer:
			return multiPlayerButton;
		case MainMenuItemType::Options:
			return optionsButton;
		case MainMenuItemType::Quit:
			return quitButton;
		case MainMenuItemType::Unknown:
			return singlePlayerButton;
	}
}
const std::shared_ptr< MenuItem > &Renderer::GetLobbyMenuItem( const LobbyMenuItem &type ) const
{
	switch ( type )
	{
		case LobbyMenuItem::NewGame:
			return lobbyNewGameButton;
		case LobbyMenuItem::Update:
			return lobbyUpdateButton;
		case LobbyMenuItem::Back:
			return lobbyBackButton;
		case LobbyMenuItem::GameList:
			return quitButton;
		case LobbyMenuItem::Unknown:
			return singlePlayerButton;
	}
}
const std::shared_ptr< MenuItem > &Renderer::GetPauseMenuItem( const PauseMenuItemType &type ) const
{
	switch ( type )
	{
		case PauseMenuItemType::Resume:
			return pauseResumeButton;
		case PauseMenuItemType::MainMenu:
			return pauseMainMenuButton;
		case PauseMenuItemType::Quit:
			return pauseQuitButton;
		case PauseMenuItemType::Unknown:
			return pauseQuitButton;
	}
}
