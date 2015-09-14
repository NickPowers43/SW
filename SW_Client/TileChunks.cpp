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

	TileChunk* TileChunks::ClientChunkTop(SW::TileChunk* chunk) { SW::TileChunk* temp; if ((temp = TryGetChunk(glm::ivec2(chunk->index.x, chunk->index.y + 1)))) return static_cast<TileChunk*>(temp); else return NULL; }
	TileChunk* TileChunks::ClientChunkBottom(SW::TileChunk* chunk) { SW::TileChunk* temp; if ((temp = TryGetChunk(glm::ivec2(chunk->index.x, chunk->index.y - 1)))) return static_cast<TileChunk*>(temp); else return NULL; }
	TileChunk* TileChunks::ClientChunkLeft(SW::TileChunk* chunk) { SW::TileChunk* temp; if ((temp = TryGetChunk(glm::ivec2(chunk->index.x - 1, chunk->index.y)))) return static_cast<TileChunk*>(temp); else return NULL; }
	TileChunk* TileChunks::ClientChunkRight(SW::TileChunk* chunk) { SW::TileChunk* temp; if ((temp = TryGetChunk(glm::ivec2(chunk->index.x + 1, chunk->index.y)))) return static_cast<TileChunk*>(temp); else return NULL; }
	TileChunk* TileChunks::ClientChunkTopLeft(SW::TileChunk* chunk) { SW::TileChunk* temp; if ((temp = TryGetChunk(glm::ivec2(chunk->index.x - 1, chunk->index.y + 1)))) return static_cast<TileChunk*>(temp); else return NULL; }
	TileChunk* TileChunks::ClientChunkTopRight(SW::TileChunk* chunk) { SW::TileChunk* temp; if ((temp = TryGetChunk(glm::ivec2(chunk->index.x + 1, chunk->index.y + 1)))) return static_cast<TileChunk*>(temp); else return NULL; }
	TileChunk* TileChunks::ClientChunkBottomLeft(SW::TileChunk* chunk) { SW::TileChunk* temp; if ((temp = TryGetChunk(glm::ivec2(chunk->index.x - 1, chunk->index.y - 1)))) return static_cast<TileChunk*>(temp); else return NULL; }
	TileChunk* TileChunks::ClientChunkBottomRight(SW::TileChunk* chunk) { SW::TileChunk* temp; if ((temp = TryGetChunk(glm::ivec2(chunk->index.x + 1, chunk->index.y - 1)))) return static_cast<TileChunk*>(temp); else return NULL; }

	SW::TileChunk* TileChunks::CreateChunkAt(glm::ivec2 index, TileChunkVersion_t version)
	{
		return new TileChunk(index, version);
	}
}
