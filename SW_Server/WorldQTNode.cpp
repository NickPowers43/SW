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


	WorldQTNode::WorldQTNode(int depth, glm::vec2 bl, float size, QTNode* parent) : SpacialQTNode(bl, size, parent)
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

		children[QT_BL] = new WorldQTNode(depth + 1, bl_quadrant<float>());
		children[QT_TL] = new WorldQTNode(depth + 1, tl_quadrant<float>());
		children[QT_BR] = new WorldQTNode(depth + 1, br_quadrant<float>());
		children[QT_TR] = new WorldQTNode(depth + 1, tr_quadrant<float>());
	}
	void WorldQTNode::StepSurrounding(QTNode** adjacent)
	{
		myServer->poll();

		//deliver appropriate vessels to (t, and r)
		VesselIterator itr(&vessels);
		while (itr.dir_ref) {
			if (itr->pos.x > Right<float>())
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
			if (itr->pos.z > Top<float>())
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

		ll_for_each(vessels, [&](Vessel* vessel) {

			vessel->Step(nw_main);

			ll_for_each(vessels, [&](Vessel* otherVessel) {
				if (vessel != otherVessel)
				{
					vessel->pos *= 1.0f;
					//compute the interaction between these two vessels
				}
			});
			for (size_t i = 0; i < 8; i++)
			{
				if (adjacent[i])
				{
					WorldQTNode* other = static_cast<WorldQTNode*>(adjacent[i]);
					ll_for_each(other->vessels, [&](Vessel* otherVessel) {
						if (vessel != otherVessel)
						{
							vessel->pos *= 1.0f;
							//compute the interaction between these two vessels
						}
					});
				}
			}
		});

		//deliver appropriate vessels to (b, and l)
		/*ll_for_each(&vessels, [&](Vessel** vPtr, Vessel* vessel) {
			if (vessel->pos.x > Right<float>())
			{
				if (adjacent[QT_ARG_R])
				{
					static_cast<WorldQTNode*>(adjacent[QT_ARG_R])->AddVessel(vessel, true);
				}
				else
				{
					static_cast<WorldQTNode*>(parent)->AddVessel(vessel, false);
				}
				*vPtr = NULL;
			}
			if (vessel->pos.z > Top<float>())
			{
				if (adjacent[QT_ARG_T])
				{
					static_cast<WorldQTNode*>(adjacent[QT_ARG_T])->AddVessel(vessel, true);
				}
				else
				{
					static_cast<WorldQTNode*>(parent)->AddVessel(vessel, false);
				}
				*vPtr = NULL;
			}
		});*/

		itr = VesselIterator(&vessels);
		while (itr.dir_ref) {
			if (itr->pos.x > Right<float>())
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
			if (itr->pos.z > Top<float>())
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

		QTNode::StepSurrounding(adjacent);
	}
	void WorldQTNode::AddVessel(Vessel* vessel, bool force)
	{
		if (force || (vessel->pos.x > Left<float>() && vessel->pos.x < Right<float>() && vessel->pos.z > Bottom<float>() && vessel->pos.z < Top<float>()))
		{
			if (flags & WQT_FLAG_BROKEN)
			{
				//add to children
				if (vessel->pos.x > MiddleHorizontal<float>())
				{
					if (vessel->pos.z > MiddleVertical<float>())
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
					if (vessel->pos.z > MiddleVertical<float>())
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
