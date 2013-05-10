#pragma once

#include <sys/stat.h>
#include <time.h>

class Timer
{
public:
	Timer();

	// Restars everything
	void Restart();
	
	// Time since last frame ( for framerate calculation )
	void GetDelta( float &deltaMSec );

	// Elapsed time since last reset ( used for regular updates and elapsed game time )
	unsigned int GetElapsed( bool reset = false );
	
	// /Used for things that should be updated regularly
	bool IsUpdateTime();

	//int GetSleepTime();
private:
	void ResetPrevTime();

	// Returns current time ( with 0 == program startup )
	unsigned long long GetCurrentTime() const;

	// Convert timespec to a timespac in nanoseconds.
	unsigned long long CreateTimeStamp( const timespec &tm ) const;

	unsigned long long start;
	unsigned long long delta;
	unsigned long long update;

	int sleepTime;
};
