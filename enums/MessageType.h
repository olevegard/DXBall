#pragma once

enum MessageType
{
	PaddlePosition, 	// Contains paddle position
	PaddleResize, 	// Contains new paddle size ( for use with paddle shrik / enlarge )

	BallSpawned, 	// New ball spawned. Contains ID, position, speed and direction.
	BallData,    	// ID, position, speed, direction
	BallKilled,  	// ID of ball

	TileHit,		// Player hit a tile, remove tile, reduce tile HP or trigger explosion.

	GameSettings,		// Contains resolution of the remote player. Sent during beggining and when resolution chages.
	GameStateChanged,	// Game was paused by remote player
	GameInstance,		// A new game was started, display it in the list of games
	NewGame,			// Tell the server than you have started a game

	BonusSpawned,	// A bonus was triggeredby the oponenyyt
	BonusPickup, 	// A bonus was picked up by the oponent
};
