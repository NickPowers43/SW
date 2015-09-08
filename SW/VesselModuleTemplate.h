#pragma once
#include "SW.h"

namespace SW
{
	class VesselModuleTemplate
	{
	public:
		VesselModuleTemplate(glm::ivec2 dim);
		~VesselModuleTemplate();

		glm::ivec2 dim;
		Tile* tiles;

		Tile TileAtV(glm::ivec2 index);
		Tile* TileAt(glm::ivec2 index);
		void BuildWall(glm::ivec2 index, int count, WallType_t type);
		void BuildWall(glm::ivec2 index, int count, WallType_t type, bool reverse);
		void BuildWall(glm::ivec2* index, int count, WallType_t type, bool reverse);
	};

}
