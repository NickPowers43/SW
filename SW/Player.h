#pragma once
#include "SW.h"
#include "RigidBody.h"

namespace SW
{
	class Player :
		public RigidBody
	{
	public:
		Player(glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 rot);
		~Player();

		InputFlags_t inputs;

		void Step();
	};

}
