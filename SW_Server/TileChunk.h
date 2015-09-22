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
		~TileChunk();
	};
}

