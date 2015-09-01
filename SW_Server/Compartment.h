#pragma once

#include "stdafx.h"
#include "AABBi.h"

namespace SW_Server
{
	class Compartment
	{
	public:

		uint32_t index;
		AABBi aabb;
		float area;
		float pressure;
		float oxygen;

		Compartment(uint32_t index);
		~Compartment();

		Compartment* Instance();
		void FitTile(glm::ivec2 index);
		void Combine(Compartment* other);

	private:
		bool isPtr;
		Compartment* ptrValue;
	};
}
