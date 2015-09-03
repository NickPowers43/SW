#include "stdafx.h"
#include "VesselObject.h"


namespace SW_Server
{
	VesselObject::VesselObject(ObjectType::ObjectType type, glm::vec2 pos)
	{
		VesselObject::pos = pos;
		VesselObject::type = type;
	}


	VesselObject::~VesselObject()
	{
	}
}
