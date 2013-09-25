#include <string>

#include "MenuManager.h"
#include "Renderer.h"
#include "math/RectHelpers.h"

MenuManager::MenuManager()
	:	currentGameState( GameState::MainMenu )

	,	hasGameStateChanged( false )

	,	singlePlayer( "Single Player", SDL_Rect{ 10, 100, 100, 100 }, MenuItemType::SinglePlayer )
	,	multiPlayer ( "Multiplayer"  , SDL_Rect{ 10, 100, 100, 100 }, MenuItemType::MultiPlayer )
	,	options     ( "Options"      , SDL_Rect{ 10, 100, 100, 100 }, MenuItemType::Options )
	,	quit        ( "Quit"         , SDL_Rect{ 10, 100, 100, 100 }, MenuItemType::Quit )
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
	MenuItemType mouseOver = CheckIntersections( x, y );

	renderer.SetMainMenuItemUnderline( true, mouseOver );
	switch ( mouseOver )
	{
		case MenuItemType::SinglePlayer:
			renderer.SetMainMenuItemUnderline( false, MenuItemType::MultiPlayer );
			renderer.SetMainMenuItemUnderline( false, MenuItemType::Options );
			renderer.SetMainMenuItemUnderline( false, MenuItemType::Quit );
			break;
		case MenuItemType::MultiPlayer:
			renderer.SetMainMenuItemUnderline( false, MenuItemType::SinglePlayer );
			renderer.SetMainMenuItemUnderline( false, MenuItemType::Options );
			renderer.SetMainMenuItemUnderline( false, MenuItemType::Quit );
			break;
		case MenuItemType::Options:
			renderer.SetMainMenuItemUnderline( false, MenuItemType::SinglePlayer );
			renderer.SetMainMenuItemUnderline( false, MenuItemType::MultiPlayer );
			renderer.SetMainMenuItemUnderline( false, MenuItemType::Quit );
			break;
		case MenuItemType::Quit:
			renderer.SetMainMenuItemUnderline( false, MenuItemType::SinglePlayer );
			renderer.SetMainMenuItemUnderline( false, MenuItemType::MultiPlayer );
			renderer.SetMainMenuItemUnderline( false, MenuItemType::Options );
			break;
		case MenuItemType::Unknown:
			renderer.SetMainMenuItemUnderline( false, MenuItemType::SinglePlayer );
			renderer.SetMainMenuItemUnderline( false, MenuItemType::MultiPlayer );
			renderer.SetMainMenuItemUnderline( false, MenuItemType::Options );
			renderer.SetMainMenuItemUnderline( false, MenuItemType::Quit );
			break;
	}

}
bool MenuManager::CheckItemMouseClick( int x, int y)
{
	switch ( CheckIntersections( x, y))
	{
		case MenuItemType::SinglePlayer:
			SetGameState( GameState::InGame );
			break;
		case MenuItemType::MultiPlayer:
			break;
		case MenuItemType::Options:
			break;
		case MenuItemType::Quit:
			SetGameState( GameState::Quit );
			break;
		case MenuItemType::Unknown:
			return false;
	}

	return true;
}
MenuItemType MenuManager::CheckIntersections( int x, int y )
{
	if ( RectHelpers::CheckMouseIntersection( x, y, singlePlayer.GetRect() ) )
		return MenuItemType::SinglePlayer;

	if ( RectHelpers::CheckMouseIntersection( x, y, multiPlayer.GetRect() ) )
		return MenuItemType::MultiPlayer;

	if ( RectHelpers::CheckMouseIntersection( x, y, options.GetRect() ) )
		return MenuItemType::Options;

	if ( RectHelpers::CheckMouseIntersection( x, y, quit.GetRect() ) )
		return MenuItemType::Quit;

	return MenuItemType::Unknown;
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
