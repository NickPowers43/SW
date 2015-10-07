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
		ObjectType_t object;
		FloorType_t floor0;
		FloorType_t floor1;

		int WallCount();
		int GetWalls(WallType_t* wall0, WallType_t* wall1);
		bool Contains(WallType_t wall);
		bool ContainsMask(WallTypeMask_t wall);
	};
}

