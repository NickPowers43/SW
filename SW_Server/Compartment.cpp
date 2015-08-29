#include "stdafx.h"
#include "Compartment.h"


namespace SW_Server
{
	Compartment::Compartment(uint32_t index)
	{
		isPtr = false;
		ptrValue = NULL;

		aabb = AABBi();
		Compartment::index = index;
		area = 0.0f;
		oxygen = 0.0f;
		pressure = 0.0f;
	}


	Compartment::~Compartment()
	{
	}

	Compartment* Compartment::Instance()
	{
		if (isPtr)
		{
			return ptrValue;
		} 
		else
		{
			return this;
		}
	}

	void Compartment::FitTile(glm::ivec2 index)
	{
		aabb.FitWhole(index);
	}

	void Compartment::Combine(Compartment* other)
	{
		if (other != this)
		{
			isPtr = true;
			ptrValue = other->Instance();

			other->aabb.FitAABB(aabb);
		}
	}

}
