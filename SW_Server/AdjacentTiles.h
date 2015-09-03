#pragma once

#include "Vessel.h"

namespace SW_Server
{
	class AdjacentTiles
	{
	public:
		AdjacentTiles(Vessel* vessel);
		AdjacentTiles(Vessel* vessel, glm::ivec2 start);
		~AdjacentTiles();

		void Reset(glm::ivec2 pos);
		void MoveRight();

		VesselTile* l2Tile;
		VesselTile* lTile;
		VesselTile* blTile;
		VesselTile* bTile;
		VesselTile* b2Tile;
		VesselTile* brTile;
		VesselTile* b2rTile;
		VesselTile* br2Tile;
		VesselTile* tile;
		VesselTile* rTile;
		VesselTile* r2Tile;

	protected:
		
		Vessel* vessel;
		glm::ivec2 pos;
	};
}

