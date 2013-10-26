#pragma once

enum MessageType
{
	PaddlePosition, 	// Contains paddle position
	PaddleResize, 	// Contains new paddle size ( for use with paddle shrik / enlarge )

	BallSpawned, 	// New ball spawned. Contains ID, position, speed and direction.
	BallData,    	// ID, position, speed, direction
	BallKilled,  	// ID of ball

	TileHit,

	BonusSpawned,

	GeneralUpade,	// Contains lifes, points and number of active balls.

	BoardUpdate	// Contains number of tiles, and position and type information about all tiles. 
};
