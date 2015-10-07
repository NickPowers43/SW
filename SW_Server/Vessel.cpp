#include "stdafx.h"
#include "Vessel.h"
#include "Player.h"
#include <SW\TileChunk.h>
#include <SW\NetworkWriter.h>
#include <SW\NetworkReader.h>
#include <SW\VesselModule.h>

using namespace SW;

namespace SW_Server
{
	uint32_t Vessel::nextIndex = 0;

	Vessel::Vessel(VesselIndex_t index, glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 rot, Vessel* next) :
		SW::Vessel(index, vel, m, pos, rot),
		SW::LinkedListNode<Vessel>(next)
	{


		nextCompartmentIndex = 0;
		nextModuleIndex = 0;

		nextChunkIndex = 0;
		index = nextIndex++;
		//modifiedChunks.reserve(10);
	}


	Vessel::~Vessel()
	{
	}

	void Vessel::ReadRequestTilesMessage(Player* player, NetworkWriter* nw, NetworkReader* nr)
	{
		glm::ivec2 origin;
		glm::ivec2 size;

		origin.x = nr->ReadInt32();
		origin.y = nr->ReadInt32();
		size.x = nr->ReadInt32();
		size.y = nr->ReadInt32();

		if (!size.x || !size.y)
		{
			AABBi aabb = tiles.GetAABB();
			origin = aabb.bl;
			size = aabb.tr - aabb.bl;
		}

		nw->Write(ServerMessageType::SetTiles);
		nw->Write((int32_t)origin.x);
		nw->Write((int32_t)origin.y);
		nw->Write((int32_t)size.x);
		int32_t* sizeYLocation = (int32_t*)nw->cursor;
		nw->Write((int32_t)0);

		glm::ivec2 end(origin + size);

		for (int i = origin.y; i < end.y; i++)
		{
			if (nw->Remaining() < (TILE_MESSAGE_SIZE * size.x))
			{
				*sizeYLocation = end.y - i;
				origin.y = i;
				player->FlushBuffer(nw);

				nw->Write(ServerMessageType::SetTiles);
				nw->Write((int32_t)origin.x);
				nw->Write((int32_t)origin.y);
				nw->Write((int32_t)size.x);
				sizeYLocation = (int32_t*)nw->cursor;
				nw->Write((int32_t)0);
			}

			for (int j = origin.x; j < end.x; j++)
			{
				SW::Tile* tile = tiles.TryGet(glm::ivec2(j, i));
				if (tile)
				{
					nw->Write(tile->flags);
					nw->Write(tile->wallMask);
					nw->Write(tile->floor0);
					nw->Write(tile->floor1);
				}
				else
				{
					nw->Write((TileFlag_t)0);
					nw->Write((WallTypeMask_t)0);
					nw->Write((FloorType_t)0);
					nw->Write((FloorType_t)0);
				}
			}
		}

		*sizeYLocation = end.y - origin.y;
		player->FlushBuffer(nw);
	}
	void Vessel::ReadModuleRequestMessage(Player* player, NetworkWriter* nw, NetworkReader* nr)
	{
		nw->Write((MessageType_t)ServerMessageType::SetModule);
		uint16_t* response_count = (uint16_t*)nw->cursor;
		nw->Write((uint16_t)0);

		uint16_t module_count = nr->ReadUint16();
		size_t j = 0;
		for (size_t i = 0; i < module_count; i++)
		{
			uint32_t id = nr->ReadUint32();

			if (modules[j].id != id)
			{
				if (nw->Remaining() < sizeof(uint32_t))
				{
					//start a new message
					player->FlushBuffer(nw);
					nw->Write((MessageType_t)ServerMessageType::SetModule);
					response_count = (uint16_t*)nw->cursor;
					nw->Write((uint16_t)0);
				}

				//tell client to delete this one
				(*response_count)++;
				nw->Write((int32_t)(-((int32_t)i)));
				i++;
			}
			else
			{
				//skip this one it is up to date
				j++;
			}
		}
		response_count = (uint16_t*)nw->cursor;
		nw->Write((uint16_t)0);
		for (size_t i = j; i < modules.size(); i++)
		{
			if (nw->Remaining() < MAX_MODULE_MESSAGE_SIZE)
			{
				//start a new message
				player->FlushBuffer(nw);
				nw->Write((MessageType_t)ServerMessageType::SetModule);
				nw->Write((uint16_t)0);//zero deletions
				response_count = (uint16_t*)nw->cursor;
				nw->Write((uint16_t)0);
			}

			//tell client to add this one
			(*response_count)++;
			modules[i].WriteSetModuleMessage(nw);
		}
	}

