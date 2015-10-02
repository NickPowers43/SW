#pragma once
#include <SW/SpacialQTNode.h>
#include "Vessel.h"

namespace SW_Server
{
	extern int* adjRequiredBits;
	extern int* adjBitValues;

	static int WQT_FLAG_BROKEN = 1;

	class WorldQTNode :
		public SW::SpacialQTNode<float>
	{

	public:

		class VesselIterator
		{
		public:
			Vessel** vessel;
			Vessel* dir_ref;

			VesselIterator(Vessel** temp);
			VesselIterator(const VesselIterator& org);
			VesselIterator& operator++();
			VesselIterator operator++(int);
			Vessel* operator->() const;
			void swap(VesselIterator& other);
			Vessel* RemoveCurrent();
		};

		WorldQTNode(int depth, glm::vec2 bl, float size, QTNode* parent);
		WorldQTNode(int depth, const SpacialQTNode & org);
		~WorldQTNode();

		void Break();

		virtual void StepSurrounding(QTNode** adjacent) override;
		void AddVessel(Vessel* vessel, bool force);
		size_t VesselCount();
		Vessel* BackVessel();
		void AppendVessel(Vessel* vessel);

		int depth;
		Vessel* vessels;
		int vesselCount;
		uint8_t flags;


	};
}

