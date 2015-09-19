#pragma once
#include "SW.h"
#include "AABBi.h"
#include "VesselModuleTileSet.h"

namespace SW
{
	class VesselModuleTemplate
	{
	public:
		VesselModuleTemplate(glm::ivec2 dim);
		~VesselModuleTemplate();

		AABBi GetAABB();

		VesselModuleTileSet tiles;
	};

}
