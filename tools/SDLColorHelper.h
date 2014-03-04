#pragma once

#include <SDL2/SDL.h>

inline std::istream& operator>>( std::istream &is, SDL_Color &color )
{
	int32_t r = 0;
	is >> r;
	color.r = static_cast<uint8_t> ( r );

	int32_t g = 0;
	is >> g;
	color.g = static_cast<uint8_t> ( g );

	int32_t b = 0;
	is >> b;
	color.b = static_cast<uint8_t> ( b );

	int32_t a = 0;
	is >> a;
	color.a = static_cast<uint8_t> ( a );

	return is;
}
inline std::ostream& operator<<( std::ostream &os, const SDL_Color &color )
{
	os
		<< static_cast < int32_t> ( color.r ) << ", "
		<< static_cast < int32_t> ( color.g ) << ", "
		<< static_cast < int32_t> ( color.b ) << ", "
		<< static_cast < int32_t> ( color.a );
	return os;
}
