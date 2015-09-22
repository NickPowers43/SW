#pragma once
#include <SW/SW.h>
#include <SW/TileChunk.h>
#include "GraphicsCommon.h"
#include "NetworkWriter.h"

namespace SW_Client
{
	class TileChunk :
		public SW::TileChunk
	{
	public:
		TileChunk(glm::ivec2 index);
		~TileChunk();

	};
}

