
#include "TileChunks.h"
#include "AABBi.h"
#include "TileChunk.h"

namespace SW
{
	TileChunks::TileChunks()
	{
	}


	TileChunks::~TileChunks()
	{
	}

	TileChunk* TileChunks::TryGetChunk(glm::ivec2 index)
	{
		return chunks.TryGet(index);
	}
	void TileChunks::SetChunk(TileChunk* chunk)
	{
		chunks.Set(chunk->index, chunk);
	}

	TileChunk* TileChunks::Top(TileChunk* chunk) { return TryGetChunk(glm::ivec2(chunk->index.x, chunk->index.y + 1)); }
	TileChunk* TileChunks::Bottom(TileChunk* chunk) { return TryGetChunk(glm::ivec2(chunk->index.x, chunk->index.y - 1)); }
	TileChunk* TileChunks::Left(TileChunk* chunk) { return TryGetChunk(glm::ivec2(chunk->index.x - 1, chunk->index.y)); }
	TileChunk* TileChunks::Right(TileChunk* chunk) { return TryGetChunk(glm::ivec2(chunk->index.x + 1, chunk->index.y)); }
	TileChunk* TileChunks::TopLeft(TileChunk* chunk) { return TryGetChunk(glm::ivec2(chunk->index.x - 1, chunk->index.y + 1)); }
	TileChunk* TileChunks::TopRight(TileChunk* chunk) { return TryGetChunk(glm::ivec2(chunk->index.x + 1, chunk->index.y + 1)); }
	TileChunk* TileChunks::BottomLeft(TileChunk* chunk) { return TryGetChunk(glm::ivec2(chunk->index.x - 1, chunk->index.y - 1)); }
	TileChunk* TileChunks::BottomRight(TileChunk* chunk) { return TryGetChunk(glm::ivec2(chunk->index.x + 1, chunk->index.y - 1)); }

	Tile* TileChunks::TryGet(glm::ivec2 index)
	{
		glm::ivec2 chunkI = TileIToChunkI(index);
		TileChunk* chunk = chunks.TryGet(chunkI);

		if (!chunk) {
			return NULL;
		}
		else {
			index -= ChunkIToTileI(chunkI);
			return chunk->TryGet(index);
		}
	}
	void TileChunks::Set(glm::ivec2 index, Tile* val)
	{
		glm::ivec2 chunkI = TileIToChunkI(index);
		TileChunk* chunk = chunks.TryGet(chunkI);

		if (!chunk) {
			chunk = CreateChunkAt(chunkI, 0);
		}

		index -= chunk->OriginTileIndex();

		chunk->Set(index, val);

	}
	AABBi TileChunks::GetAABB()
	{
		return AABBi(ChunkIToTileI(chunks.origin), ChunkIToTileI(chunks.origin + chunks.dim));
	}


	glm::ivec2 TileChunks::TileOffset(glm::ivec2 tileI, glm::ivec2 chunkI)
	{
		glm::ivec2 rel = ChunkIToTileI(chunkI);
		return tileI - rel;
	}
	glm::ivec2 TileChunks::TileIToChunkI(glm::ivec2 tileI)
	{
		glm::ivec2 output;
		if (tileI.x < 0)
		{
			tileI.x++;
			output.x = (tileI.x / CHUNK_SIZE) - 1;
		}
		else
		{
			output.x = tileI.x / CHUNK_SIZE;
		}
		if (tileI.y < 0)
		{
			tileI.y++;
			output.y = (tileI.y / CHUNK_SIZE) - 1;
		}
		else
		{
			output.y = tileI.y / CHUNK_SIZE;
		}
		return output;
	}
	glm::ivec2 TileChunks::ChunkIToTileI(glm::ivec2 chunkI)
	{
		return chunkI * CHUNK_SIZE;
	}
	glm::ivec2 TileChunks::WorldToChunkI(glm::vec2 worldPosition)
	{
		return glm::ivec2(
			(worldPosition.x < 0.0f) ? -1 - (int)(worldPosition.x / -(float)CHUNK_SIZE) : (int)(worldPosition.x / (float)CHUNK_SIZE),
			(worldPosition.y < 0.0f) ? -1 - (int)(worldPosition.y / -(float)CHUNK_SIZE) : (int)(worldPosition.y / (float)CHUNK_SIZE));
	}
	glm::vec2 TileChunks::ChunkIToWorld(glm::ivec2 chunkI)
	{
		return glm::vec2(
			chunkI.x * (float)CHUNK_SIZE,
			chunkI.y * (float)CHUNK_SIZE);
	}
}
