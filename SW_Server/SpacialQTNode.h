#pragma once
#include "QTNode.h"


namespace SW_Server
{
	class SpacialQTNode :
		public QTNode
	{
	public:
		SpacialQTNode(glm::vec2 bl, float size);
		~SpacialQTNode();

		SpacialQTNode bl_quadrant();
		SpacialQTNode tl_quadrant();
		SpacialQTNode br_quadrant();
		SpacialQTNode tr_quadrant();

		glm::vec2 bl;
		float size;
	};
}

