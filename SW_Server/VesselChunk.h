#pragma once

#include <glm\vec2.hpp>
#include "VesselTile.h"

namespace SW_Server
{
	const uint8_t VesselChunkFlag_None = 0;
	const uint8_t VesselChunkFlag_Seen = 1;
	const uint8_t VesselChunkFlag_Modified = 1;

	class VesselChunk
	{
	public:

		static float chunkUpdateInterval;

		uint8_t flags;
		glm::ivec2 index;
		VesselTile** data;
		uint32_t version;

		VesselChunk(glm::ivec2 index, uint32_t version);
		VesselChunk(glm::ivec2 index);
		~VesselChunk();

		void SetTile(glm::ivec2 offset, VesselTile* val);
		VesselTile* TileAt(int x, int y);
		VesselTile* TileAt(glm::ivec2 index);
		glm::ivec2 OriginTileIndex();

		void Instantiate(VesselChunk* t, VesselChunk* l, VesselChunk* r, VesselChunk* b, VesselChunk* br, glm::vec2 position);
		void Destroy();

		bool modified;

	private:
		char* messageBytes;
		int messageBytesSize;
	};
}

