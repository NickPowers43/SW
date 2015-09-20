#pragma once
#include "SW.h"
#include "AABBi.h"

namespace SW
{
	class Compartment
	{
	public:

		CompartmentIndex_t index;
		AABBi aabb;
		float area;
		float pressure;
		float oxygen;

		Compartment(CompartmentIndex_t index);
		~Compartment();

		Compartment* Instance();
		void FitTile(glm::ivec2 index);
		void Reference(Compartment* other);

	private:
		bool isPtr;
		Compartment* ptrValue;
	};
}

