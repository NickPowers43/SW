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

		TileChunk* Top(TileChunk* chunk);
		TileChunk* Bottom(TileChunk* chunk);
		TileChunk* Left(TileChunk* chunk);
		TileChunk* Right(TileChunk* chunk);
		TileChunk* TopLeft(TileChunk* chunk);
		TileChunk* TopRight(TileChunk* chunk);
		TileChunk* BottomLeft(TileChunk* chunk);
		TileChunk* BottomRight(TileChunk* chunk);

		virtual Tile* TryGet(glm::ivec2 index) final;
		virtual void Set(glm::ivec2 index, Tile* val) final;
		virtual AABBi GetAABB() final;

		glm::ivec2 TileOffset(glm::ivec2 tileI, glm::ivec2 chunkI);
		glm::ivec2 TileIToChunkI(glm::ivec2 tileI);
		glm::ivec2 ChunkIToTileI(glm::ivec2 chunkI);
		glm::ivec2 WorldToChunkI(glm::vec2 worldPosition);
		glm::vec2 ChunkIToWorld(glm::ivec2 chunkI);
	};
}

