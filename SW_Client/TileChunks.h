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

		void DestroyChunks();
		virtual SW::TileChunk* CreateChunkAt(glm::ivec2 index) final;
	};

}
