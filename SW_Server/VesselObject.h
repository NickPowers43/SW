#pragma once


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

