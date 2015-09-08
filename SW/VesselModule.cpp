
#include "VesselModule.h"
#include "NetworkWriter.h"


namespace SW
{
	VesselModule::VesselModule(uint32_t id)
	{
		VesselModule::id = id;
	}


	VesselModule::~VesselModule()
	{
	}

	void VesselModule::WriteSetModuleMessage(NetworkWriter* nw)
	{
		nw->Write(type);
		nw->Write((uint32_t)pos.x);
		nw->Write((uint32_t)pos.y);
	}
}
