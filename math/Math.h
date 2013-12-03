#pragma once

#include <random>

namespace Math
{
	static double GenRandomNumber( double min, double max )
	{
		static std::random_device rseed;
		std::mt19937 rgen(rseed());
		std::uniform_real_distribution<double> rdist( min, max );

		return rdist(rgen);
	}
	static int GenRandomNumber( int max )
	{
		static std::random_device rseed;
		std::mt19937 rgen(rseed());
		std::uniform_int_distribution<int> rdist( 1, max );

		return rdist(rgen);
	}
}
