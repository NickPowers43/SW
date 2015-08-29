#pragma once

#include "stdafx.h"
#include "AABBi.h"
#include "VesselTile.h"
#include "VesselChunk.h"
#include "DynamicArray2D.h"

namespace SW_Server
{
	static glm::ivec2* wallOffsets = new glm::ivec2[9] {
		glm::ivec2(0, 0),
		glm::ivec2(1, 0),
		glm::ivec2(2, 1),
		glm::ivec2(1, 1),
		glm::ivec2(1, 2),
		glm::ivec2(0, 1),
		glm::ivec2(-1, 2),
		glm::ivec2(-1, 1),
		glm::ivec2(-2, 1)
	};

	static int PLAYER_CHUNK_RANGE = 2;
	static int CHUNK_SIZE_POW = 3;
	static int CHUNK_SIZE = 1 << CHUNK_SIZE_POW;
	static int CHUNK_DATA_COUNT = CHUNK_SIZE * CHUNK_SIZE;
	static int CHUNK_OFFSET_MASK = (1 << (CHUNK_SIZE_POW + 1)) - 1;

	static float WALL_RADIUS = 0.15f;

	class Vessel
	{
	public:
		uint32_t index;
		DynamicArray2D<VesselChunk> chunks;

		Vessel();
		~Vessel();

		bool IsWallLegal(glm::ivec2 index, uint8_t wallType);
		bool ContainsWall(glm::ivec2 index);
		bool LegalWallStart(uint8_t type, glm::ivec2 index);
		bool TooCloseToWall(glm::ivec2 index);
		bool EmptyRect(glm::ivec2 bl, glm::ivec2 size);
		bool EmptyTile(glm::ivec2 loc);
		bool LegalWallEnd(uint8_t type, glm::ivec2 index);
		bool NonAcuteSequence(uint8_t wall0, uint8_t wall1);

		VesselTile* TryGetTile(glm::ivec2 index);
		VesselChunk* Top(VesselChunk* chunk);
		VesselChunk* Bottom(VesselChunk* chunk);
		VesselChunk* Left(VesselChunk* chunk);
		VesselChunk* Right(VesselChunk* chunk);
		VesselChunk* TopLeft(VesselChunk* chunk);
		VesselChunk* TopRight(VesselChunk* chunk);
		VesselChunk* BottomLeft(VesselChunk* chunk);
		VesselChunk* BottomRight(VesselChunk* chunk);
		void InstantiateChunk(VesselChunk* chunk);

		glm::ivec2 TileOffset(glm::ivec2 tileI, glm::ivec2 chunkI);
		glm::ivec2 TileIToChunkI(glm::ivec2 tileI);
		glm::ivec2 ChunkIToTileI(glm::ivec2 chunkI);
		glm::ivec2 WorldToChunkI(glm::vec2 worldPosition);
		glm::ivec2 WorldToTileI(glm::vec2 worldPosition);
		glm::vec2 ChunkIToWorld(glm::ivec2 chunkI);
		glm::vec2 TileIToWorld(glm::ivec2 tileI);

	private:
		
		uint32_t nextChunkIndex;

		static uint32_t nextIndex;

		AABBi aabb;
		bool interiorExists;

		std::vector<ServerVC*> modifiedChunks;
	};
}

