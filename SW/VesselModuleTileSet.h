#pragma once
#include "FixedTileSet.h"
#include "CompartmentTileSet.h"

namespace SW
{
	class VesselModuleTileSet :
		virtual public FixedTileSet,
		virtual public CompartmentTileSet
	{
	public:
		VesselModuleTileSet(glm::ivec2 dim);
		~VesselModuleTileSet();
	};

}
