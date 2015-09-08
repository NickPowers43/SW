
#include "VesselModuleTemplate.h"
#include "Tile.h"

namespace SW
{
	VesselModuleTemplate::VesselModuleTemplate(glm::ivec2 dim)
	{
		VesselModuleTemplate::dim = dim;
		VesselModuleTemplate::tiles = tiles = new Tile[(dim.x + 1) * (dim.y + 1)];
		for (size_t i = 0; i < ((dim.x + 1) * (dim.y + 1)); i++)
		{
			tiles[i] = Tile();
		}
	}


	VesselModuleTemplate::~VesselModuleTemplate()
	{
	}

	Tile VesselModuleTemplate::TileAtV(glm::ivec2 index)
	{
		return tiles[(index.y * dim.x) + index.x];
	}
	Tile* VesselModuleTemplate::TileAt(glm::ivec2 index)
	{
		return &tiles[(index.y * dim.x) + index.x];
	}
	void VesselModuleTemplate::BuildWall(glm::ivec2 index, int count, WallType_t type)
	{
		Tile* tile;

		for (int i = 0; i < count; i++) {

			tile = TileAt(index);

			tile->wallMask |= (1 << (type - 1));
			tile->flags |= TileFlag::TileFlag::WallNode;

			index += wallOffsets[type];

			tile = TileAt(index);
			tile->flags |= TileFlag::TileFlag::WallNode;
		}
	}
	void VesselModuleTemplate::BuildWall(glm::ivec2 index, int count, WallType_t type, bool reverse)
	{
		if (reverse) {
			index -= wallOffsets[type] * count;
		}

		BuildWall(index, count, type);

		if (!reverse) {
			index += wallOffsets[type] * count;
		}
	}
	void VesselModuleTemplate::BuildWall(glm::ivec2* index, int count, WallType_t type, bool reverse)
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
