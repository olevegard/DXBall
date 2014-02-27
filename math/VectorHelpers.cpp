#include "VectorHelpers.h"


Vector2f Math::Scale( Vector2f vec, double factor )
{
	return Vector2f( vec.x * factor, vec.y * factor );
}


Vector2f Math::FlipAndScale( Vector2f vec, double factor, double height )
{
	Vector2f retVec = Scale( vec, factor );
	retVec.y -= height;
	return retVec;
}
