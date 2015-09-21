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

		static bool OverlappingInclusive(AABBi a, AABBi b);
		static bool OverlappingExclusive(AABBi a, AABBi b);
		bool ContainsExclusive(glm::ivec2 point);
		bool ContainsInclusive(glm::ivec2 point);
		void FitWhole(glm::ivec2 point);
		void Fit(glm::ivec2 point);
		void FitAABB(AABBi & aabb);
	};
}

