#pragma once
#include "SW.h"
#include "TrackedTileSet.h"
#include "CompartmentTileSet.h"

namespace SW
{
	class TrackedCompartmentTileSet :
		virtual public CompartmentTileSet,
		virtual public TrackedTileSet
	{
	public:
		TrackedCompartmentTileSet();
		~TrackedCompartmentTileSet();

		virtual void SetCompartmentFloor(FloorType_t type, Compartment* c) override;
	};
}

