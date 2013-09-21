#pragma once

enum class TileType
{
	Regular,
	Explosive,
	Hard,
	Unbreakable,
	Wall_Of_Death // Balls can pass through towards their owner's tile, but not the other way. They have to be hit 3 ( ? ) times before they are destroyed.
};
