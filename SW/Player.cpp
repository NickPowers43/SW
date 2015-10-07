
#include "Player.h"
#include "Vessel.h"
#include "NetworkWriter.h"
#include <glm/glm.hpp>
#include <glm/mat2x2.hpp>


namespace SW
{
	Player::Player(glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 rot) :
		RigidBody(vel, m, pos, glm::vec3(0.0f, 0.0f, 0.0f), rot)
	{
	}


	Player::~Player()
	{
	}

	void Player::Step()
	{

		if (inputs & (INPUT_FLAG_W | INPUT_FLAG_A | INPUT_FLAG_S | INPUT_FLAG_D))
		{
			glm::vec2 f(0.0f, 0.0f);
			if (inputs & INPUT_FLAG_W)
			{
				f += glm::vec2(0.0f, 1.0f);
			}
			if (inputs & INPUT_FLAG_S)
			{
				f += glm::vec2(0.0f, -1.0f);
			}
			if (inputs & INPUT_FLAG_A)
			{
				f += glm::vec2(-1.0f, 0.0f);
			}
			if (inputs & INPUT_FLAG_D)
			{
				f += glm::vec2(1.0f, 0.0f);
			}

			f = glm::normalize(f) * deltaTime;
			float cos = glm::cos(rot.x);
			float sin = glm::sin(rot.x);
			f = glm::mat2(cos, -sin, sin, cos) * f;

			ApplyForceXZ(f);
			//pos += glm::vec3(translate.x, 0.0f, translate.y);
		}

		StepXZ();
		vel *= 0.5f;
	}
}
