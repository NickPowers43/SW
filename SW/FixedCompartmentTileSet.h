#pragma once
#include "FixedTileSet.h"
#include "CompartmentTileSet.h"

namespace SW
{
	class FixedCompartmentTileSet :
		virtual public FixedTileSet, 
		virtual public CompartmentTileSet
	{
	public:
		FixedCompartmentTileSet(glm::ivec2 dim);
		~FixedCompartmentTileSet();
	};


}