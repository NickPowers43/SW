#pragma once
#include "../SW/CompartmentTile.h"
#include "../SW/NetworkWriter.h"

namespace SW_Server
{
	class Tile :
		public SW::CompartmentTile
	{
	public:
		Tile();
		~Tile();

		void WriteSetChunkMessage(SW::NetworkWriter* nw);
	};
}

