#include "stdafx.h"
#include "WorldQTNode.h"



namespace SW_Server
{
	int* adjRequiredBits = new int[8]{
		(QT_T_BIT | QT_R_BIT),
		(QT_T_BIT),
		(QT_T_BIT | QT_R_BIT),
		(QT_R_BIT),
		(QT_R_BIT),
		(QT_T_BIT | QT_R_BIT),
		(QT_T_BIT),
		(QT_T_BIT | QT_R_BIT)
	};
	int* adjBitValues = new int[8]{
		(QT_BR),
		(~QT_T_BIT),
		(QT_BL),
		(QT_R_BIT),
		(~QT_R_BIT),
		(QT_TR),
		(QT_T_BIT),
		(QT_TL)
	};

	WorldQTNode::VesselIterator::VesselIterator(Vessel** temp)
	{
		vessel = temp;
		dir_ref = *temp;
	}
	WorldQTNode::VesselIterator::VesselIterator(const VesselIterator& org)
	{
		*this = org;
	}
	WorldQTNode::VesselIterator& WorldQTNode::VesselIterator::operator++() //pre-icrement
	{
		vessel = &((dir_ref)->Next());
		dir_ref = *vessel;
		return *this;
	}
	WorldQTNode::VesselIterator WorldQTNode::VesselIterator::operator++(int)
	{
		VesselIterator tmp(*this);
		vessel = &((dir_ref)->Next());
		dir_ref = *vessel;
		return tmp;
	}
	Vessel* WorldQTNode::VesselIterator::operator->() const
	{
		return dir_ref;
	}
	void WorldQTNode::VesselIterator::swap(VesselIterator& other)
	{
		using std::swap;
		swap(vessel, other.vessel);
	}
	Vessel* WorldQTNode::VesselIterator::RemoveCurrent()
	{
		Vessel* curr = dir_ref;
		dir_ref = curr->Next();
		*vessel = dir_ref;
		curr->Next() = NULL;
		return curr;
	}


	WorldQTNode::WorldQTNode(int depth, VesselVecType bl, VesselValueType size, QTNode* parent) : SpacialQTNode(bl, size, parent)
	{
		WorldQTNode::depth = depth;
		flags = 0;
		vessels = NULL;
	}
	WorldQTNode::WorldQTNode(int depth, const SpacialQTNode & org) : SpacialQTNode(org)
	{
		WorldQTNode::depth = depth;
		flags = 0;
		vessels = NULL;
	}


	WorldQTNode::~WorldQTNode()
	{
	}

