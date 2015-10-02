
#include "GameObject.h"

namespace SW
{
	GameObject::GameObject(glm::vec3 pos, glm::vec3 rot)
	{
		GameObject::pos = pos;
		GameObject::rot = rot;
	}
	GameObject::~GameObject()
	{

	}

	glm::vec2 GameObject::posXZ()
	{
		return glm::vec2(pos.x, pos.z);
	}
}