#pragma once

#include <sys/stat.h>
class Timer
{
public:
	Timer()
		:	start(0)
		,	delta(0)
		,	prev(0)

		,	tmStart()
		,	sleepTime( 1000 )
	{
		Restart();
	}
	void Restart()
	{
		// Get current time
	 	clock_gettime( CLOCK_MONOTONIC_RAW, &tmStart);

		// Calc full time value for start
		start = tmStart.tv_sec * 1000000000 + tmStart.tv_nsec;

		// Initilize timestamp to the same as start
		delta = start;
		prev = start;
	}
	void GetDelta( float &deltaMSec )
	{
		timespec tmDelta;
		clock_gettime( CLOCK_MONOTONIC_RAW, &tmDelta);
		unsigned long long deltaCurrent = tmDelta.tv_sec * 1000000000 + tmDelta.tv_nsec;
		unsigned long long diff = deltaCurrent - delta;
		deltaMSec = static_cast< float > ( diff / 1000000.0f );

		delta = deltaCurrent;
	}
	
	unsigned int GetElapsed( bool reset = false )
	{
		timespec tmCurrent;
	 	clock_gettime( CLOCK_MONOTONIC_RAW, &tmCurrent);
		unsigned long long currentTime = CreateTimeStamp( tmCurrent );
		unsigned long long diff = currentTime - prev;

		if ( reset && false )
			prev = currentTime;

		return diff / 1000000;

	}
	
	void ResetPrevTime()
	{
		timespec tmCurrent;
		clock_gettime( CLOCK_MONOTONIC_RAW, &tmCurrent);
		prev = CreateTimeStamp( tmCurrent );
	}

	unsigned long long CreateTimeStamp( const timespec &tm ) const
	{
		unsigned long long timeStamp = tm.tv_sec * 1000000000 + tm.tv_nsec;
		return timeStamp; 
	}

	int GetSleepTime()
	{
		unsigned int elapsed = GetElapsed();
		return  sleepTime - elapsed;
	}

	bool IsUpdateTime()
	{

		bool update = static_cast<int>(  GetElapsed( false ) - sleepTime ) >= 0;
		
		if ( update )
			ResetPrevTime();

		return update;
	}

private:
	unsigned long long start;
	unsigned long long delta;
	unsigned long long prev;

	timespec tmStart;

	int sleepTime;
};
