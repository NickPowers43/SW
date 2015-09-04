#pragma once

namespace SW_Server
{
	class AABBi
	{
	public:
		glm::ivec2 bl;
		glm::ivec2 tr;

		AABBi(glm::ivec2 bl, glm::ivec2 tr);
		~AABBi();

		void AABBi::FitWhole(glm::ivec2 point);
		void AABBi::Fit(glm::ivec2 point);
		void AABBi::FitAABB(AABBi & aabb);
	};
}

