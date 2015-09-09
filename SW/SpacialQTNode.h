#pragma once
#include "SW.h"
#include "QTNode.h"


namespace SW
{
	template<typename value_type>
	class SpacialQTNode :
		public QTNode
	{
	protected:
		glm::tvec2<value_type> bl;
		value_type size;

	public:
		SpacialQTNode(glm::tvec2<value_type> bl, value_type size, QTNode* parent) : QTNode(parent)
		{
			SpacialQTNode::bl = bl;
			SpacialQTNode::size = size;
		}
		~SpacialQTNode()
		{
		}

		template<typename value_type>
		const SpacialQTNode bl_quadrant()
		{
			return SpacialQTNode(bl, size * 0.5, static_cast<QTNode*>(this));
		}
		template<typename value_type>
		const SpacialQTNode tl_quadrant()
		{
			value_type q_size = size * 0.5;
			return SpacialQTNode(glm::tvec2<value_type>(bl.x, bl.y + q_size), q_size, static_cast<QTNode*>(this));
		}
		template<typename value_type>
		const SpacialQTNode br_quadrant()
		{
			value_type q_size = size * 0.5;
			return SpacialQTNode(glm::tvec2<value_type>(bl.x + q_size, bl.y), q_size, static_cast<QTNode*>(this));
		}
		template<typename value_type>
		const SpacialQTNode tr_quadrant()
		{
			value_type q_size = size * 0.5;
			return SpacialQTNode(bl + glm::tvec2<value_type>(q_size), q_size, static_cast<QTNode*>(this));
		}

		template<typename value_type>
		value_type Left()
		{
			return bl.x;
		}
		template<typename value_type>
		value_type Right()
		{
			return bl.x + size;
		}
		template<typename value_type>
		value_type Bottom()
		{
			return bl.y;
		}
		template<typename value_type>
		value_type Top()
		{
			return bl.y + size;
		}

		template<typename value_type>
		value_type MiddleHorizontal()
		{
			return bl.x + (size * 0.5);
		}
		template<typename value_type>
		value_type MiddleVertical()
		{
			return bl.y + (size * 0.5);
		}
	};
}

