#pragma once

#include <SDL2/SDL.h>
#include "../math/Rect.h"
#include "../math/Vector2f.h"
#include "../math/Math.h"



struct Particle
{
	Particle( Rect r, SDL_Color  clr );


	void Updated( double delta );
	Particle() = delete;

	Rect rect;
	SDL_Color color;
	Vector2f dir;
	double decay;
	int32_t speed;
	bool isAlive;
};

