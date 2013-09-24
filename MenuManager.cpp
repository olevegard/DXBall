#include <string>

#include "MenuManager.h"
#include "Renderer.h"
#include "math/RectHelpers.h"

MenuManager::MenuManager()
	:	singlePlayer( "Single Player", SDL_Rect{ 10, 100, 100, 100 }, MenuItemType::SinglePlayer )
	,	multiPlayer ( "Multiplayer"  , SDL_Rect{ 10, 100, 100, 100 }, MenuItemType::MultiPlayer )
	,	options     ( "Options"      , SDL_Rect{ 10, 100, 100, 100 }, MenuItemType::Options )
	,	quit        ( "Quit"         , SDL_Rect{ 10, 100, 100, 100 }, MenuItemType::Quit )
	//,	currentState( GameState::MainMenu )
{

}
void MenuManager::AddMenuElememts( Renderer &renderer )
{
	renderer.AddMenuButtons( "Single Player", "Multiplayer", "Options", "Quit" );

	singlePlayer.SetRect( renderer.GetSinglePlayerRect() );
	multiPlayer.SetRect( renderer.GetMultiplayerPlayerRect() );
	options.SetRect( renderer.GetOptionsPlayerRect() );
	quit.SetRect( renderer.GetQuitPlayerRect() );
}

void MenuManager::CheckItemMouseOver( int x, int y, Renderer &renderer )
{
	RemoevAllUnderscores( renderer );

	switch ( CheckIntersections( x, y))
	{
		case MenuItemType::SinglePlayer:
			renderer.SetSinglePlayerUnderline( true );
			break;
		case MenuItemType::MultiPlayer:
			renderer.SetMultiplayerUnderline( true );
			break;
		case MenuItemType::Options:
			renderer.SetOptionsUnderline( true );
			break;
		case MenuItemType::Quit:
			renderer.SetQuitUnderline( true );
			break;
		case MenuItemType::Unknown:
			break;
	}
}
bool MenuManager::CheckItemMouseClick( int x, int y)
{
	switch ( CheckIntersections( x, y))
	{
		case MenuItemType::SinglePlayer:
			currentGameState = GameState::InGame;
			break;
		case MenuItemType::MultiPlayer:
			break;
		case MenuItemType::Options:
			break;
		case MenuItemType::Quit:
			currentGameState = GameState::Quit;
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
	renderer.SetSinglePlayerUnderline( false );
	renderer.SetMultiplayerUnderline( false );
	renderer.SetOptionsUnderline( false );
	renderer.SetQuitUnderline( false );
}
