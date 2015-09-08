#pragma once
#include "Tile.h"

namespace SW
{
	class TileSet
	{
	public:
		TileSet();
		~TileSet();

		virtual Tile* TryGet(glm::ivec2 index) = 0;
		virtual void Set(glm::ivec2 index, Tile* val) = 0;
		virtual AABBi GetAABB() = 0;

		void BuildWall(glm::ivec2 index, int count, uint8_t type);
		void BuildWall(glm::ivec2 index, int count, uint8_t type, bool reverse);
		void BuildWall(glm::ivec2* index, int count, uint8_t type, bool reverse);

		bool IsWallLegal(glm::ivec2 index, uint8_t wallType);
		bool ContainsWall(glm::ivec2 index);
		bool LegalWallStart(uint8_t type, glm::ivec2 index);
		bool TooCloseToWall(glm::ivec2 index);
		bool EmptyRect(glm::ivec2 bl, glm::ivec2 size);
		bool EmptyTile(glm::ivec2 loc);
		bool LegalWallEnd(uint8_t type, glm::ivec2 index);
		bool NonAcuteSequence(uint8_t wall0, uint8_t wall1);

		glm::ivec2 WorldToTileI(glm::vec2 worldPosition);
		glm::vec2 TileIToWorld(glm::ivec2 tileI);
	};
}

