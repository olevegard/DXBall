#include "Particle.h"

Particle::Particle( Rect r, SDL_Color  clr )
{
	rect = r;
	color = clr;

	dir.x = RandomHelper::GenRandomNumber( -1.0, 1.0 );
	dir.y = RandomHelper::GenRandomNumber( -1.0, 1.0 );

	SetDecay( 0.0, 1.0 );
	SetSpeed( 0.0, 1.0 );

	isAlive = true;
}
void Particle::SetDecay( double min, double max )
{
	decay = RandomHelper::GenRandomNumber( min, max );
}
void Particle::SetSpeed( double min, double max )
{
	speed =  RandomHelper::GenRandomNumber( min, max );
}
void Particle::Updated( double delta )
{
	if ( !isAlive )
		return;

	rect.x += dir.x *  speed * delta * 365;
	rect.y += dir.y *  speed * delta * 365;

	uint16_t alphaChange = color.a;
	alphaChange -= static_cast< uint16_t > ( 255 *  decay * delta  );

	if ( color.a == 0 || alphaChange > 255 )
		isAlive = false;
	color.a = static_cast< uint8_t> (alphaChange );
}

