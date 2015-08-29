#pragma once

#include "stdafx.h"

namespace SW_Server
{
	class AABBi
	{
	public:
		glm::ivec2 bl;
		glm::ivec2 tr;

		AABBi();
		~AABBi();

		void AABBi::FitWhole(glm::ivec2 point);
		void AABBi::Fit(glm::ivec2 point);
		void AABBi::FitAABB(AABBi & aabb);
	};
}

