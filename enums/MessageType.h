#pragma once

enum MessageType
{
	PaddlePosition, 	// Contains paddle position
	PaddleResize, 	// Contains new paddle size ( for use with paddle shrik / enlarge )

	BallSpawned, 	// New ball spawned. Contains ID, position, speed and direction.
	BallData,    	// ID, position, speed, direction
	BallKilled,  	// ID of ball

	TileHit,

	GameSettings,	// Contains resolution of the remote player. Sent during beggining and when resolution chages.

	BonusSpawned,

	GeneralUpade,	// Contains lifes, points and number of active balls. [ Not needed atm ]

	BoardUpdate	// Contains number of tiles, and position and type information about all tiles.
};
