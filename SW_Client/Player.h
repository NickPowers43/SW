#pragma once
#include <SW/Player.h>

namespace SW_Client
{
	class Player :
		public SW::Player
	{
	public:
		Player(glm::vec2 vel, float m, glm::vec2 pos, float rot, glm::ivec2 chunkI);
		~Player();
	};
}

