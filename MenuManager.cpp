#include <string>
#include <memory>

#include "MenuManager.h"
#include "MenuList.h"
#include "Renderer.h"
#include "Logger.h"

#include "math/RectHelpers.h"

MenuManager::MenuManager()
	:	currentGameState( GameState::MainMenu )
	,	prevGameState( GameState::Quit )

	,	hasGameStateChanged( false )
	,	isTwoPlayerMode( false )

	,	seletedGameID( -1 )
	,	lobbyStateChanged( false  )
{
	logger = Logger::Instance();
}
void MenuManager::CheckItemMouseOver( int x, int y, Renderer &renderer ) const
{
	if ( currentGameState == GameState::Paused )
	{
		CheckItemMouseOver_Pause( x, y, renderer );
	}
	else if ( currentGameState == GameState::MainMenu )
	{
		CheckItemMouseOver_MainMenu( x, y, renderer );
	}
	else if ( currentGameState == GameState::Lobby )
	{
		CheckItemMouseOver_Lobby( x, y, renderer );
	}
}
void MenuManager::CheckItemMouseOver_MainMenu( int x, int y, Renderer &renderer ) const
{
	MainMenuItemType mouseOver = CheckIntersections( x, y );

	renderer.SetMainMenuItemUnderline( true, mouseOver );
	switch ( mouseOver )
	{
		case MainMenuItemType::SinglePlayer:
			renderer.SetMainMenuItemUnderline( false, MainMenuItemType::MultiPlayer );
			renderer.SetMainMenuItemUnderline( false, MainMenuItemType::Options );
			renderer.SetMainMenuItemUnderline( false, MainMenuItemType::Quit );
			break;
		case MainMenuItemType::MultiPlayer:
			renderer.SetMainMenuItemUnderline( false, MainMenuItemType::SinglePlayer );
			renderer.SetMainMenuItemUnderline( false, MainMenuItemType::Options );
			renderer.SetMainMenuItemUnderline( false, MainMenuItemType::Quit );
			break;
		case MainMenuItemType::Options:
			renderer.SetMainMenuItemUnderline( false, MainMenuItemType::SinglePlayer );
			renderer.SetMainMenuItemUnderline( false, MainMenuItemType::MultiPlayer );
			renderer.SetMainMenuItemUnderline( false, MainMenuItemType::Quit );
			break;
		case MainMenuItemType::Quit:
			renderer.SetMainMenuItemUnderline( false, MainMenuItemType::SinglePlayer );
			renderer.SetMainMenuItemUnderline( false, MainMenuItemType::MultiPlayer );
			renderer.SetMainMenuItemUnderline( false, MainMenuItemType::Options );
			break;
		case MainMenuItemType::Unknown:
			renderer.SetMainMenuItemUnderline( false, MainMenuItemType::SinglePlayer );
			renderer.SetMainMenuItemUnderline( false, MainMenuItemType::MultiPlayer );
			renderer.SetMainMenuItemUnderline( false, MainMenuItemType::Options );
			renderer.SetMainMenuItemUnderline( false, MainMenuItemType::Quit );
			break;
	}

}
void MenuManager::CheckItemMouseOver_Pause( int x, int y, Renderer &renderer ) const
{
	PauseMenuItemType mouseOver = CheckIntersections_Pause( x, y );

	renderer.SetMainMenuItemUnderline( true, mouseOver );
	switch ( mouseOver )
	{
		case PauseMenuItemType::Resume:
			renderer.SetMainMenuItemUnderline( false, PauseMenuItemType::MainMenu );
			renderer.SetMainMenuItemUnderline( false, PauseMenuItemType::Quit );
			break;
		case PauseMenuItemType::MainMenu:
			renderer.SetMainMenuItemUnderline( false, PauseMenuItemType::Resume);
			renderer.SetMainMenuItemUnderline( false, PauseMenuItemType::Quit );
			break;
		case PauseMenuItemType::Quit :
			renderer.SetMainMenuItemUnderline( false, PauseMenuItemType::Resume);
			renderer.SetMainMenuItemUnderline( false, PauseMenuItemType::MainMenu );
			break;
		case PauseMenuItemType::Unknown:
			renderer.SetMainMenuItemUnderline( false, PauseMenuItemType::Resume);
			renderer.SetMainMenuItemUnderline( false, PauseMenuItemType::MainMenu );
			renderer.SetMainMenuItemUnderline( false, PauseMenuItemType::Quit );
			break;
	}
}

