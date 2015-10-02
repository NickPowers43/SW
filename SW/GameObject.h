#pragma once
#include "SW.h"

namespace SW
{
	class GameObject
	{
	public:
		GameObject(glm::vec3 pos, glm::vec3 rot);
		~GameObject();

		glm::vec2 posXZ();

		glm::vec3 pos;
		glm::vec3 rot;
	};

}
