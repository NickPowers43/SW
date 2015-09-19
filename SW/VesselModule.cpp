
#include "VesselModule.h"
#include "NetworkWriter.h"


namespace SW
{
	VesselModule::VesselModule(VMFlags_t flags, uint32_t id)
	{
		VesselModule::flags = flags;
		VesselModule::id = id;
	}


	VesselModule::~VesselModule()
	{
	}

	AABBi VesselModule::GetAABB()
	{
		AABBi output = temp->tiles.GetAABB();
		if (flags & VM_FLAG_ROTATED)
		{
			output.tr = glm::ivec2(output.tr.y, output.tr.x);
		}
		return output;
	}
	void VesselModule::WriteSetModuleMessage(NetworkWriter* nw)
	{
		nw->Write(type);
		nw->Write((uint32_t)pos.x);
		nw->Write((uint32_t)pos.y);
	}
}
