#pragma once

#include<ostream>

namespace Math
{

	Vector2f Scale( Vector2f vec, double factor );
	Vector2f FlipAndScale( Vector2f vec, double factor, double height  );

	Vector2f Scale( Vector2f vec, double factor )
	{
		return Vector2f( vec.x * factor, vec.y * factor );
	}


	Vector2f FlipAndScale( Vector2f vec, double factor, double height )
	{
		Vector2f retVec = Scale( vec, factor );
		retVec.y -= height;
		return retVec;
	}
}
