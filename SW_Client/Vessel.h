#pragma once
#include "TileChunks.h"
#include "NetworkReader.h"
#include "NetworkWriter.h"
#include <SW/SW.h>
#include <SW/Vessel.h>

namespace SW_Client
{
	class Vessel :
		public SW::Vessel
	{
	public:
		Vessel(VesselIndex_t index);
		~Vessel();

		TileChunks tiles;

		void ReadSetChunkMessage(NetworkReader* nr, NetworkWriter* nw);
		void InstantiateChunk(TileChunk* chunk);
	};

}
