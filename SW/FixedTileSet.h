#pragma once
#include "TileSet.h"
namespace SW
{
	class FixedTileSet :
		virtual public TileSet
	{
	public:
		FixedTileSet(glm::ivec2 dim);
		~FixedTileSet();

		glm::ivec2 dim;
		Tile** tiles;

		virtual Tile* TryGet(glm::ivec2 index) final;
		virtual void Set(glm::ivec2 index, Tile* val) final;
		virtual AABBi GetAABB() final;

		Tile* TileAt(glm::ivec2 index);
	};
}

