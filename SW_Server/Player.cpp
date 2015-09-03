#include "stdafx.h"
#include "Player.h"
#include "Vessel.h"


namespace SW_Server
{
	Player::Player()
	{
		Player::currentVessel = NULL;
		Player::pos = glm::vec2(0.0f, 0.0f);
		Player::chunkI = glm::ivec2(0, 0);
		Player::rot = 0.0f;
	}


	Player::~Player()
	{
	}
}
