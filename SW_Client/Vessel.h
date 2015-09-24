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
		Vessel(VesselIndex_t index);
		~Vessel();

		SW::Compartment* playerComp;
		Player* myPlayer;
		TileChunks tiles;

		bool fMeshCreated;
		int fIndicesCount;
		GLuint floorVBuffer;
		GLuint floorIBuffer;

		void AddMyPlayer(Player* player, NetworkWriter* nw);
		void Update(NetworkWriter* nw);
		void Clear();

		void DrawWorld();
		void DrawWalls(SW::TileSet* ts, SW::AABBi region);
		void GenerateFloorMesh(SW::TileSet* ts, SW::AABBi region);

		void ReadSetTilesMessage(NetworkReader* nr, NetworkWriter* nw);
	};

}
