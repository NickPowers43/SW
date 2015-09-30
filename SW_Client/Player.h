#pragma once
#include <SW/Player.h>
#include "SW_Client.h"

#define PLAYER_EYE_HEIGHT 

namespace SW_Client
{
	class Player :
		public SW::Player
	{
	public:
		Player(glm::vec2 vel, float m, glm::vec2 pos, float rot, glm::ivec2 chunkI);
		~Player();

		glm::vec2 facing;
	};
}

