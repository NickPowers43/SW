#pragma once
#include "VesselModuleTemplate.h"

namespace SW_Server
{
	class VesselModule
	{
	public:
		VesselModule(uint32_t id);
		~VesselModule();

		void WriteSetModuleMessage(NetworkWriter* nw);

		VMFlags_t flags;
		uint32_t id;
		VMType_t type;
		glm::ivec2 pos;
		VesselModuleTemplate* temp;
		VesselModule* children[4];
		int rot;
	};
}

