#pragma once
#include "SW.h"

namespace SW
{
	class Tile
	{
	public:

		Tile();
		~Tile();

		TileFlag_t flags;
		WallTypeMask_t wallMask;
		FloorType_t floor0;
		FloorType_t floor1;

		int WallCount();
		int GetWalls(uint8_t* wall0, uint8_t* wall1);
		bool Contains(uint8_t wall);
		bool ContainsMask(uint8_t wall);
	};
}

