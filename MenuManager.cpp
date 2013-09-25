#include <string>

#include "MenuManager.h"
#include "Renderer.h"
#include "math/RectHelpers.h"

MenuManager::MenuManager()
	:	currentGameState( GameState::MainMenu )
	,	prevGameState( GameState::Quit )

	,	hasGameStateChanged( false )

	,	singlePlayer( "Single Player" )
	,	multiPlayer ( "Multiplayer"   )
	,	options     ( "Options"       )
	,	quit        ( "Quit"          )

	,	pauseResumeButton  ( "Resume"   )
	,	pauseMainMenuButton( "Main Menu"       )
	,	pauseQuitButton    ( "Quit"          )
{

}
void MenuManager::AddMenuElememts( Renderer &renderer )
{
	renderer.AddMainMenuButtons( "Single Player", "Multiplayer", "Options", "Quit" );

	singlePlayer.SetRect( renderer.GetSinglePlayerRect() );
	multiPlayer.SetRect( renderer.GetMultiplayerPlayerRect() );
	options.SetRect( renderer.GetOptionsPlayerRect() );
	quit.SetRect( renderer.GetQuitPlayerRect() );
}

void MenuManager::AddPauseMenuElememts( Renderer &renderer )
{
	renderer.AddPauseMenuButtons( pauseResumeButton.GetName(), pauseMainMenuButton.GetName(), pauseQuitButton.GetName() );

	pauseResumeButton.SetRect( renderer.GetPauseResumeRect() );
	pauseMainMenuButton.SetRect( renderer.GetPauseMainMenuRect() );
	pauseQuitButton.SetRect( renderer.GetPauseQuitRect() );
}
void MenuManager::CheckItemMouseOver( int x, int y, Renderer &renderer )
{
	if ( currentGameState == GameState::Paused )
	{
		CheckItemMouseOver_Pause( x, y, renderer );
		return;
	}

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
void MenuManager::CheckItemMouseOver_Pause( int x, int y, Renderer &renderer )
{
	PauseMenuItemType mouseOver = CheckIntersections_Pause( x, y );

	renderer.SetMainMenuItemUnderline( true, mouseOver );
	switch ( mouseOver )
	{
		case PauseMenuItemType::Resume:
			std::cout << "mouse over resume " << std::endl;
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
	switch ( CheckIntersections( x, y))
	{
		case MainMenuItemType::SinglePlayer:
			SetGameState( GameState::InGame );
			break;
		case MainMenuItemType::MultiPlayer:
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

	return true;
}
MainMenuItemType MenuManager::CheckIntersections( int x, int y )
{
	if ( RectHelpers::CheckMouseIntersection( x, y, singlePlayer.GetRect() ) )
		return MainMenuItemType::SinglePlayer;

	if ( RectHelpers::CheckMouseIntersection( x, y, multiPlayer.GetRect() ) )
		return MainMenuItemType::MultiPlayer;

	if ( RectHelpers::CheckMouseIntersection( x, y, options.GetRect() ) )
		return MainMenuItemType::Options;

	if ( RectHelpers::CheckMouseIntersection( x, y, quit.GetRect() ) )
		return MainMenuItemType::Quit;

	return MainMenuItemType::Unknown;
}
PauseMenuItemType MenuManager::CheckIntersections_Pause( int x, int y )
{
	if ( RectHelpers::CheckMouseIntersection( x, y, pauseResumeButton.GetRect() ) )
		return PauseMenuItemType::Resume;

	if ( RectHelpers::CheckMouseIntersection( x, y, pauseMainMenuButton.GetRect() ) )
		return PauseMenuItemType::MainMenu;

	if ( RectHelpers::CheckMouseIntersection( x, y, pauseQuitButton.GetRect() ) )
		return PauseMenuItemType::Quit;

	return PauseMenuItemType::Unknown;
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
void MenuManager::SetGameState( GameState gs )
{
	hasGameStateChanged = true;
	prevGameState = currentGameState;
	currentGameState = gs;
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
	else if ( prevGameState == GameState::Paused )
		SetGameState( GameState::MainMenu );
	else
		SetGameState( prevGameState );

	return currentGameState;
}
