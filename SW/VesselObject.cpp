
#include "VesselObject.h"


namespace SW
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
