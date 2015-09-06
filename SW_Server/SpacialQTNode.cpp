#include "stdafx.h"
#include "SpacialQTNode.h"


namespace SW_Server
{
	SpacialQTNode::SpacialQTNode(glm::vec2 bl, float size)
	{
		SpacialQTNode::bl = bl;
		SpacialQTNode::size = size;
	}


	SpacialQTNode::~SpacialQTNode()
	{
	}

	SpacialQTNode SpacialQTNode::bl_quadrant()
	{
		return SpacialQTNode(bl, size * 0.5f);
	}
	SpacialQTNode SpacialQTNode::tl_quadrant()
	{
		float q_size = size * 0.5f;
		return SpacialQTNode(glm::vec2(bl.x, bl.y + q_size), q_size);
	}
	SpacialQTNode SpacialQTNode::br_quadrant()
	{
		float q_size = size * 0.5f;
		return SpacialQTNode(glm::vec2(bl.x + q_size, bl.y), q_size);
	}
	SpacialQTNode SpacialQTNode::tr_quadrant()
	{
		float q_size = size * 0.5f;
		return SpacialQTNode(bl + glm::vec2(q_size), q_size);
	}
}
