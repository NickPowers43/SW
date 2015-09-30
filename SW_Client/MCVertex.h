#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace SW_Client
{
	class MCVertex
	{
	public:
		MCVertex();
		MCVertex(glm::vec3 pos);
		MCVertex(glm::vec3 pos, glm::vec2 uv);
		MCVertex(glm::vec3 pos, glm::vec3 normal);
		MCVertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal);
		~MCVertex();

		glm::vec3 pos;
		glm::vec2 uv;
		glm::vec3 normal;
	};
}

