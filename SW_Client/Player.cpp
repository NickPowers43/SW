#include "Player.h"

namespace SW_Client
{
	Player::Player(glm::vec2 vel, float m, glm::vec2 pos, float rot, glm::ivec2 chunkI) : SW::Player(vel, m, pos, rot, chunkI)
	{
		facing = glm::vec2(0.0f, 0.0f);
	}


	Player::~Player()
	{
	}
}
