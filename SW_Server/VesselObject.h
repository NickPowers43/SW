#pragma once


namespace ObjectType
{
	enum ObjectType : uint8_t {
		Spawner = 0,
		Turret = 1
	};
}

namespace SW_Server
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

