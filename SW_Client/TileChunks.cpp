#include "TileChunks.h"
#include "TileChunk.h"


namespace SW_Client
{
	TileChunks::TileChunks()
	{
	}


	TileChunks::~TileChunks()
	{
	}

	void TileChunks::DestroyChunks()
	{
		for (size_t i = 0; i < chunks.dim.y; i++)
		{
			for (size_t j = 0; j < chunks.dim.x; j++)
			{
				SW::TileChunk* swChunk = TryGetChunk(chunks.origin + glm::ivec2(j, i));
				if (swChunk)
				{
					TileChunk* chunk = static_cast<TileChunk*>(swChunk);
					chunk->Destroy();
				}
			}
		}
	}

	SW::TileChunk* TileChunks::CreateChunkAt(glm::ivec2 index)
	{
		TileChunk* chunk = new TileChunk(index);
		chunks.Set(index, chunk);
		return chunk;
	}
}
