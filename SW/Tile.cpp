
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
		*wall0 = WallType::None;
		*wall1 = WallType::None;

		int count = 0;

		for (int i = 1; i < 9; i++) {
			if (Contains(i)) {
				if (count < 1) {
					*wall0 = i;
				}
				else {
					*wall1 = i;
					return 2;
				}
				count++;
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

