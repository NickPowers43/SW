#include "stdafx.h"
#include "VesselTile.h"


namespace SW_Server
{
	VesselTile::VesselTile()
	{
		flags = 0;
		wallMask = 0;
	}


	VesselTile::~VesselTile()
	{
	}

	int VesselTile::WallCount()
	{
		int count = 0;

		for (int i = 0; i < 8; i++) {
			if (((wallMask >> i) & 1) > 0) {
				count++;
			}
		}

		return count;
	}
	int VesselTile::GetWalls(uint8_t* wall0, uint8_t* wall1)
	{
		*wall0 = WallType_None;
		*wall1 = WallType_None;
		bool firstSet = false;

		int count = 0;

		for (int i = 0; i < 8; i++) {
			if (((wallMask >> i) & 1) > 0) {
				count++;
				if (!firstSet) {
					*wall0 = (uint8_t)(i + 1);
					firstSet = true;
				}
				else {
					*wall1 = (uint8_t)(i + 1);
				}
			}
		}

		return count;
	}
	bool VesselTile::Contains(uint8_t wall)
	{
		return (wallMask & (1 << (wall - 1))) > 0;
	}
	bool VesselTile::ContainsMask(uint8_t wall)
	{
		return (wallMask & wall) > 0;
	}
}

