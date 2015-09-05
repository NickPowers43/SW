#include "stdafx.h"
#include "AABBi.h"


namespace SW_Server
{
	AABBi::AABBi(glm::ivec2 bl, glm::ivec2 tr)
	{
		AABBi::bl = bl;
		AABBi::tr = tr;
	}


	AABBi::~AABBi()
	{
	}

	void AABBi::FitWhole(glm::ivec2 point)
	{
		bl.x = glm::min(point.x++, bl.x);
		bl.y = glm::min(point.y++, bl.y);
		tr.x = glm::max(point.x, tr.x);
		tr.y = glm::max(point.y, tr.y);
	}

	void AABBi::Fit(glm::ivec2 point)
	{
		bl.x = glm::min(point.x, bl.x);
		bl.y = glm::min(point.y, bl.y);
		tr.x = glm::max(point.x, tr.x);
		tr.y = glm::max(point.y, tr.y);
	}

	void AABBi::FitAABB(AABBi & aabb)
	{
		bl.x = glm::min(aabb.bl.x, bl.x);
		bl.y = glm::min(aabb.bl.y, bl.y);

		tr.x = glm::max(aabb.tr.x, tr.x);
		tr.y = glm::max(aabb.tr.y, tr.y);
	}
}
