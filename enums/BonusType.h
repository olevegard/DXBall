#pragma once

enum class BonusType
{
	// Local player bonuses
	Extra_Life, // Obvious
	Ball_Split, // Ball split into two
	Super_Ball, // Ball goes trhough everything

	// Remote player bonus.
	Ball_Steal, // Remote player looses a ball to local  player. ( Can be changed to 'looses all balls but one' )
	Ball_Loose, // Local  player looses a ball to remote player. ( Can be changed to 'looses all balls but one' )
	Death_Wall, // A line of special wall of death tiles covers the screen left to right above/under remote player paddle. Wall moves toward oponent every x seconds.
	Bonus_Steal, // Steal all bonuses from oponent.

};
