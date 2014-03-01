#include "Particle.h"

Particle::Particle( Rect r, SDL_Color  clr )
{
	rect = r;
	color = clr;
	dir.x = RandomHelper::GenRandomNumber( -1.0, 1.0 );
	dir.y = RandomHelper::GenRandomNumber( -1.0, 1.0 );
	//decay = RandomHelper::GenRandomNumber( 8.0, 12.0 );
	decay = RandomHelper::GenRandomNumber( 1.0, 2.0 );
	speed = static_cast< int32_t > ( RandomHelper::GenRandomNumber( 0.2, 10.0 ) * 367 );
	//speed = static_cast< int32_t > ( RandomHelper::GenRandomNumber( 0.2, 10.0 ) * 10 );
	isAlive = true;
}

void Particle::Updated( double delta )
{
	if ( !isAlive )
		return;

	rect.x += dir.x *  speed * delta;
	rect.y += dir.y *  speed * delta;

	uint16_t alphaChange = color.a;
	alphaChange -= static_cast< uint16_t > ( 255 *  decay * delta  );

	if ( color.a == 0 || alphaChange > 255 )
		isAlive = false;
	color.a = static_cast< uint8_t> (alphaChange );
}

