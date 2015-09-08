#pragma once
#include "SW.h"

namespace SW
{
	class VesselObject
	{
	public:
		VesselObject(ObjectType::ObjectType type, glm::vec2 pos);
		~VesselObject();

		glm::vec2 pos;
		ObjectType::ObjectType type;
	};
}

