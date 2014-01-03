#pragma once

enum MessageType
{
	PaddlePosition, 	// Contains paddle position
	PaddleResize, 		// Contains new paddle size ( for use with paddle shrik / enlarge )

	BallSpawned, 		// New ball spawned. Contains ID, position, speed and direction.
	BallData,    		// ID, position, speed, direction
	BallKilled,  		// ID of ball

	TileHit,			// Player hit a tile, remove tile, reduce tile HP or trigger explosion.

	GameSettings,		// Contains resolution of the remote player. Sent during beggining and when resolution chages.
	GameStateChanged,	// Game was paused by remote player
	GameInstance,		// A new game was started, display it in the list of games
	GameJoined,			// Game was joined by oponent

	NewGame,			// Tell the server than you have started a game
	EndGame,			// Tell the server that the game has ended
	GetGameList,		// Tell the server to send you the list of avaibale games

	PlayerName,			// Tell the other player your name
	LevelDone,			// Tell the other player that the level is done on your side

	BonusSpawned,		// A bonus was triggeredby the oponenyyt
	BonusPickup, 		// A bonus was picked up by the oponent

	BulletFire,			// Bullet was fired from oponent
	BulletKilled,		// Bullet fired from opnent was killed
};
