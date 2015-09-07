#pragma once

#include "RigidBody.h"
#include "LinkedListNode.h"
#include "DynamicArray2D.h"
#include "AABBi.h"

namespace SW_Server
{
	static float WALL_RADIUS = 0.15f;

	class AdjacentTiles;

	class Vessel : 
		public RigidBody<VesselValueType>,
		public LinkedListNode<Vessel>
	{
	public:
		uint32_t index;
		DynamicArray2D<VesselChunk> chunks;

		Vessel(VesselVecType vel, VesselValueType m, VesselVecType pos, VesselValueType rot, Vessel* next);
		~Vessel();

		int GetMessageBytes(void* & ref);

		void IsModuleLegal();

		void InstantiateNearbyChunks(Player* player);

		void ReadChunkRequestMessage(Player* player, NetworkWriter* nw, NetworkReader* nr);
		void ReadModuleRequestMessage(Player* player, NetworkWriter* nw, NetworkReader* nr);

		void AddPlayerVessel(Player* player, NetworkWriter* nw, glm::vec2 position);
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

		std::vector<VesselModule> modules;
		uint32_t nextModuleIndex;
		std::vector<VesselObject*> objects;
		std::vector<Player*> playersOnBoard;
		std::vector<VesselChunk*> modifiedChunks;
	};
}

