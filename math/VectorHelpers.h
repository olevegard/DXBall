#pragma once

#include<ostream>

inline std::ostream& operator<<(std::ostream& stream, const Vector2f &pos)
{
	stream << pos.x << " , " << pos.y;
	return stream;
}
