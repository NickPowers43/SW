#include "MCVertex.h"


namespace SW_Client
{
	MCVertex::MCVertex()
	{

	}
	MCVertex::MCVertex(glm::vec3 pos)
	{
		MCVertex::pos = pos;
	}
	MCVertex::MCVertex(glm::vec3 pos, glm::vec2 uv)
	{
		MCVertex::pos = pos;
		MCVertex::uv = uv;
	}
	MCVertex::MCVertex(glm::vec3 pos, glm::vec3 normal)
	{
		MCVertex::pos = pos;
		MCVertex::normal = normal;
	}
	MCVertex::MCVertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal)
	{
		MCVertex::pos = pos;
		MCVertex::uv = uv;
		MCVertex::normal = normal;
	}

	MCVertex::~MCVertex()
	{
	}
}