#include "TrackedCompartmentTileSet.h"
#include "AABBi.h"
#include "AdjacentTiles.h"
#include "Compartment.h"
#include "CompartmentTile.h"


namespace SW
{
	TrackedCompartmentTileSet::TrackedCompartmentTileSet()
	{
	}


	TrackedCompartmentTileSet::~TrackedCompartmentTileSet()
	{
	}

	void TrackedCompartmentTileSet::SetCompartmentFloor(uint8_t type, Compartment* c)
	{
		AABBi aabb = GetAABB();

		for (int i = aabb.bl.y; i < aabb.tr.y; i++) {
			for (int j = aabb.bl.x; j < aabb.tr.x; j++) {
				glm::ivec2 tileI = glm::ivec2(j, i);
				Tile* tile;
				if ((tile = TryGet(tileI))) {
					bool modified = false;
					if (GetC0(tile) && GetC0(tile)->Instance() == c) {
						//cout << "Changing floor0 type at (" << tileI.x << "," << tileI.y << ")" << std::endl;
						tile->floor0 = type;
						modified = true;
					}
					if (GetC1(tile) && GetC1(tile)->Instance() == c) {
						//cout << "Changing floor1 type at (" << tileI.x << "," << tileI.y << ")" << std::endl;
						tile->floor1 = type;
						modified |= true;
					}

					if (modified) {
						MarkTileAsModified(tileI);
					}
				}
			}
		}
	}
}
