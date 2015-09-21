
#include "Player.h"
#include "Vessel.h"
#include "NetworkWriter.h"


namespace SW
{
	Player::Player(glm::vec2 vel, float m, glm::vec2 pos, float rot, glm::ivec2 chunkI) :
		RigidBody<float>(vel, m, pos, rot),
		chunkI(chunkI)
	{
	}


	Player::~Player()
	{
	}
}
