#include "VesselModuleTileSet.h"


namespace SW
{
	VesselModuleTileSet::VesselModuleTileSet(glm::ivec2 dim) : FixedTileSet(dim + glm::ivec2(1, 1))
	{
	}


	VesselModuleTileSet::~VesselModuleTileSet()
	{
	}
}
