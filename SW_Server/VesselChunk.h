#pragma once

#include <glm\vec2.hpp>
#include "VesselTile.h"

namespace SW_Server
{
	class VesselChunk
	{
	public:

		static float chunkUpdateInterval;

		glm::ivec2 index;
		VesselTile** data;


		VesselChunk(glm::ivec2 index);
		~VesselChunk();

		void SetTile(glm::ivec2 offset, VesselTile* val);
		VesselTile* TileAt(int x, int y);
		VesselTile* TileAt(glm::ivec2 index);
		glm::ivec2 OriginTileIndex();

		void Instantiate(VesselChunk* t, VesselChunk* l, VesselChunk* r, VesselChunk* b, VesselChunk* br, glm::vec2 position);
		void Destroy();
	};
}

