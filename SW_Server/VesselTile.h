#pragma once
#include "Compartment.h"

namespace TileFlag
{
	enum TileFlag : uint16_t {
		None = 0,
		Hatch0 = 1,
		Hatch1 = 2,
		SolidBlock = 4,
		WallNode = 8
	};
}

namespace WallTypeMask
{
	enum WallTypeMask : uint8_t {
		None = 0,
		OneByZero = 1,
		TwoByOne = 2,
		OneByOne = 4,
		OneByTwo = 8,
		ZeroByOne = 16,
		OneByTwoFlipped = 32,
		OneByOneFlipped = 64,
		TwoByOneFlipped = 128
	};
}

namespace WallType
{
	enum WallType : uint8_t {
		None = 0,
		OneByZero = 1,
		TwoByOne = 2,
		OneByOne = 3,
		OneByTwo = 4,
		ZeroByOne = 5,
		OneByTwoFlipped = 6,
		OneByOneFlipped = 7,
		TwoByOneFlipped = 8
	};
}

namespace FloorType
{
	enum FloorType : uint8_t {
		None = 0,
		Basic = 1,
		SmoothWhite = 2
	};
}

namespace SW_Server
{
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
		int GetWalls(WallType::WallType* wall0, WallType::WallType* wall1);
		bool Contains(WallType::WallType wall);
		bool Contains(WallTypeMask::WallTypeMask wall);
	};
}

