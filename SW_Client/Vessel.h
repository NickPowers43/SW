#pragma once
#include "TileChunks.h"
#include "NetworkReader.h"
#include "NetworkWriter.h"
#include <SW/AABBi.h>
#include <SW/SW.h>
#include <SW/Vessel.h>
#include "Player.h"

namespace SW_Client
{
	class Vessel :
		public SW::Vessel
	{
	public:
		Vessel(VesselIndex_t index, glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 rot);
		~Vessel();

		SW::Compartment* playerComp;
		Player* myPlayer;
		TileChunks tiles;

		bool fMeshCreated;
		int fIndicesCount;
		GLuint floorVBuffer;
		GLuint floorIBuffer;

		void AddMyPlayer(Player* player, NetworkWriter* nw);
		void ProcessInputs(NetworkWriter* nw);
		void Step(NetworkWriter* nw);
		void Clear();

		void DrawWorld();
		void DrawTiles(SW::TileSet* ts, SW::AABBi region);
		void GenerateFloorMesh(SW::TileSet* ts, SW::AABBi region);

		void ReadSetTilesMessage(NetworkReader* nr, NetworkWriter* nw);
	};

}
