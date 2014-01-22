#pragma once

struct timespec;
class Timer
{
public:
	Timer();

	// Restars everything
	void Restart();

	// Time since last frame ( for framerate calculation )
	double GetDelta( );

	// Elapsed time since last reset ( used for regular updates and elapsed game time )
	unsigned int GetElapsed( bool reset = false );

	// /Used for things that should be updated regularly
	bool IsUpdateTime();
private:
	void ResetPrevTime();

	// Returns current time ( with 0 == program startup )
	unsigned long long GetCurrentTimeMicroS() const;

#if defined(_WIN32)
	unsigned long long CreateTimeStamp(  unsigned short sec,  unsigned short msec ) const;
#else
	// Convert timespec to a timespac in nanoseconds.
	unsigned long long CreateTimeStamp( const timespec &tm ) const;
#endif

	unsigned long long start;
	unsigned long long delta;
	unsigned long long update;

	unsigned int sleepTime;

	double prevDelta;
};
