#include "TileChunk.h"
#include "SW_Client.h"
#include <sstream>
#include <SW/AABBi.h>
#include <SW/Tile.h>
#include <glm/trigonometric.hpp>

namespace SW_Client
{
	TileChunk::TileChunk(glm::ivec2 index) : SW::TileChunk(index)
	{
	}


	TileChunk::~TileChunk()
	{
	}

}
