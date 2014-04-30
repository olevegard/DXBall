#pragma once

struct PlayerInfo
{
	PlayerInfo()
		:	lastBulletFired( 0 )
		,	fireInterval( 1 )
		,	fireInterval_Normal( 1 )
		,	fireInterval_Slow( 2000 )
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

		 if ( bonusType == BonusType::SuperBall )
		 {
			 if ( isActive && !fastMode )
				 fireInterval = fireInterval_Slow;
			 else
				 fireInterval = fireInterval_Normal;
		 }
	}
	void ReduceLifes()
	{
		if ( lives == 0 )
			return;

		--lives;
		RemoveAllBonuses();
	}
	void RemoveAllBonuses()
	{
		for ( auto &p : bonusMap )
			p.second = false;
	}
	bool CanSpawnNewBall()
	{
		return ( lives > 0 && activeBalls == 0 );
	}
	bool CanFireBullet()
	{
		if ( !IsBonusActive( BonusType::FireBullets ) )
			return false;

		bool canFire =  ( SDL_GetTicks() - lastBulletFired  ) > fireInterval;

		if ( canFire )
			ResetBulletTimer();

		return canFire;
	}
	void ResetBulletTimer()
	{
		lastBulletFired = SDL_GetTicks();
	}
	std::string name;
	uint32_t points;
	uint32_t lives;
	uint32_t activeBalls;
	uint32_t lastBulletFired;
	uint32_t fireInterval;
	uint32_t fireInterval_Normal;
	uint32_t fireInterval_Slow; // Fire speed when SuperBall and not fast mode
	std::map< BonusType, bool > bonusMap;
	double ballSpeed;
	bool fastMode;
};
