#pragma once
#include "Compartment.h"

namespace SW_Server
{
	class VesselTile
	{
	public:
		uint16_t flags;
		uint8_t wallMask;
		uint8_t floor0;
		uint8_t floor1;
		Compartment* c0;
		Compartment* c1;

		VesselTile();
		~VesselTile();

		void WriteSetChunkMessage(NetworkWriter* nw);
		int WallCount();
		int GetWalls(uint8_t* wall0, uint8_t* wall1);
		bool Contains(uint8_t wall);
		bool ContainsMask(uint8_t wall);
	};
}

