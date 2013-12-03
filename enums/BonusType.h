#pragma once

enum class BonusType
{
	// Local player bonuses
	ExtraLife, // Obvious
	Death, // Loose 1 life
	SuperBall, // Ball goes trhough everything
	FireBullets,


	BallSplit, // Ball split into two

	// Remote player bonus.
	BallSteal, // Remote player looses a ball to local  player. ( Can be changed to 'looses all balls but one' )
	BallLoose, // Local  player looses a ball to remote player. ( Can be changed to 'looses all balls but one' )
	DeathWall, // A line of special wall of death tiles covers the screen left to right above/under remote player paddle. Wall moves toward oponent every x seconds.

	BonusSteal, // Steal all bonuses from oponent.

};
