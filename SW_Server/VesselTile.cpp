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
	int VesselTile::GetWalls(WallType::WallType* wall0, WallType::WallType* wall1)
	{
		*wall0 = WallType::WallType::None;
		*wall1 = WallType::WallType::None;
		bool firstSet = false;

		int count = 0;

		for (int i = 0; i < 8; i++) {
			if (((wallMask >> i) & 1) > 0) {
				count++;
				if (!firstSet) {
					*wall0 = (WallType::WallType)(i + 1);
					firstSet = true;
				}
				else {
					*wall1 = (WallType::WallType)(i + 1);
				}
			}
		}

		return count;
	}
	bool VesselTile::Contains(WallType::WallType wall)
	{
		return (wallMask & (1 << (wall - 1))) > 0;
	}
	bool VesselTile::Contains(WallTypeMask::WallTypeMask wall)
	{
		return (wallMask & wall) > 0;
	}
}

