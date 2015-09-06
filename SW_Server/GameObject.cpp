#include "stdafx.h"
#include "GameObject.h"


namespace SW_Server
{
	GameObject::GameObject(glm::vec2 pos, float rot)
	{
		GameObject::pos = pos;
		GameObject::rot = rot;
	}


	GameObject::~GameObject()
	{
	}

}