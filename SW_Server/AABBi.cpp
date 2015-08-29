#include "AABBi.h"
#include <glm\common.hpp>


namespace SW_Server
{
	AABBi::AABBi()
	{
	}


	AABBi::~AABBi()
	{
	}

	void AABBi::FitWhole(glm::ivec2 point)
	{
		if (point.x < bl.x){
			bl.x = point.x;
		}
		if (point.y < bl.y){
			bl.y = point.y;
		}

		if (point.x >= tr.x){
			tr.x = point.x + 1;
		}
		if (point.y >= tr.y){
			tr.y = point.y + 1;
		}
	}

	void AABBi::Fit(glm::ivec2 point)
	{
		if (point.x < bl.x){
			bl.x = point.x;
		}
		if (point.y < bl.y){
			bl.y = point.y;
		}

		if (point.x > tr.x){
			tr.x = point.x;
		}
		if (point.y > tr.y){
			tr.y = point.y;
		}
	}

	void AABBi::FitAABB(AABBi & aabb)
	{
		bl.x = glm::min(aabb.bl.x, bl.x);
		bl.y = glm::min(aabb.bl.y, bl.y);

		tr.x = glm::max(aabb.tr.x, tr.x);
		tr.y = glm::max(aabb.tr.y, tr.y);
	}
}
