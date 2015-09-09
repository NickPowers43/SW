#pragma once
#include "SW.h"
#include "Tile.h"
#include "TileSet.h"

namespace SW
{
	class TileChunk :
		public TileSet
	{
	public:

		TileChunk(glm::ivec2 index, uint32_t version);
		TileChunk(glm::ivec2 index);
		~TileChunk();

		uint8_t flags;
		glm::ivec2 index;
		Tile** data;
		uint32_t version;

		virtual Tile* TryGet(glm::ivec2 index) final;
		virtual void Set(glm::ivec2 index, Tile* val) final;
		virtual AABBi GetAABB() final;

		glm::ivec2 OriginTileIndex();
	};
}
