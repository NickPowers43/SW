#pragma once
#include "TileSet.h"

namespace SW
{
	class TrackedTileSet :
		virtual public TileSet
	{
	public:
		TrackedTileSet();
		~TrackedTileSet();

		virtual void MarkTileAsModified(glm::ivec2 index);
	};

}
