#include "stdafx.h"
#include "TileChunks.h"
#include "TileChunk.h"
#include "Player.h"
#include <SW\NetworkWriter.h>
#include <SW\NetworkReader.h>
#include <SW\CompartmentTile.h>



namespace SW_Server
{
	TileChunks::TileChunks()
	{
	}


	TileChunks::~TileChunks()
	{
	}

	SW::TileChunk* TileChunks::CreateChunkAt(glm::ivec2 index)
	{
		TileChunk* chunk = new TileChunk(index);
		chunks.Set(index, chunk);
		return chunk;
	}
}
