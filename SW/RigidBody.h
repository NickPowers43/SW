#pragma once
#include "SW.h"
#include "GameObject.h"

namespace SW
{
	template<typename value_type>
	class RigidBody :
		public GameObject<value_type>
	{
	public:
		template<typename value_type>
		RigidBody(glm::tvec2<value_type> vel, float m, glm::vec2 pos, float rot) : GameObject<value_type>(pos, rot)
		{
			RigidBody::vel = vel;
			RigidBody::m = m;
			RigidBody::com = glm::vec2(0.0f, 0.0f);
		}
		~RigidBody()
		{

		}

		template<typename value_type>
		value_type mass()
		{
			return m;
		}
		template<typename value_type>
		value_type massInv()
		{
			return m / 1.0;
		}
		template<typename value_type>
		glm::tvec2<value_type> velocity()
		{
			return vel;
		}

		template<typename value_type>
		void ApplyForce(glm::vec2 dir)
		{

		}

	private:

		glm::tvec2<value_type> com;
		glm::tvec2<value_type> vel;
		value_type m;
	};

}
