#pragma once
#include "TileChunks.h"
#include "NetworkReader.h"
#include "NetworkWriter.h"
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

		Player* myPlayer;
		TileChunks tiles;

		void AddMyPlayer(Player* player, NetworkWriter* nw);
		void Update(NetworkWriter* nw);
		void Clear();

		void DrawWorld();
		void DrawFloor();
		void DrawWalls();
		void DrawShadows();

		void ReadSetTilesMessage(NetworkReader* nr, NetworkWriter* nw);
		void InstantiateChunk(TileChunk* chunk);
	};

}
