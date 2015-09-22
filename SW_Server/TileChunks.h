#pragma once
#include <SW/TileChunks.h>
#include <SW/TrackedTileSet.h>
#include <SW/CompartmentTileSet.h>

namespace SW_Server
{
	class TileChunks :
		virtual public SW::TileChunks,
		virtual public SW::CompartmentTileSet,
		virtual public SW::TrackedTileSet
	{
	public:
		TileChunks();
		~TileChunks();

		virtual SW::TileChunk* CreateChunkAt(glm::ivec2 index) final;

	};
}

