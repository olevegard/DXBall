#include <string>

#include "MenuManager.h"
#include "Renderer.h"
#include "math/RectHelpers.h"

MenuManager::MenuManager()
	:	currentGameState( GameState::MainMenu )
	,	prevGameState( GameState::Quit )

	,	hasGameStateChanged( false )

	,	singlePlayer( "Single Player", SDL_Rect{ 10, 100, 100, 100 }, MainMenuItemType::SinglePlayer )
	,	multiPlayer ( "Multiplayer"  , SDL_Rect{ 10, 100, 100, 100 }, MainMenuItemType::MultiPlayer )
	,	options     ( "Options"      , SDL_Rect{ 10, 100, 100, 100 }, MainMenuItemType::Options )
	,	quit        ( "Quit"         , SDL_Rect{ 10, 100, 100, 100 }, MainMenuItemType::Quit )
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

void MenuManager::CheckItemMouseOver( int x, int y, Renderer &renderer )
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
bool MenuManager::CheckItemMouseClick( int x, int y)
{
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
void MenuManager::RemoevAllUnderscores( Renderer &renderer )
{
	renderer.RemoveMainMenuItemsUnderlines();
}
GameState MenuManager::GetGameState() const
{
	return currentGameState;
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
