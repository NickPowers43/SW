#include "FixedTileSet.h"
#include "AABBi.h"

namespace SW
{
	FixedTileSet::FixedTileSet(glm::ivec2 dim)
	{
		FixedTileSet::dim = dim;
		FixedTileSet::tiles = tiles = new Tile*[dim.x * dim.y];
		for (size_t i = 0; i < (dim.x * dim.y); i++)
		{
			tiles[i] = CreateTile();
		}
	}


	FixedTileSet::~FixedTileSet()
	{
		if (tiles)
		{
			for (size_t i = 0; i < (dim.x * dim.y); i++)
			{
				if (tiles[i])
				{
					DestroyTile(tiles[i]);
				}
			}

			delete tiles;
		}
	}

	Tile* FixedTileSet::TryGet(glm::ivec2 index)
	{
		return tiles[(index.y * dim.x) + index.x];
	}
	void FixedTileSet::Set(glm::ivec2 index, Tile* val)
	{
		tiles[(index.y * dim.x) + index.x] = val;
	}
	AABBi FixedTileSet::GetAABB()
	{
		return AABBi(glm::ivec2(0, 0), dim);
	}

	Tile* FixedTileSet::TileAt(glm::ivec2 index)
	{
		return tiles[(index.y * dim.x) + index.x];
	}
}