void MenuManager::CheckItemMouseOver_Lobby( int x, int y, Renderer &renderer ) const
{
	LobbyMenuItem mouseOver = CheckIntersections_Lobby( x, y );

	renderer.SetLobbyItemUnderline( true, mouseOver );
	switch ( mouseOver )
	{
		case LobbyMenuItem::NewGame:
			renderer.SetLobbyItemUnderline( false, LobbyMenuItem::Update );
			renderer.SetLobbyItemUnderline( false, LobbyMenuItem::Back );
			break;
		case LobbyMenuItem::Update:
			renderer.SetLobbyItemUnderline( false, LobbyMenuItem::NewGame );
			renderer.SetLobbyItemUnderline( false, LobbyMenuItem::Back );
			break;
		case LobbyMenuItem::Back:
			renderer.SetLobbyItemUnderline( false, LobbyMenuItem::NewGame );
			renderer.SetLobbyItemUnderline( false, LobbyMenuItem::Update );
			break;
		case LobbyMenuItem::GameList:
			break;
		case LobbyMenuItem::Unknown:
			renderer.SetLobbyItemUnderline( false, LobbyMenuItem::NewGame );
			renderer.SetLobbyItemUnderline( false, LobbyMenuItem::Update );
			renderer.SetLobbyItemUnderline( false, LobbyMenuItem::Back );
			break;
	}
}
bool MenuManager::CheckItemMouseClick( int x, int y)
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
				return false;
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
				break;
			case MainMenuItemType::Quit:
				SetGameState( GameState::Quit );
				break;
			case MainMenuItemType::Unknown:
				return false;
		}
	}
	else if ( currentGameState == GameState::Lobby )
	{
		lobbyState = CheckIntersections_Lobby( x, y);

		if ( lobbyState == LobbyMenuItem::GameList )
			seletedGameID = lobbyGameList->FindIntersectedItem( x, y );
		else
			seletedGameID = -1;

		lobbyStateChanged = true;
		return true;
	}

	return false;
}
MainMenuItemType MenuManager::CheckIntersections( int x, int y ) const
{
	if ( RectHelpers::CheckMouseIntersection( x, y, singlePlayerButton->GetRect() ) )
		return MainMenuItemType::SinglePlayer;

	if ( RectHelpers::CheckMouseIntersection( x, y, multiPlayerButton->GetRect() ) )
		return MainMenuItemType::MultiPlayer;

	if ( RectHelpers::CheckMouseIntersection( x, y, optionsButton->GetRect() ) )
		return MainMenuItemType::Options;

	if ( RectHelpers::CheckMouseIntersection( x, y, quitButton->GetRect() ) )
		return MainMenuItemType::Quit;

	return MainMenuItemType::Unknown;
}
PauseMenuItemType MenuManager::CheckIntersections_Pause( int x, int y ) const
{
	if ( RectHelpers::CheckMouseIntersection( x, y, pauseResumeButton->GetRect() ) )
		return PauseMenuItemType::Resume;

	if ( RectHelpers::CheckMouseIntersection( x, y, pauseMainMenuButton->GetRect() ) )
		return PauseMenuItemType::MainMenu;

	if ( RectHelpers::CheckMouseIntersection( x, y, pauseQuitButton->GetRect() ) )
		return PauseMenuItemType::Quit;

	return PauseMenuItemType::Unknown;
}
LobbyMenuItem MenuManager::CheckIntersections_Lobby( int x, int y ) const
{
	if ( RectHelpers::CheckMouseIntersection( x, y, lobbyNewGameButton->GetRect() ) )
		return LobbyMenuItem::NewGame;

	if ( RectHelpers::CheckMouseIntersection( x, y, lobbyUpdateButton->GetRect() ) )
		return LobbyMenuItem::Update;

	if ( RectHelpers::CheckMouseIntersection( x, y, lobbyBackButton->GetRect() ) )
		return LobbyMenuItem::Back;

	if ( RectHelpers::CheckMouseIntersection( x, y, lobbyGameList->GetRect() ) )
		return LobbyMenuItem::GameList;

	return LobbyMenuItem::Unknown;
}
void MenuManager::RemoevAllUnderscores( Renderer &renderer )
{
	renderer.RemoveMainMenuItemsUnderlines();
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
	bool state = lobbyStateChanged;
	lobbyStateChanged = false;
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
		   );
}
void MenuManager::SetMainMenuItem( const MainMenuItemType &type, const std::shared_ptr< MenuItem >& button )
{
	switch ( type )
	{
		case MainMenuItemType::SinglePlayer:
			singlePlayerButton = button;
			break;
		case MainMenuItemType::MultiPlayer:
			multiPlayerButton = button;
			break;
		case MainMenuItemType::Options:
			optionsButton = button;
			break;
		case MainMenuItemType::Quit:
			quitButton = button;
			break;
		case MainMenuItemType::Unknown:
			singlePlayerButton = button;
			break;
	}
}
void MenuManager::SetLobbyMenuItem( const LobbyMenuItem &type, const std::shared_ptr< MenuItem >  &button )
{
	switch ( type )
	{
		case LobbyMenuItem::NewGame:
			lobbyNewGameButton = button;
			break;
		case LobbyMenuItem::Update:
			lobbyUpdateButton = button;
			break;
		case LobbyMenuItem::Back:
			lobbyBackButton = button;
			break;
		case LobbyMenuItem::GameList:
			quitButton = button;
			break;
		case LobbyMenuItem::Unknown:
			singlePlayerButton = button;
			break;
	}
}
void MenuManager::SetPauseMenuItem( const PauseMenuItemType &type, const std::shared_ptr< MenuItem >  &button )
{
	switch ( type )
	{
		case PauseMenuItemType::Resume:
			pauseResumeButton = button;
			break;
		case PauseMenuItemType::MainMenu:
			pauseMainMenuButton = button;
			break;
		case PauseMenuItemType::Quit:
			pauseQuitButton = button;
			break;
		case PauseMenuItemType::Unknown:
			pauseQuitButton = button;
			break;
	}
}
