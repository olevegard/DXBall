#include "Particle.h"

Particle::Particle( Rect r, SDL_Texture* text )
{
	rect = r;
	texture = text;
	dir.x = RandomHelper::GenRandomNumber( -1.0, 1.0 );
	dir.y = RandomHelper::GenRandomNumber( -1.0, 1.0 );
	decay = RandomHelper::GenRandomNumber( 2.0, 4.0 );
	speed = static_cast< int32_t > ( RandomHelper::GenRandomNumber( 0.2, 10.0 ) * 367 );
	alpha = 254;
	isAlive = true;
}

void Particle::Updated( double delta )
{
	if ( !isAlive )
		return;

	rect.x += dir.x *  speed * delta;
	rect.y += dir.y *  speed * delta;

	uint16_t alphaChange = alpha;
	alphaChange -= static_cast< uint16_t > ( 255 * delta * decay  );

	if ( alpha == 0 || alphaChange > 255 )
		isAlive = false;
	alpha = static_cast< uint8_t> (alphaChange );
}

