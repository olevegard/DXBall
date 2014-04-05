#include "MenuManager.h"

#include <string>
#include <memory>

#include "structs/menu_items/MenuList.h"
#include "structs/menu_items/MenuItem.h"
#include "math/RectHelpers.h"

#include "Logger.h"

MenuManager::MenuManager( ConfigLoader &configLoader_ )
	:	currentGameState( GameState::MainMenu )
	,	prevGameState( GameState::Quit )

	,	hasGameStateChanged( false )
	,	hasLobbyStateChanged( false  )
	,	isTwoPlayerMode( false )

	,	selectedGameID( -1 )
	,	configLoader( configLoader_ )
{
	logger = Logger::Instance();
}
void MenuManager::InitConfigList()
{
	auto &configItems = configList->GetConfigList();

	for ( auto &item : configItems )
	{
		configList->Set( static_cast< uint32_t > ( configLoader.Get( item.first ) ) , item.first );
	}
}
void MenuManager::CheckItemMouseOver( int x, int y )
{
	if ( currentGameState == GameState::Paused )
		CheckItemMouseOver_Pause( x, y );
	else if ( currentGameState == GameState::MainMenu )
		CheckItemMouseOver_MainMenu( x, y );
	else if ( currentGameState == GameState::Lobby )
		CheckItemMouseOver_Lobby( x, y );
	else
		CheckItemMouseOver_Options( x, y );
}
void MenuManager::CheckItemMouseOver_MainMenu( int x, int y )
{
	MainMenuItemType mouseOver = CheckIntersections( x, y );

	for ( const auto &item : mainMenuItems )
	{
		if ( item.first ==  mouseOver )
			item.second->SetSelcted( true );
		else
			item.second->SetSelcted( false );
	}
}
void MenuManager::CheckItemMouseOver_Pause( int x, int y )
{
	PauseMenuItemType mouseOver = CheckIntersections_Pause( x, y );

	for ( const auto &item : pauseMenuItems )
	{
		if ( item.first ==  mouseOver )
			item.second->SetSelcted( true );
		else
			item.second->SetSelcted( false );
	}
}
void MenuManager::CheckItemMouseOver_Lobby( int x, int y )
{
	LobbyMenuItem mouseOver = CheckIntersections_Lobby( x, y );

	for ( const auto &item : lobbyMenuItems )
	{
		if ( item.first ==  mouseOver )
			item.second->SetSelcted( true );
		else
			item.second->SetSelcted( false );
	}
}
void MenuManager::CheckItemMouseOver_Options( int x, int y )
{
	if ( RectHelpers::CheckMouseIntersection( x, y, backToMenuButton->GetRect() ) )
		backToMenuButton->SetSelcted( true );
	else
		backToMenuButton->SetSelcted( false );
}
PlussMin MenuManager::CheckConfigItemsClick( int32_t x, int32_t y, const std::shared_ptr< ConfigItem > &item )
{
	if ( RectHelpers::CheckMouseIntersection( x, y, item->GetPlussRect() ) )
		return PlussMin::Pluss;

	if ( RectHelpers::CheckMouseIntersection( x, y, item->GetMinusRect() ) )
		return PlussMin::Minus;

	return PlussMin::Equal;
}
void MenuManager::CheckItemMouseClick( int x, int y)
{
	if ( currentGameState == GameState::Paused )
	{
		switch ( CheckIntersections_Pause( x, y))
		{
			case PauseMenuItemType::Resume:
				SetGameState( GameState::InGame );
				break;
			case PauseMenuItemType::MainMenu:
				SetGameState( GameState::MainMenu );
				break;
			case PauseMenuItemType::Quit:
				SetGameState( GameState::Quit );
				break;
			case PauseMenuItemType::Unknown:
				break;
		}
	}
	else if ( currentGameState == GameState::MainMenu )
	{
		switch ( CheckIntersections( x, y))
		{
			case MainMenuItemType::SinglePlayer:
				SetGameState( GameState::InGame );
				isTwoPlayerMode = false;
				break;
			case MainMenuItemType::MultiPlayer:
				isTwoPlayerMode = true;
				SetGameState( GameState::Lobby );
				break;
			case MainMenuItemType::Options:
				SetGameState( GameState::Options );
				break;
			case MainMenuItemType::Quit:
				SetGameState( GameState::Quit );
				break;
			case MainMenuItemType::Unknown:
				break;
		}
	}
	else if ( currentGameState == GameState::Lobby )
	{
		lobbyState = CheckIntersections_Lobby( x, y);

		if ( lobbyState == LobbyMenuItem::GameList )
			selectedGameID= lobbyGameList->FindIntersectedItem( x, y );
		else
			selectedGameID= -1;

		hasLobbyStateChanged = true;
	}
	else
	{
		configList->CheckScrollBarIntersection( x, y );
		CheckOptionsIntersections( x, y );
	}
}
void MenuManager::CheckOptionsIntersections( int32_t x, int32_t y )
{
	if ( RectHelpers::CheckMouseIntersection( x, y, backToMenuButton->GetRect() ) )
	{
		SetGameState( GameState::MainMenu );
		return;
	}

	CheckMenuItemIntersections( x, y );
}
void MenuManager::CheckMenuItemIntersections( int32_t x, int32_t y )
{
	auto plussMin = PlussMin::Equal;
	auto type     = ConfigValueType::BallSpeed;
	auto &configItems = configList->GetConfigList();

	for ( auto &item : configItems )
	{
		plussMin = CheckConfigItemsClick( x, y,   item.second );
		if ( plussMin != PlussMin::Equal)
		{
			type = item.first;
			break;
		}
	}
	if ( plussMin != PlussMin::Equal )
	{
		configLoader.ApplyChange( type, 10, plussMin );
		configList->Set( static_cast< uint32_t > ( configLoader.Get( type ) ) , type  );
	}
}
MainMenuItemType MenuManager::CheckIntersections( int x, int y )
{
	if ( RectHelpers::CheckMouseIntersection( x, y, mainMenuItems[MainMenuItemType::SinglePlayer]->GetRect() ) )
		return MainMenuItemType::SinglePlayer;

	if ( RectHelpers::CheckMouseIntersection( x, y, mainMenuItems[MainMenuItemType::MultiPlayer]->GetRect() ) )
		return MainMenuItemType::MultiPlayer;

	if ( RectHelpers::CheckMouseIntersection( x, y, mainMenuItems[MainMenuItemType::Options]->GetRect() ) )
		return MainMenuItemType::Options;

	if ( RectHelpers::CheckMouseIntersection( x, y, mainMenuItems[MainMenuItemType::Quit]->GetRect() ) )
		return MainMenuItemType::Quit;

	return MainMenuItemType::Unknown;
}
PauseMenuItemType MenuManager::CheckIntersections_Pause( int x, int y )
{
	if ( RectHelpers::CheckMouseIntersection( x, y, pauseMenuItems[PauseMenuItemType::Resume]->GetRect() ) )
		return PauseMenuItemType::Resume;

	if ( RectHelpers::CheckMouseIntersection( x, y, pauseMenuItems[PauseMenuItemType::MainMenu]->GetRect() ) )
		return PauseMenuItemType::MainMenu;

	if ( RectHelpers::CheckMouseIntersection( x, y, pauseMenuItems[PauseMenuItemType::Quit]->GetRect() ) )
		return PauseMenuItemType::Quit;

	return PauseMenuItemType::Unknown;
}
LobbyMenuItem MenuManager::CheckIntersections_Lobby( int x, int y )
{
	if ( RectHelpers::CheckMouseIntersection( x, y, lobbyMenuItems[LobbyMenuItem::NewGame]->GetRect() ) )
		return LobbyMenuItem::NewGame;

	if ( RectHelpers::CheckMouseIntersection( x, y, lobbyMenuItems[LobbyMenuItem::Update]->GetRect() ) )
		return LobbyMenuItem::Update;

	if ( RectHelpers::CheckMouseIntersection( x, y, lobbyMenuItems[LobbyMenuItem::Back]->GetRect() ) )
		return LobbyMenuItem::Back;

	if ( RectHelpers::CheckMouseIntersection( x, y, lobbyGameList->GetRect() ) )
		return LobbyMenuItem::GameList;

	return LobbyMenuItem::Unknown;
}
GameState MenuManager::GetGameState() const
{
	return currentGameState;
}
GameState MenuManager::GetPrevGameState() const
{
	return prevGameState;
}
void MenuManager::SetGameState( const GameState &gs )
{
	if ( !IsGameStateChangeValid( gs ) )
		return;

	std::stringstream ss("");
	ss << "from " << static_cast< int32_t > ( currentGameState  )
	<< " to " << static_cast< int32_t > ( gs );

	logger->Log( __FILE__, __LINE__, "Changing game state : ", ss.str() );

	hasGameStateChanged = true;
	prevGameState = currentGameState;
	currentGameState = gs;
}
bool MenuManager::IsGameStateChangeValid( const GameState &gs) const
{
	if ( gs == currentGameState )
	{
		logger->Log( __FILE__, __LINE__, "GameState same as before!");
		return false;
	}

	return true;
}
bool MenuManager::HasGameStateChanged()
{
	if ( hasGameStateChanged )
	{
		hasGameStateChanged = false;
		return true;
	}

	return false;
}
GameState MenuManager::GoBackToPreviousMenuState()
{
	if ( currentGameState == GameState::MainMenu )
		SetGameState( GameState::Quit );
	else
		SetGameState( prevGameState );

	return currentGameState;
}
GameState MenuManager::GoToMenu()
{
	if ( currentGameState == GameState::MainMenu )
		SetGameState( GameState::Quit );
	else
		SetGameState( GameState::MainMenu );

	return currentGameState;
}
LobbyMenuItem MenuManager::GetLobbyState()
{
	return lobbyState;
}
bool MenuManager::HasLobbyStateChanged()
{
	bool state = hasLobbyStateChanged;
	hasLobbyStateChanged = false;
	return state;
}
bool MenuManager::WasGameStarted() const
{
	return
	(
		currentGameState == GameState::InGame &&
		prevGameState  != GameState::Lobby &&
		prevGameState  != GameState::Paused &&
		prevGameState  != GameState::InGameWait
	);
}
bool MenuManager::WasGameQuited() const
{
	return ( currentGameState != GameState::GameOver  && currentGameState != GameState::Paused && prevGameState == GameState::InGame );
}
bool MenuManager::WasGameResumed() const
{
	return ( currentGameState == GameState::InGame && prevGameState == GameState::Paused );
}
bool MenuManager::IsTwoPlayerMode() const
{
	return isTwoPlayerMode;
}
bool MenuManager::IsInAMenu() const
{
	return ( currentGameState == GameState::MainMenu
			|| currentGameState == GameState::Paused
			|| currentGameState == GameState::Lobby
			|| currentGameState == GameState::Options
	 );
}
void MenuManager::SetMainMenuItem( const MainMenuItemType &type, const std::shared_ptr< MenuItem >& button )
{
	mainMenuItems[type] = button;
}
void MenuManager::SetLobbyMenuItem( const LobbyMenuItem &type, const std::shared_ptr< MenuItem >  &button )
{
	lobbyMenuItems[type] = button;
}
void MenuManager::SetPauseMenuItem( const PauseMenuItemType &type, const std::shared_ptr< MenuItem >  &button )
{
	pauseMenuItems[type] = button;
}
int32_t MenuManager::GetSelectedGame() const
{
	return selectedGameID;
}
bool MenuManager::IsAnItemSelected() const
{
	return ( selectedGameID >= 0 );
}
GameInfo MenuManager::GetSelectedGameInfo() const
{
	return lobbyGameList->GetGameInfoForIndex( selectedGameID );
}
void MenuManager::SetGameList( const std::shared_ptr< MenuList >  gameList_ )
{
	lobbyGameList = gameList_;
}
void MenuManager::SetGameList( const std::shared_ptr< ConfigList >  configList_ )
{
	configList = configList_;
}
void MenuManager::SetOptionsMenuItem( const std::shared_ptr< MenuItem >  &button )
{
	backToMenuButton = button;
}
void MenuManager::SetConfigList( const std::shared_ptr < ConfigList > &configList_ )
{
	configList = configList_;
	InitConfigList();
}
