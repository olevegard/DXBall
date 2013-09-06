#include "Vector2f.h"

namespace Math
{
	double Dot( const Vector2f &vec1, const Vector2f &vec2 )
	{
		return ( vec1.x * vec2.x ) + ( vec1.y * vec2.y );
	}

	double PerpDot( const Vector2f &vec1, const Vector2f &vec2 )
	{
		return ( vec1.y * vec2.x ) - ( vec1.x * vec2.y );
	}
};
