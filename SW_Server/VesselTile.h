#pragma once


namespace SW_Server
{
	static uint16_t TileFlag_None = 0;
	static uint16_t TileFlag_Hatch0 = 1;
	static uint16_t TileFlag_Hatch1 = 2;
	static uint16_t TileFlag_SolidBlock = 4;
	static uint16_t TileFlag_WallNode = 8;

	static uint8_t WallTypeMask_None = 0;
	static uint8_t WallTypeMask_OneByZero = 1;
	static uint8_t WallTypeMask_TwoByOne = 2;
	static uint8_t WallTypeMask_OneByOne = 4;
	static uint8_t WallTypeMask_OneByTwo = 8;
	static uint8_t WallTypeMask_ZeroByOne = 16;
	static uint8_t WallTypeMask_OneByTwoFlipped = 32;
	static uint8_t WallTypeMask_OneByOneFlipped = 64;
	static uint8_t WallTypeMask_TwoByOneFlipped = 128;

	static uint8_t BlockType_None = 0;
	static uint8_t BlockType_Spawner = 1;
	static uint8_t BlockType_Turret = 2;

	static uint8_t WallType_None = 0;
	static uint8_t WallType_OneByZero = 1;
	static uint8_t WallType_TwoByOne = 2;
	static uint8_t WallType_OneByOne = 3;
	static uint8_t WallType_OneByTwo = 4;
	static uint8_t WallType_ZeroByOne = 5;
	static uint8_t WallType_OneByTwoFlipped = 6;
	static uint8_t WallType_OneByOneFlipped = 7;
	static uint8_t WallType_TwoByOneFlipped = 8;

	static uint8_t FloorType_None = 0;
	static uint8_t FloorType_Basic = 1;
	static uint8_t FloorType_SmoothWhite = 2;

	class VesselTile
	{
	public:
		uint16_t flags;
		uint8_t wallMask;

		VesselTile();
		~VesselTile();

		int WallCount();
		int GetWalls(uint8_t* wall0, uint8_t* wall1);
		bool Contains(uint8_t wall);
		bool ContainsMask(uint8_t wall);
	};
}

