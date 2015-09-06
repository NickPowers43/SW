#include "stdafx.h"
#include "DynamicObject.h"


namespace SW_Server
{
	DynamicObject::DynamicObject(glm::vec2 vel, float m, glm::vec2 pos, float rot) : GameObject(pos, rot)
	{
		DynamicObject::vel = vel;
		DynamicObject::m = m;
	}


	DynamicObject::~DynamicObject()
	{
	}

	float DynamicObject::mass()
	{
		return m;
	}
	float DynamicObject::massInv()
	{
		return m / 1.0f;
	}
	glm::vec2 DynamicObject::velocity()
	{
		return vel;
	}

	void DynamicObject::ApplyForce(glm::vec2 dir)
	{

	}
}
