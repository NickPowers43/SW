
#include "Player.h"
#include "Vessel.h"
#include "NetworkWriter.h"
#include <glm/glm.hpp>
#include <glm/mat2x2.hpp>

namespace SW
{
	Player::Player(PlayerID_t id, glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 rot) :
		RigidBody(vel, m, pos, glm::vec3(0.0f, 0.0f, 0.0f), rot)
	{
		inputs = 0;
		Player::id = id;
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
				f.y = 1.0f;
			}
			if (inputs & INPUT_FLAG_S)
			{
				if (inputs & INPUT_FLAG_W)
				{
					f.y = 0.0f;
				}
				else
				{
					f.y = -1.0f;
				}
			}
			if (inputs & INPUT_FLAG_A)
			{
				f.x = -1.0f;
			}
			if (inputs & INPUT_FLAG_D)
			{
				if (inputs & INPUT_FLAG_A)
				{
					f.x = 0.0f;
				}
				else
				{
					f.x = 1.0f;
				}
			}

			if (f.x != 0.0f || f.y != 0.0f)
			{
				f = glm::normalize(f) * 10000.0f * deltaTime;
				float cos = glm::cos(rot.x);
				float sin = glm::sin(rot.x);
				f = glm::mat2(cos, -sin, sin, cos) * f;

				ApplyForceXZ(f);
				//pos += glm::vec3(translate.x, 0.0f, translate.y);
			}
		}

		StepXZ();
		vel *= 0.9f;
	}
}
