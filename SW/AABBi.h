#pragma once
#include "SW.h"

namespace SW
{
	class AABBi
	{
	public:
		glm::ivec2 bl;
		glm::ivec2 tr;

		AABBi(glm::ivec2 bl, glm::ivec2 tr);
		~AABBi();

		void FitWhole(glm::ivec2 point);
		void Fit(glm::ivec2 point);
		void FitAABB(AABBi & aabb);
	};
}

