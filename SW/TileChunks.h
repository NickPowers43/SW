#pragma once
#include "SW.h"
#include "TileSet.h"
#include "DynamicArray2D.h"

namespace SW
{
	class TileChunks :
		virtual public TileSet
	{
	public:
		TileChunks();
		~TileChunks();

		DynamicArray2D<TileChunk> chunks;

		virtual TileChunk* CreateChunkAt(glm::ivec2 index) = 0;
		TileChunk* TryGetChunk(glm::ivec2 index);
		void SetChunk(TileChunk* chunk);

		virtual Tile* TryGet(glm::ivec2 index) final;
		virtual void Set(glm::ivec2 index, Tile* val) final;
		virtual AABBi GetAABB() final;

		static glm::ivec2 TileOffset(glm::ivec2 tileI, glm::ivec2 chunkI);
		static glm::ivec2 TileIToChunkI(glm::ivec2 tileI);
		static glm::ivec2 ChunkIToTileI(glm::ivec2 chunkI);
		static glm::ivec2 WorldToChunkI(glm::vec2 worldPosition);
		static glm::vec2 ChunkIToWorld(glm::ivec2 chunkI);
	};
}

