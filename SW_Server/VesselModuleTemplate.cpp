#include "stdafx.h"
#include "VesselModuleTemplate.h"
#include "VesselTile.h"

namespace SW_Server
{
	VesselModuleTemplate::VesselModuleTemplate(VesselTile* tiles, glm::ivec2 dim)
	{
		VesselModuleTemplate::dim = dim;
		VesselModuleTemplate::tiles = tiles;
		for (size_t i = 0; i < (dim.x * dim.y); i++)
		{
			tiles[i] = VesselTile();
		}
	}


	VesselModuleTemplate::~VesselModuleTemplate()
	{
	}

	VesselTile VesselModuleTemplate::TileAtV(glm::ivec2 index)
	{
		return tiles[(index.y * dim.x) + index.x];
	}
	VesselTile* VesselModuleTemplate::TileAt(glm::ivec2 index)
	{
		return &tiles[(index.y * dim.x) + index.x];
	}
	void VesselModuleTemplate::BuildWall(glm::ivec2 index, int count, uint8_t type)
	{
		VesselTile* tile;

		for (int i = 0; i < count; i++) {

			tile = TileAt(index);

			tile->wallMask |= (byte)(1 << ((byte)type - 1));
			tile->flags |= TileFlag::TileFlag::WallNode;

			index += wallOffsets[(byte)type];

			tile = TileAt(index);
			tile->flags |= TileFlag::TileFlag::WallNode;
		}
	}
	void VesselModuleTemplate::BuildWall(glm::ivec2 index, int count, uint8_t type, bool reverse)
	{
		if (reverse) {
			index -= wallOffsets[type] * count;
		}

		BuildWall(index, count, type);

		if (!reverse) {
			index += wallOffsets[type] * count;
		}
	}
	void VesselModuleTemplate::BuildWall(glm::ivec2* index, int count, uint8_t type, bool reverse)
	{
		if (reverse) {
			*index -= wallOffsets[type] * count;
		}

		BuildWall(*index, count, type);

		if (!reverse) {
			*index += wallOffsets[type] * count;
		}
	}
}
