#pragma once

struct PlayerInfo
{
	PlayerInfo()
	{
		bonusMap[BonusType::FireBullets] = false;
		bonusMap[BonusType::SuperBall] = false;
	}
	void Reset()
	{
		points = 0;
		lives = 3;
		activeBalls = 0;
		bonusMap[BonusType::FireBullets] = false;
		bonusMap[BonusType::SuperBall] = false;
	}

	bool IsBonusActive( BonusType bonusType ) const
	{
		return bonusMap.at( bonusType );
	}

	void SetBonusActive( BonusType bonusType, bool isActive )
	{
		 bonusMap[ bonusType ] = isActive;
	}

	std::string name;
	unsigned int points;
	unsigned int lives;
	unsigned int activeBalls;
	std::map< BonusType, bool > bonusMap;
	double ballSpeed;
};
