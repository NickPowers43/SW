
#include "Compartment.h"


namespace SW
{
	Compartment::Compartment(CompartmentIndex_t index) : aabb(glm::ivec2(0, 0), glm::ivec2(0, 0))
	{
		isPtr = false;
		ptrValue = NULL;

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
		if (ptrValue)
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

	void Compartment::Reference(Compartment* other)
	{
		ptrValue = other;

		if (other)
		{
			other->aabb.FitAABB(aabb);
		}
	}

}
