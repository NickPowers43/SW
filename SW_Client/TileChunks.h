#pragma once
#include <SW/SW.h>
#include <SW/TileChunks.h>
#include <SW/CompartmentTileSet.h>
#include "TileChunk.h"

namespace SW_Client
{
	class TileChunks :
		virtual public SW::TileChunks,
		virtual public SW::CompartmentTileSet
	{
	public:
		TileChunks();
		~TileChunks();

		void DestroyChunks();
		virtual SW::TileChunk* CreateChunkAt(glm::ivec2 index) final;
	};

}
