#include "stdafx.h"
#include "TileChunk.h"
#include "Tile.h"

namespace SW_Server
{
	TileChunk::TileChunk(glm::ivec2 index) : SW::TileChunk(index, 0)
	{
	}
	TileChunk::TileChunk(glm::ivec2 index, TileChunkVersion_t version) : SW::TileChunk(index, version)
	{
	}


	TileChunk::~TileChunk()
	{
	}

	SW::Tile* TileChunk::CreateTile()
	{
		return new Tile();
	}
	void TileChunk::DestroyTile(SW::Tile* tile)
	{
		delete (static_cast<Tile*>(tile));
	}

	void TileChunk::WriteSetChunkMessage(SW::NetworkWriter* nw)
	{
		nw->Write((int16_t)index.x);
		nw->Write((int16_t)index.y);
		nw->Write((uint32_t)version);

		uint16_t* tile_count = (uint16_t*)nw->cursor;
		nw->Write((uint16_t)0);
		for (size_t i = 0; i < CHUNK_DATA_COUNT; i++)
		{
			if (elements[i])
			{
				(*tile_count)++;
				nw->Write((uint16_t)i);
				(static_cast<Tile*>(elements[i]))->WriteSetChunkMessage(nw);
			}
		}
	}
}
