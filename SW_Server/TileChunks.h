#pragma once
#include "../SW/TileChunks.h"
#include "../SW/TrackedCompartmentTileSet.h"
#include "../SW/TrackedTileSet.h"

namespace SW_Server
{
	class TileChunks :
		virtual public SW::TileChunks,
		virtual public SW::TrackedCompartmentTileSet,
		virtual public SW::TrackedTileSet
	{
	public:
		TileChunks();
		~TileChunks();

		std::vector<TileChunk*> modifiedChunks;

		virtual SW::TileChunk* CreateChunkAt(glm::ivec2 index, SW::TileChunkVersion_t version) final;

		void ReadChunkRequestMessage(Player* player, NetworkWriter* nw, NetworkReader* nr);

		virtual void MarkTileAsModified(glm::ivec2 index) final;
	};
}

