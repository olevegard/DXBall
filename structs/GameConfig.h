#pragma once

#include "ConfigLoader.h"

struct GameConfig
{
	public:
		void LoadConfig()
		{
			configLoader.LoadConfig();
			bonusBoxSpeed = configLoader.GetBonusBoxSpeed();
			ballSpeed = configLoader.GetBallSpeed();
			ballSpeedFastMode = configLoader.GetBallSpeedFastMode();
			bulletSpeed = configLoader.GetBulletSpeed();

			isFastMode = configLoader.GetIsFastMode();
			bonusBoxChance = configLoader.GetBonusBoxChance();
		}

		double GetBonusBoxSpeed() const
		{
			return bonusBoxSpeed;
		}
		double GetBallSpeed() const
		{
			return ballSpeed;
		}
		double GetBallSpeedFastMode() const
		{
			return ballSpeedFastMode;
		}
		double GetBulletSpeed() const
		{
			return bulletSpeed;
		}

		bool IsFastMode() const
		{
			return isFastMode;
		}

		int32_t GetBonusBoxChance() const
		{
			return bonusBoxChance;
		}

	private :
		ConfigLoader configLoader;

		double bonusBoxSpeed;
		double ballSpeed;
		double ballSpeedFastMode;
		double bulletSpeed;

		bool isFastMode;

		int32_t bonusBoxChance;
};
