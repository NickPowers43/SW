#pragma once
#include <glm/vec3.hpp>

namespace SW_Client
{
	class MCPlane
	{
		glm::vec3 o;
		glm::vec3 n;

		MCPlane(glm::vec3 o, glm::vec3 n);
		~MCPlane();

		static glm::vec3 Intersection(MCPlane & a, MCPlane & b, MCPlane & c);
	};

}