	void WorldQTNode::Break()
	{
		vesselCount = 0;
		flags |= WQT_FLAG_BROKEN;

		children[QT_BL] = new WorldQTNode(depth + 1, bl_quadrant<VesselValueType>());
		children[QT_TL] = new WorldQTNode(depth + 1, tl_quadrant<VesselValueType>());
		children[QT_BR] = new WorldQTNode(depth + 1, br_quadrant<VesselValueType>());
		children[QT_TR] = new WorldQTNode(depth + 1, tr_quadrant<VesselValueType>());
	}
	void WorldQTNode::UpdateSurrounding(QTNode** adjacent)
	{
		//deliver appropriate vessels to (t, and r)
		VesselIterator itr(&vessels);
		while (itr.dir_ref) {
			if (itr->pos.x > Right<VesselValueType>())
			{
				Vessel* curr = itr.RemoveCurrent();
				if (adjacent[QT_ARG_R])
				{
					static_cast<WorldQTNode*>(adjacent[QT_ARG_R])->AddVessel(curr, true);
				}
				else
				{
					static_cast<WorldQTNode*>(parent)->AddVessel(curr, false);
				}
			}
			if (itr->pos.y > Top<VesselValueType>())
			{
				Vessel* curr = itr.RemoveCurrent();
				if (adjacent[QT_ARG_T])
				{
					static_cast<WorldQTNode*>(adjacent[QT_ARG_T])->AddVessel(curr, true);
				}
				else
				{
					static_cast<WorldQTNode*>(parent)->AddVessel(curr, false);
				}
			}
			if (itr.dir_ref)
				++itr;
			else
				break;
		}

		for (size_t i = 0; i < 8; i++)
		{
			if (adjacent[i])
			{
				WorldQTNode* other = static_cast<WorldQTNode*>(adjacent[i]);
				VesselIterator itr(&vessels);
				while (itr.dir_ref) {
					VesselIterator other_itr(&vessels);
					while (other_itr.dir_ref) {

						//compute the interaction between these two vessels (itr.dir_ref, and other_itr.dir_ref)

						++other_itr;
					}
					++itr;
				}
			}
		}

		//deliver appropriate vessels to (b, and l)
		itr = VesselIterator(&vessels);
		while (itr.dir_ref) {
			if (itr->pos.x > Right<VesselValueType>())
			{
				Vessel* curr = itr.RemoveCurrent();
				if (adjacent[QT_ARG_R])
				{
					static_cast<WorldQTNode*>(adjacent[QT_ARG_R])->AddVessel(curr, true);
				}
				else
				{
					static_cast<WorldQTNode*>(parent)->AddVessel(curr, false);
				}
			}
			if (itr->pos.y > Top<VesselValueType>())
			{
				Vessel* curr = itr.RemoveCurrent();
				if (adjacent[QT_ARG_T])
				{
					static_cast<WorldQTNode*>(adjacent[QT_ARG_T])->AddVessel(curr, true);
				}
				else
				{
					static_cast<WorldQTNode*>(parent)->AddVessel(curr, false);
				}
			}
			if (itr.dir_ref)
				++itr;
			else
				break;
		}

		QTNode::UpdateSurrounding(adjacent);
	}
	void WorldQTNode::AddVessel(Vessel* vessel, bool force)
	{
		if (force || (vessel->pos.x > Left<VesselValueType>() && vessel->pos.x < Right<VesselValueType>() && vessel->pos.y > Bottom<VesselValueType>() && vessel->pos.y < Top<VesselValueType>()))
		{
			if (flags & WQT_FLAG_BROKEN)
			{
				//add to children
				if (vessel->pos.x > MiddleHorizontal<VesselValueType>())
				{
					if (vessel->pos.y > MiddleVertical<VesselValueType>())
					{
						if (!children[QT_TR])
						{
							throw std::exception("child should not be NULL");
						}
						else
						{
							static_cast<WorldQTNode*>(children[QT_TR])->AddVessel(vessel, true);
						}

					}
					else
					{
						if (!children[QT_BR])
						{
							throw std::exception("child should not be NULL");
						}
						else
						{
							static_cast<WorldQTNode*>(children[QT_BR])->AddVessel(vessel, true);
						}

					}
				}
				else
				{
					if (vessel->pos.y > MiddleVertical<VesselValueType>())
					{
						if (!children[QT_TL])
						{
							throw std::exception("child should not be NULL");
						}
						else
						{
							static_cast<WorldQTNode*>(children[QT_TL])->AddVessel(vessel, true);
						}
					}
					else
					{
						if (!children[QT_BL])
						{
							throw std::exception("child should not be NULL");
						}
						else
						{
							static_cast<WorldQTNode*>(children[QT_BL])->AddVessel(vessel, true);
						}
					}
				}
			}
			else
			{
				if (depth > 10)
				{

				}
				//add to self
				if (VesselCount() < MAX_VESSELS_PER_NODE)
				{
					//this vessel
					AppendVessel(vessel);
				}
				else
				{
					Break();
					AddVessel(vessel, true);
				}
			}
		}
		else if (parent)
		{
			static_cast<WorldQTNode*>(parent)->AddVessel(vessel, false);
		}
		else
		{
			//TODO: vessel is outside the boundaries of the quad tree
		}

	}
	size_t WorldQTNode::VesselCount()
	{
		size_t count = 0;
		VesselIterator itr(&vessels);
		while (itr.dir_ref) {
			count++;
			++itr;
		}
		return count;
	}
	Vessel* WorldQTNode::BackVessel()
	{
		if (vessels)
		{
			Vessel* curr = vessels;
			while (curr->Next())
			{
				curr = curr->Next();
			}
			return curr;
		}
		else
		{
			return NULL;
		}
		
	}
	void WorldQTNode::AppendVessel(Vessel* vessel)
	{
		if (vessels)
		{
			BackVessel()->Next() = vessel;
		}
		else
		{
			vessels = vessel;
		}

		vessel->Next() = NULL;
	}
}
