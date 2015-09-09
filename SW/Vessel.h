#pragma once
#include "TileChunks.h"

namespace SW
{
	static float WALL_RADIUS = 0.15f;

	class AdjacentTiles;

	class Vessel
	{
	public:

		Vessel(VesselIndex_t index);
		~Vessel();

		void IsModuleLegal();

		//virtual void PlaceBlock(uint16_t type, glm::ivec2 location);


	protected:
		VesselIndex_t index;

		std::vector<VesselModule> modules;
		std::vector<VesselObject*> objects;
		std::vector<TileChunk*> modifiedChunks;
	};
}

