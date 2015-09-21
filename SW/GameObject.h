#pragma once
#include "SW.h"

namespace SW
{
	template<typename value_type>
	class GameObject
	{
	public:
		GameObject(glm::tvec2<value_type> pos, value_type rot)
		{
			GameObject::pos = pos;
			GameObject::rot = rot;
		}
		~GameObject()
		{

		}

		glm::tvec2<value_type> pos;
		value_type rot;
	};

}
