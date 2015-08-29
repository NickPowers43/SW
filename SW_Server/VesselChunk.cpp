#include "stdafx.h"
#include "VesselChunk.h"
#include "Vessel.h"

namespace SW_Server
{
	VesselChunk::VesselChunk(glm::ivec2 index)
	{
		VesselChunk::version = 0;
		VesselChunk::index = index;
		data = new VesselTile*[CHUNK_SIZE * CHUNK_SIZE];
	}


	VesselChunk::~VesselChunk()
	{
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
