#pragma once
namespace SW_Server
{
	class VesselModuleTemplate
	{
	public:
		VesselModuleTemplate(VesselTile* tiles, glm::ivec2 dim);
		~VesselModuleTemplate();

		glm::ivec2 dim;
		VesselTile* tiles;

		VesselTile TileAtV(glm::ivec2 index);
		VesselTile* TileAt(glm::ivec2 index);
		void BuildWall(glm::ivec2 index, int count, uint8_t type);
		void BuildWall(glm::ivec2 index, int count, uint8_t type, bool reverse);
		void BuildWall(glm::ivec2* index, int count, uint8_t type, bool reverse);
	};

}
