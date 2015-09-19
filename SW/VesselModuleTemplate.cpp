
#include "VesselModuleTemplate.h"
#include "Tile.h"

namespace SW
{
	VesselModuleTemplate::VesselModuleTemplate(glm::ivec2 dim) : tiles(dim)
	{

	}


	VesselModuleTemplate::~VesselModuleTemplate()
	{
	}

	AABBi VesselModuleTemplate::GetAABB()
	{
		AABBi output = tiles.GetAABB();
		output.tr -= glm::ivec2(1, 1);
		return output;
	}
}
