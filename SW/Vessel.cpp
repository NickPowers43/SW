
#include "Vessel.h"

#include "Tile.h"
#include "TileChunk.h"
#include "Player.h"
#include "VesselObject.h"
#include "VesselModule.h"
#include "AdjacentTiles.h"


namespace SW
{
	Vessel::Vessel(VesselIndex_t index)
	{
		
		Vessel::index = index;
	}

	Vessel::~Vessel()
	{
	}

}