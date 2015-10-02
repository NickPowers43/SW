
#include "Player.h"
#include "Vessel.h"
#include "NetworkWriter.h"


namespace SW
{
	Player::Player(glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 rot) :
		RigidBody(vel, m, pos, glm::vec3(0.0f, 0.0f, 0.0f), rot)
	{
	}


	Player::~Player()
	{
	}
}
