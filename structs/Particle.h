#pragma once

#include <SDL2/SDL.h>
#include "../math/Rect.h"
#include "../math/Vector2f.h"
#include "../math/Math.h"



struct Particle
{
	Particle( Rect r, SDL_Texture* text );

	void Updated( double delta );
	Particle() = delete;

	Rect rect;
	SDL_Texture* texture;
	Vector2f dir;
	double decay;
	int32_t speed;
	uint8_t alpha;
	bool isAlive;
};

