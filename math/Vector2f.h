#pragma once

#include<ostream>

struct Vector2f{
	Vector2f( double posX, double posY)
		:   x(0)
		,   y(0)
	{
		x = posX;
		y = posY;

	}
	Vector2f()
		:   x( 0.0f )
		,   y( 0.0f )
	{}

	Vector2f( const Vector2f& vec)
		:   x( vec.x )
		,   y( vec.y )
	{
		x = vec.x;
		y = vec.y;
	}

	double x;
	double y;

	inline void operator+=( const Vector2f &vec )
	{
		x += vec.x;
		y += vec.y;
	}
	inline void operator-=( const Vector2f &vec )
	{
		x -= vec.x;
		y -= vec.y;
	}
	inline void operator/=( const Vector2f &vec )
	{
		x /= vec.x;
		y /= vec.y;
	}
};

inline bool operator==( const Vector2f &vec1, const Vector2f &vec2)
{
	return ( ( vec1.x == vec2.x ) && vec1.y == vec2.y );
}

inline std::ostream& operator<<(std::ostream& stream, const Vector2f &pos)
{
	stream << pos.x << " , " << pos.y;
	return stream;
}

inline bool operator!=( const Vector2f &vec1, const Vector2f &vec2)
{
	return !( vec1 == vec2);
}
inline Vector2f operator+( const Vector2f &point1, const Vector2f &point2 )
{
	Vector2f p( point1.x + point2.x, point1.y + point2.y );
	return p;
}

inline Vector2f operator-( const Vector2f &point1, const Vector2f &point2 )
{
	Vector2f v( point1.x - point2.x, point1.y - point2.y );
	return v;
}

inline Vector2f operator-( const Vector2f &point1)
{
	Vector2f v( -point1.x, -point1.y);
	return v;
}
inline Vector2f operator*( const Vector2f &point1, const Vector2f &point2 )
{
	Vector2f v( point1.x * point2.x, point1.y * point2.y );
	return v;
}
inline Vector2f operator*( float c, const Vector2f &vec)
{
	Vector2f v( vec.x * c, vec.y * c );
	return v;
}
inline Vector2f operator*( const Vector2f &vec, float c)
{
	Vector2f v( vec.x * c, vec.y * c );
	return v;
}
inline Vector2f operator+( float c, const Vector2f &vec)
{
	Vector2f v( vec.x + c, vec.y + c );
	return v;
}

inline Vector2f operator/=( Vector2f vec, float f )
{
	Vector2f v( vec.x /=  f, vec.y /= f );
	//Vector3f v( vec.x /  f, vec.y / f, vec.z / f);
	return v;
}

inline Vector2f operator/( Vector2f vec, float f )
{
	Vector2f v( vec.x /  f, vec.y / f );
	//Vector3f v( vec.x /  f, vec.y / f, vec.z / f);
	return v;
}

namespace Math
{
	double Dot( const Vector2f &vec1, const Vector2f &vec2 );
	double PerpDot( const Vector2f &vec1, const Vector2f &vec2 );
};
