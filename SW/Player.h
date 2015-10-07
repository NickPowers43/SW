#pragma once
#include "SW.h"
#include "RigidBody.h"

namespace SW
{
	class Player :
		public RigidBody
	{
	public:
		Player(PlayerID_t id, glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 rot);
		~Player();

		PlayerID_t id;
		InputFlags_t inputs;

		void Step();
	};

}
