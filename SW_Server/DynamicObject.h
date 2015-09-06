#pragma once
#include "GameObject.h"

namespace SW_Server
{
	class DynamicObject :
		public GameObject
	{
	public:
		DynamicObject(glm::vec2 vel, float m, glm::vec2 pos, float rot);
		~DynamicObject();


		float mass();
		float massInv();
		glm::vec2 velocity();

		void ApplyForce(glm::vec2 dir);

	private:


		glm::vec2 vel;
		float m;



	};

}
