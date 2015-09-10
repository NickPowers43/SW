#pragma once
#include "../SW/SW.h"
#include "../SW/TileChunk.h"
#include "../SW/NetworkWriter.h"

namespace SW_Server
{
	class TileChunk :
		public SW::TileChunk
	{
	public:
		TileChunk(glm::ivec2 index);
		TileChunk(glm::ivec2 index, TileChunkVersion_t version);
		~TileChunk();

		virtual SW::Tile* CreateTile() override;
		virtual void DestroyTile(SW::Tile* tile) override;

		void WriteSetChunkMessage(SW::NetworkWriter* nw);

		int dummy;
	};
}

