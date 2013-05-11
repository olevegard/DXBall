#include "Timer.h"
#include <iostream>

#include <sys/stat.h>

Timer::Timer()
	:	start(0)
	,	delta(0)
	,	update(0)

	 ,	sleepTime( 1000 )
{
	Restart();
}

// Restars everything
void Timer::Restart()
{
	// Get current time
	start = GetCurrentTime();

	// Initilize timestamp to the same as start
	delta = start;
	update = start;
}

// Time since last frame ( for framerate calculation )
float Timer::GetDelta( )
{

	// Get diff ms
	unsigned long long deltaCurrent = GetCurrentTime();
	unsigned long long diff = deltaCurrent - delta;
	float deltaMSec = static_cast< float > ( diff / 1000000.0f );

	// Reset delta
	delta = deltaCurrent;

	return deltaMSec;
}

// Elapsed time since last reset ( used for regular updates and elapsed game time )
unsigned int Timer::GetElapsed( bool reset /*= false*/ )
{
	unsigned long long currentTime = GetCurrentTime();
	unsigned long long diff = currentTime - update;

	if ( reset )
		update = currentTime;

	return diff / 1000000;
}

// /Used for things that should be updated regularly
bool Timer::IsUpdateTime()
{
	// Check if there's more than sleepTime milliseconds since update
	bool update = static_cast<int>(  GetElapsed( false ) - sleepTime ) >= 0;

	if ( update )
		ResetPrevTime();

	return update;
}

/*
   int Timer::GetSleepTime()
   {
   unsigned int elapsed = GetElapsed();
   return  sleepTime - elapsed;
   }
   */
void Timer::ResetPrevTime()
{
	update = GetCurrentTime();
}

// Returns current time ( with 0 == program startup )
unsigned long long Timer::GetCurrentTime() const
{
	timespec tmCurrent;
	clock_gettime( CLOCK_MONOTONIC_RAW, &tmCurrent);
	return CreateTimeStamp( tmCurrent );
}
// Convert timespec to a timespac in nanoseconds.
unsigned long long Timer::CreateTimeStamp( const timespec &tm ) const
{
	unsigned long long timeStamp = tm.tv_sec * 1000000000 + tm.tv_nsec;
	return timeStamp; 
}
