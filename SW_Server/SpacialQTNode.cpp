#include "stdafx.h"
#include "SpacialQTNode.h"


namespace SW_Server
{
	SpacialQTNode::SpacialQTNode(VesselVecType bl, VesselValueType size, QTNode* parent) : QTNode(parent)
	{
		SpacialQTNode::bl = bl;
		SpacialQTNode::size = size;
	}


	SpacialQTNode::~SpacialQTNode()
	{
	}

	const SpacialQTNode SpacialQTNode::bl_quadrant()
	{
		return SpacialQTNode(bl, size * 0.5, static_cast<QTNode*>(this));
	}
	const SpacialQTNode SpacialQTNode::tl_quadrant()
	{
		VesselValueType q_size = size * 0.5;
		return SpacialQTNode(VesselVecType(bl.x, bl.y + q_size), q_size, static_cast<QTNode*>(this));
	}
	const SpacialQTNode SpacialQTNode::br_quadrant()
	{
		VesselValueType q_size = size * 0.5;
		return SpacialQTNode(VesselVecType(bl.x + q_size, bl.y), q_size, static_cast<QTNode*>(this));
	}
	const SpacialQTNode SpacialQTNode::tr_quadrant()
	{
		VesselValueType q_size = size * 0.5;
		return SpacialQTNode(bl + VesselVecType(q_size), q_size, static_cast<QTNode*>(this));
	}

	VesselValueType SpacialQTNode::Left()
	{
		return bl.x;
	}
	VesselValueType SpacialQTNode::Right()
	{
		return bl.x + size;
	}
	VesselValueType SpacialQTNode::Bottom()
	{
		return bl.y;
	}
	VesselValueType SpacialQTNode::Top()
	{
		return bl.y + size;
	}

	VesselValueType SpacialQTNode::MiddleHorizontal()
	{
		return bl.x + (size * 0.5);
	}
	VesselValueType SpacialQTNode::MiddleVertical()
	{
		return bl.y + (size * 0.5);
	}
}
