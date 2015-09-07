#pragma once
#include "QTNode.h"


namespace SW_Server
{
	class SpacialQTNode :
		public QTNode
	{
	public:
		SpacialQTNode(VesselVecType bl, VesselValueType size, QTNode* parent);
		~SpacialQTNode();

		const SpacialQTNode bl_quadrant();
		const SpacialQTNode tl_quadrant();
		const SpacialQTNode br_quadrant();
		const SpacialQTNode tr_quadrant();

		VesselValueType Left();
		VesselValueType Right();
		VesselValueType Bottom();
		VesselValueType Top();

		VesselValueType MiddleHorizontal();
		VesselValueType MiddleVertical();

		VesselVecType bl;
		VesselValueType size;
	};
}

