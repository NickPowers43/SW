#pragma once
#include "SW.h"
#include "AABBi.h"
#include "VesselModuleTemplate.h"

#define VM_FLAG_FLIPPED_X 1
#define VM_FLAG_FLIPPED_Y 2
#define VM_FLAG_ROTATED 4

namespace SW
{
	class VesselModule
	{
	public:
		VesselModule(VMFlags_t flags, uint32_t id);
		~VesselModule();

		AABBi GetAABB();
		void WriteSetModuleMessage(NetworkWriter* nw);


		VMFlags_t flags;
		uint32_t id;
		VMType_t type;
		glm::ivec2 pos;
		VesselModuleTemplate* temp;
		int rot;
	};
}

