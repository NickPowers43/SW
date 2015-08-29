#pragma once
#include "Compartment.h"

namespace SW_Server
{
	const uint16_t TileFlag_None = 0;
	const uint16_t TileFlag_Hatch0 = 1;
	const uint16_t TileFlag_Hatch1 = 2;
	const uint16_t TileFlag_SolidBlock = 4;
	const uint16_t TileFlag_WallNode = 8;

	const uint8_t WallTypeMask_None = 0;
	const uint8_t WallTypeMask_OneByZero = 1;
	const uint8_t WallTypeMask_TwoByOne = 2;
	const uint8_t WallTypeMask_OneByOne = 4;
	const uint8_t WallTypeMask_OneByTwo = 8;
	const uint8_t WallTypeMask_ZeroByOne = 16;
	const uint8_t WallTypeMask_OneByTwoFlipped = 32;
	const uint8_t WallTypeMask_OneByOneFlipped = 64;
	const uint8_t WallTypeMask_TwoByOneFlipped = 128;

	const uint8_t BlockType_None = 0;
	const uint8_t BlockType_Spawner = 1;
	const uint8_t BlockType_Turret = 2;

	const uint8_t WallType_None = 0;
	const uint8_t WallType_OneByZero = 1;
	const uint8_t WallType_TwoByOne = 2;
	const uint8_t WallType_OneByOne = 3;
	const uint8_t WallType_OneByTwo = 4;
	const uint8_t WallType_ZeroByOne = 5;
	const uint8_t WallType_OneByTwoFlipped = 6;
	const uint8_t WallType_OneByOneFlipped = 7;
	const uint8_t WallType_TwoByOneFlipped = 8;

	const uint8_t FloorType_None = 0;
	const uint8_t FloorType_Basic = 1;
	const uint8_t FloorType_SmoothWhite = 2;

	class VesselTile
	{
	public:
		uint16_t flags;
		uint8_t wallMask;
		uint8_t floor0;
		uint8_t floor1;
		Compartment* c0;
		Compartment* c1;

		VesselTile();
		~VesselTile();

		int WallCount();
		int GetWalls(uint8_t* wall0, uint8_t* wall1);
		bool Contains(uint8_t wall);
		bool ContainsMask(uint8_t wall);
	};
}

