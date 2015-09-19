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

		TileChunks tiles;
		Player* myPlayer;

		void AddMyPlayer(Player* player, NetworkWriter* nw);
		void Update(NetworkWriter* nw);
		void Clear();
		void UpdateChunks(bool force, NetworkWriter* nw);

		void DrawWorld();
		void DrawFloor();
		void DrawWalls();
		void DrawShadows();

		void ReadSetChunkMessage(NetworkReader* nr, NetworkWriter* nw);
		void InstantiateChunk(TileChunk* chunk);
	};

}
