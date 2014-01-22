#include "Timer.h"

#if defined(_WIN32)
#include <Windows.h>
#else
#include <time.h>
#endif
#include <cstdint>
#include <iostream>

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
	start = GetCurrentTimeMicroS();

	// Initilize timestamp to the same as start
	delta = start;
	update = start;
}
// Time since last frame ( for framerate calculation )
double Timer::GetDelta( )
{
	// Get diff ms
	unsigned long long timeCurrent = GetCurrentTimeMicroS();
	unsigned long long diff = timeCurrent - delta;
	double  deltaMSec = static_cast< double > ( diff / 1000000.0f );

	if ( deltaMSec < 0.0 || deltaMSec > 0.99 )
	{
		deltaMSec = prevDelta;
	}
	else
		prevDelta = deltaMSec;

	// Reset delta
	delta = timeCurrent;

	return deltaMSec;
}
// Elapsed time since last reset ( used for regular updates and elapsed game time )
unsigned int Timer::GetElapsed( bool reset /*= false*/ )
{
	unsigned long long currentTime = GetCurrentTimeMicroS();
	unsigned long long diff = currentTime - update;

	if ( reset )
		update = currentTime;

	return static_cast< unsigned int > ( diff / 1000000);
}

// /Used for things that should be updated regularly
bool Timer::IsUpdateTime()
{
	// Check if there's more than sleepTime milliseconds since update
	bool isUpdateTime = static_cast<int>(  GetElapsed( false ) - sleepTime ) >= 0;

	if ( isUpdateTime )
		ResetPrevTime();

	return isUpdateTime;
}
void Timer::ResetPrevTime()
{
	update = GetCurrentTimeMicroS();
}
// Returns current time ( with 0 == program startup )
unsigned long long Timer::GetCurrentTimeMicroS() const
{
#if defined(_WIN32)
	SYSTEMTIME st;
	GetSystemTime(&st);
	return CreateTimeStamp( st.wSecond, st.wMilliseconds );
#else
	timespec tmCurrent;
	clock_gettime( CLOCK_MONOTONIC_RAW, &tmCurrent);

	return CreateTimeStamp( tmCurrent );
#endif
}
// Convert timespec to a timespac in nanoseconds.
#if defined(_WIN32)
unsigned long long Timer::CreateTimeStamp(  unsigned short sec, unsigned short msec ) const
{
	// Get time in µsec ( microseconds )
	unsigned long long timeStamp = static_cast< unsigned long long > ( sec * 1000000 + msec * 1000  );

	return timeStamp;
}
#else
unsigned long long Timer::CreateTimeStamp( const timespec &tm ) const
{
	// Get time in nsec ( nanoseconds )
	unsigned long long timeStamp = static_cast< unsigned long long > ( tm.tv_sec * 1000000000 + tm.tv_nsec );

	// Convert to  µsec ( microseconds )
	timeStamp /= 1000;

	return timeStamp;
}
#endif