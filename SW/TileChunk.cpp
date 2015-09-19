
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
		TileChunk::data = new Tile*[CHUNK_SIZE * CHUNK_SIZE];

		for (size_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
		{
			data[i] = NULL;
		}
	}
	TileChunk::TileChunk(glm::ivec2 index)
	{
		version = 0;
		TileChunk::flags = 0;
		TileChunk::version = 0;
		TileChunk::index = index;
		TileChunk::data = new Tile*[CHUNK_SIZE * CHUNK_SIZE];

		for (size_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
		{
			data[i] = NULL;
		}
	}


	TileChunk::~TileChunk()
	{
		for (size_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
		{
			if (data[i])
			{
				DestroyTile(data[i]);
			}
		}

		delete data;
	}

	void TileChunk::Set(glm::ivec2 index, Tile* val)
	{
		data[index.x + (index.y * CHUNK_SIZE)] = val;
	}
	Tile* TileChunk::TryGet(glm::ivec2 index)
	{
		return data[index.x + (index.y * CHUNK_SIZE)];
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
