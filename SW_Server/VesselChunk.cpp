#include "stdafx.h"
#include "VesselChunk.h"
#include "Vessel.h"
#include "NetworkWriter.h"

namespace SW_Server
{
	VesselChunk::VesselChunk(glm::ivec2 index)
	{
		modified = true;
		VesselChunk::version = 0;
		VesselChunk::index = index;
		data = new VesselTile*[CHUNK_SIZE * CHUNK_SIZE];
		for (size_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
		{
			data[i] = NULL;
		}
	}


	VesselChunk::~VesselChunk()
	{
	}

	void VesselChunk::WriteSetChunkMessage(NetworkWriter* nw)
	{
		nw->Write((uint16_t)index.x);
		nw->Write((uint16_t)index.y);

		uint16_t* tile_count = (uint16_t*)nw->cursor;
		nw->Write((uint16_t)0);
		for (size_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
		{
			if (data[i])
			{
				*tile_count++;
				data[i]->WriteSetChunkMessage(nw);
			}
		}
	}

	void VesselChunk::SetTile(glm::ivec2 offset, VesselTile* val)
	{
		data[offset.x + (offset.y * CHUNK_SIZE)] = val;
	}

	VesselTile* VesselChunk::TileAt(int x, int y)
	{
		return data[x + (y * CHUNK_SIZE)];
	}

	VesselTile* VesselChunk::TileAt(glm::ivec2 index)
	{
		return data[index.x + (index.y * CHUNK_SIZE)];
	}

	glm::ivec2 VesselChunk::OriginTileIndex()
	{
		return glm::ivec2(index.x * CHUNK_SIZE, index.y * CHUNK_SIZE);
	}

	void VesselChunk::Instantiate(VesselChunk* t, VesselChunk* l, VesselChunk* r, VesselChunk* b, VesselChunk* br, glm::vec2 position)
	{
		//TODO: 
	}
	void VesselChunk::Destroy()
	{
		//TODO: destroy instantiated objects
	}
}
