#pragma once
#include <SW\Vessel.h>
#include <SW/RigidBody.h>
#include <SW/LinkedListNode.h>
#include "TileChunks.h"

#include <SW/VesselObject.h>
#include <SW\DynamicArray2D.h>
#include <SW\AABBi.h>
#include <SW\NetworkWriter.h>
#include <SW\NetworkReader.h>
#include <SW/VesselModule.h>

namespace SW_Server
{
	class Vessel :
		public SW::Vessel,
		public SW::RigidBody<VesselValueType>,
		public SW::LinkedListNode<Vessel>
	{
	public:
		Vessel(VesselIndex_t index, VesselVecType vel, VesselValueType m, VesselVecType pos, VesselValueType rot, Vessel* next);
		~Vessel();

		void ReadChunkRequestMessage(Player* player, NetworkWriter* nw, NetworkReader* nr);
		void ReadModuleRequestMessage(Player* player, NetworkWriter* nw, NetworkReader* nr);

		void BroadcastPlayerAddition(Player* player, NetworkWriter* nw);
		size_t GetOnBoardPlayerInformationMessageSize();
		void SendMakeVesselActive(Player* player, NetworkWriter* nw);
		void AddPlayerVessel(Player* player, NetworkWriter* nw, glm::vec2 position);
		void RemovePlayer(Player* player);
		void BuildModule(VMType_t type, glm::ivec2 position);

		virtual SW::VesselObject* PlaceObject(ObjectType::ObjectType type, glm::vec2 location);

		void AddModule(SW::VesselModule vm);
		void RemoveModuleAt(size_t index);

		static uint32_t nextIndex;
		uint32_t nextCompartmentIndex;
		uint32_t nextChunkIndex;
		uint32_t nextModuleIndex;

		TileChunks tiles;

		vector<Player*> playersOnBoard;
	};
}

