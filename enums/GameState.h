#pragma once

enum class GameState
{
	MainMenu,	//0 - Self explanatory
	Lobby,		//1 - Muliplayer menu
	InGame, 	//2 - Game has been started and is active
	Paused, 	//3 - Game is paused
	InGameMenu, //4 - Not used ?
	InGameWait, //5 - A mulitplayer game has been started, but player 2 hasn't joined
	GameOver,	//6 - Game is over, display winner
	Options,	//7 - Display options ( not implemented )
	Quit 		//8 - Game is to be quit
};