	void Vessel::BroadcastPlayerAddition(Player* player, NetworkWriter* nw)
	{
		//flush the current messages and start creating a broadcast message
		nw->Reset();
		nw->Write((MessageType_t)ServerMessageType::AddPlayer);
		nw->Write((PlayerID_t)player->id);
		nw->Write((float)player->pos.x);
		nw->Write((float)player->pos.z);

		std::for_each(playersOnBoard.begin(), playersOnBoard.end(), [nw](std::pair<const PlayerID_t, Player*> & pair) {
			pair.second->JustSendBuffer(nw);
		});
	}
	size_t Vessel::GetOnBoardPlayerInformationMessageSize()
	{
		return (sizeof(MessageType_t) + sizeof(VesselIndex_t) + sizeof(uint16_t) + ((playersOnBoard.size() + 1) * 2 * sizeof(float)));
	}
	void Vessel::SendMakeVesselActive(Player* player, NetworkWriter* nw)
	{
		nw->Reset();
		nw->Write((MessageType_t)ServerMessageType::MakeVesselActive);
		nw->Write((VesselIndex_t)index);
		nw->Write((uint16_t)playersOnBoard.size());
		std::for_each(playersOnBoard.begin(), playersOnBoard.end(), [nw](std::pair<const PlayerID_t, Player*> & pair) {
			nw->Write((PlayerID_t)pair.second->id);
			nw->Write((float)pair.second->pos.x);
			nw->Write((float)pair.second->pos.z);
		});
		//write information for yourself
		nw->Write((PlayerID_t)player->id);
		nw->Write((float)player->pos.x);
		nw->Write((float)player->pos.z);

		player->FlushBuffer(nw);
	}
	void Vessel::AddPlayerVessel(Player* player, NetworkWriter* nw, glm::vec2 position)
	{
		player->pos = glm::vec3(position.x, 0.0f, position.y);
		player->currentVessel = this;

		//tell onBoardPlayers to add player
		BroadcastPlayerAddition(player, nw);

		SendMakeVesselActive(player, nw);
		
		playersOnBoard[player->id] = player;
	}
	void Vessel::RemovePlayer(Player* player)
	{
		if (playersOnBoard.count(player->id))
		{
			playersOnBoard.erase(player->id);

			//std::for_each(playersOnBoard.begin(), playersOnBoard.end(), [](std::pair<PlayerID_t, Player*> & pair) {
			//	//tell pair.second player has left vessel
			//});
		}

		//uint16_t id = (1 << 17) - 1;
		//for (size_t i = 0; i < playersOnBoard.size(); i++)
		//{
		//	if (player == playersOnBoard[i])
		//	{
		//		id = i;
		//		break;
		//	}
		//}
		//if (id != ((1 << 17) - 1))
		//{
		//	for (size_t i = id; i < playersOnBoard.size() - 1; i++)
		//	{
		//		playersOnBoard[i] = playersOnBoard[i + 1];
		//	}
		//	playersOnBoard.pop_back();

		//	player->currentVessel = NULL;

		//	//if (GameManager.Instance.Initialized) {
		//	//	nw->SeekZero();
		//	//	nw->Write((ushort)ServerMessageType.RemovePlayer);
		//	//	nw->Write((ushort)id);
		//	//	for (int i = 0; i < playersOnBoard.Count; i++) {
		//	//		//send
		//	//	}
		//	//}
		//}
	}
	void Vessel::BuildModule(VMType_t type, glm::ivec2 position)
	{
		VesselModuleTemplate* typeTemplate = vesselModuleTemplates[type];

		for (size_t i = 0; i < modules.size(); i++)
		{
			if (true)
			{

				return;
			}
		}
	}
	void Vessel::Step(NetworkWriter* nw)
	{
		std::for_each(playersOnBoard.begin(), playersOnBoard.end(), [nw](std::pair<const PlayerID_t, Player*> & pair) {
			pair.second->Step();

			nw->Reset();
			pair.second->WriteUpdateMessage(nw);
			pair.second->FlushBuffer(nw);
		});
	}

	void Vessel::PlaceObject(ObjectType_t type, glm::ivec2 location)
	{
		SW::Tile* tile = tiles.TryGet(location);

		if (!tile)
		{
			tile = new SW::Tile();
			tiles.Set(location, tile);
		}

		tile->object = type;
	}

	void Vessel::AddModule(SW::VesselModule vm)
	{
		SW::Vessel::AddModule(vm);

	}
	void Vessel::RemoveModuleAt(size_t index)
	{
		VesselModule* rVM = &modules[index];



		SW::Vessel::RemoveModuleAt(index);
	}
}
