#pragma once
#include "SW.h"

namespace SW
{
	class Vessel;

	class Player
	{
	public:
		Player(glm::ivec2 chunkI);
		~Player();

		glm::ivec2 chunkI;
	};

}
