#pragma once
#include "SW.h"
#include "GameObject.h"

namespace SW
{
	class RigidBody :
		public GameObject
	{
	public:
		RigidBody(glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 com, glm::vec3 rot);
		~RigidBody();

		float mass();
		float massInv();
		glm::vec3 velocity();
		void ApplyForceXZ(glm::vec2 f);
		void ApplyForceXZ(glm::vec2 local, glm::vec2 f);
		void StepXZ();

		glm::vec3 com;
		glm::vec3 vel;
		float angular_vel;
		float m;
	};

}
