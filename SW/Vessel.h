#pragma once
#include "TileChunks.h"
#include "VesselModule.h"
#include "AABBi.h"
#include <vector>

namespace SW
{
	static float WALL_RADIUS = 0.15f;

	class Vessel
	{
	public:

		Vessel(VesselIndex_t index);
		~Vessel();

		int ModulesOverlapping(AABBi aabb);
		int ModulesOverlapping(glm::ivec2 point);
		bool IsModuleLegal(VesselModule* vm);
		bool IsModuleLegal(VMType_t type, glm::ivec2 position);
		bool IsModuleLegal(VesselModuleTemplate* tLate, glm::ivec2 position);

		void AddModule(VesselModule vm);
		void RemoveModuleAt(size_t index);

		//virtual void PlaceBlock(uint16_t type, glm::ivec2 location);

		std::vector<VesselModule> modules;

		VesselIndex_t index;
	};
}

