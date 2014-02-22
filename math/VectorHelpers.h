#pragma once

#include<ostream>

namespace Math
{

	Vector2f Scale( Vector2f vec, double factor );
	Vector2f Scale( Vector2f vec, double factor )
	{
		return Vector2f( vec.x * factor, vec.y * factor );
	}

}
