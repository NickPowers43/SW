#pragma once
#include <SW/SW.h>
#include <SW/TileChunks.h>
#include "TileChunk.h"

namespace SW_Client
{
	class TileChunks :
		public SW::TileChunks
	{
	public:
		TileChunks();
		~TileChunks();

		TileChunk* ClientChunkTop(SW::TileChunk* chunk);
		TileChunk* ClientChunkBottom(SW::TileChunk* chunk);
		TileChunk* ClientChunkLeft(SW::TileChunk* chunk);
		TileChunk* ClientChunkRight(SW::TileChunk* chunk);
		TileChunk* ClientChunkTopLeft(SW::TileChunk* chunk);
		TileChunk* ClientChunkTopRight(SW::TileChunk* chunk);
		TileChunk* ClientChunkBottomLeft(SW::TileChunk* chunk);
		TileChunk* ClientChunkBottomRight(SW::TileChunk* chunk);

		void DestroyChunks();
		virtual SW::TileChunk* CreateChunkAt(glm::ivec2 index, TileChunkVersion_t version) final;
	};

}
