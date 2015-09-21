
#include "TileChunk.h"
#include "Vessel.h"
#include "NetworkWriter.h"
#include "AABBi.h"

namespace SW
{
	TileChunk::TileChunk(glm::ivec2 index, uint32_t version)
	{
		TileChunk::version = version;
		TileChunk::flags = 0;
		TileChunk::version = 0;
		TileChunk::index = index;

		for (size_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
		{
			elements[i] = NULL;
		}
	}
	TileChunk::TileChunk(glm::ivec2 index)
	{
		version = 0;
		TileChunk::flags = 0;
		TileChunk::version = 0;
		TileChunk::index = index;

		for (size_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
		{
			elements[i] = NULL;
		}
	}


	TileChunk::~TileChunk()
	{
	}

	void TileChunk::Set(glm::ivec2 index, Tile* val)
	{
		FixedRectangleSet<Tile*, CHUNK_SIZE, CHUNK_SIZE>::Set(index, val);
	}
	Tile* TileChunk::TryGet(glm::ivec2 index)
	{
		return FixedRectangleSet<Tile*, CHUNK_SIZE, CHUNK_SIZE>::TryGet(index);
	}
	AABBi TileChunk::GetAABB()
	{
		glm::ivec2 org = OriginTileIndex();
		return AABBi(org, org + glm::ivec2(CHUNK_SIZE, CHUNK_SIZE));
	}
	glm::ivec2 TileChunk::OriginTileIndex()
	{
		return glm::ivec2(index.x * CHUNK_SIZE, index.y * CHUNK_SIZE);
	}
}
