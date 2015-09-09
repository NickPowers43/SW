#pragma once
#include "SW.h"
#include "VesselModuleTileSet.h"

namespace SW
{
	class VesselModuleTemplate
	{
	public:
		VesselModuleTemplate(glm::ivec2 dim);
		~VesselModuleTemplate();

		VesselModuleTileSet tiles;
	};

}
