#pragma once
#include "SW.h"
#include "RigidBody.h"

namespace SW
{
	class Player :
		public RigidBody<float>
	{
	public:
		Player(glm::vec2 vel, float m, glm::vec2 pos, float rot, glm::ivec2 chunkI);
		~Player();

		glm::ivec2 chunkI;
	};

}
