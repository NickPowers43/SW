
#include "Tile.h"
#include "NetworkWriter.h"
#include "Compartment.h"


namespace SW
{
	Tile::Tile()
	{
		flags = 0;
		wallMask = 0;
		floor0 = FloorType::None;
		floor1 = FloorType::None;
	}


	Tile::~Tile()
	{
	}

	int Tile::WallCount()
	{
		int count = 0;

		for (int i = 0; i < 8; i++) {
			if (((wallMask >> i) & 1) > 0) {
				count++;
			}
		}

		return count;
	}
	int Tile::GetWalls(WallType_t* wall0, WallType_t* wall1)
	{
		*wall0 = WallType::WallType::None;
		*wall1 = WallType::WallType::None;
		bool firstSet = false;

		int count = 0;

		for (int i = 0; i < 8; i++) {
			if (((wallMask >> i) & 1) > 0) {
				count++;
				if (!firstSet) {
					*wall0 = (i + 1);
					firstSet = true;
				}
				else {
					*wall1 = (i + 1);
				}
			}
		}

		return count;
	}
	bool Tile::Contains(WallType_t wall)
	{
		return (wallMask & (1 << (wall - 1))) > 0;
	}
	bool Tile::ContainsMask(WallTypeMask_t wall)
	{
		return (wallMask & wall) > 0;
	}
}

