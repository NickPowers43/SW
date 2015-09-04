#pragma once

#include "AABBi.h"
#include "VesselTile.h"
#include "VesselChunk.h"
#include "DynamicArray2D.h"
#include "Player.h"
#include "VesselObject.h"

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

	class AdjacentTiles;

	class Vessel
	{
	public:
		uint32_t index;
		DynamicArray2D<VesselChunk> chunks;

		Vessel();
		~Vessel();

		int GetMessageBytes(void* & ref);

		void InstantiateNearbyChunks(Player* player);
		void ReadChunkRequestMessage(Player* player, NetworkReader* nr);
		void AddPlayerVessel(Player* player, glm::vec2 position);
		void RemovePlayer(Player* player);
		virtual void BuildFoundation(glm::ivec2 origin, glm::ivec2 size);
		//virtual void PlaceBlock(uint16_t type, glm::ivec2 location);
		VesselChunk* CreateChunk(glm::ivec2 index);
		void AddModifiedChunk(VesselChunk* vc);
		void SetTile(glm::ivec2 index, VesselTile* val);
		void BuildWall(glm::ivec2 index, int count, uint8_t type);
		void BuildWall(glm::ivec2 index, int count, uint8_t type, bool reverse);
		void BuildWall(glm::ivec2* index, int count, uint8_t type, bool reverse);
		void FillTile(AdjacentTiles* t);
		void RebuildCompartments();
		Compartment* CompartmentAt(glm::vec2 world);
		void SetCompartmentFloor(uint8_t type, Compartment* c);
		virtual VesselObject* PlaceObject(ObjectType::ObjectType type, glm::vec2 location);

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

	protected:
		
		bool updateMessageBytes;
		void* messageBytes;
		int messageBytesCount;

		uint32_t nextChunkIndex;

		static uint32_t nextIndex;
		uint32_t nextCompartmentIndex;

		AABBi aabb;
		bool interiorExists;
		bool noPlayers;
		float timeEmpty;

		std::vector<VesselObject*> objects;
		std::vector<Player*> playersOnBoard;
		std::vector<VesselChunk*> modifiedChunks;
	};
}

