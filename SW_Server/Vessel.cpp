#include "stdafx.h"
#include "Vessel.h"
#include "Player.h"
#include "TileChunk.h"
#include <SW\NetworkWriter.h>
#include <SW\NetworkReader.h>
#include <SW\VesselModule.h>

using namespace SW;

namespace SW_Server
{
	uint32_t Vessel::nextIndex = 0;

	Vessel::Vessel(VesselIndex_t index, VesselVecType vel, VesselValueType m, VesselVecType pos, VesselValueType rot, Vessel* next) :
		SW::Vessel(index),
		SW::RigidBody<VesselValueType>(vel, m, pos, rot),
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

	void Vessel::ReadChunkRequestMessage(Player* player, NetworkWriter* nw, NetworkReader* nr)
	{
		nw->Write(ServerMessageType::SetChunk);
		uint8_t* response_count = (uint8_t*)nw->cursor;
		nw->Write((uint8_t)0);

		uint8_t chunk_count = nr->ReadUint8();
		for (size_t i = 0; i < chunk_count; i++)
		{
			int16_t x = nr->ReadInt16();
			int16_t y = nr->ReadInt16();
			uint32_t version = nr->ReadUint32();

			cout << "Client requesting chunk at (" << x << ", " << y << ")\n";

			TileChunk* vc = static_cast<TileChunk*>(tiles.TryGetChunk(glm::ivec2(x, y)));
			if (vc)
			{
				if (version != vc->version)
				{
					if (nw->Remaining() < SW::MAX_VESSELCHUNK_MESSAGE_SIZE)
					{
						//start a new message
						player->FlushBuffer(nw);
						nw->Write(ServerMessageType::SetChunk);
						response_count = (uint8_t*)nw->cursor;
						nw->Write((uint8_t)0);
					}

					(*response_count)++;
					vc->WriteSetChunkMessage(nw);
				}
			}
		}
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
		player->FlushBuffer(nw);
		nw->Write((MessageType_t)ServerMessageType::AddPlayer);
		nw->Write((float)player->pos.x);
		nw->Write((float)player->pos.y);

		for (size_t i = 0; i < playersOnBoard.size(); i++)
		{
			playersOnBoard[i]->JustSendBuffer(nw);
		}

		nw->Reset();
	}
	size_t Vessel::GetOnBoardPlayerInformationMessageSize()
	{
		return (sizeof(MessageType_t) + sizeof(VesselIndex_t) + sizeof(uint16_t) + ((playersOnBoard.size() + 1) * 2 * sizeof(float)));
	}
	void Vessel::SendMakeVesselActive(Player* player, NetworkWriter* nw)
	{
		if (nw->Remaining() < GetOnBoardPlayerInformationMessageSize())
		{
			player->FlushBuffer(nw);
		}
		nw->Write((MessageType_t)ServerMessageType::MakeVesselActive);
		nw->Write((VesselIndex_t)index);
		nw->Write((uint16_t)playersOnBoard.size());
		for (size_t i = 0; i < playersOnBoard.size(); i++)
		{
			nw->Write((float)playersOnBoard[i]->pos.x);
			nw->Write((float)playersOnBoard[i]->pos.y);
		}
		//write information for yourself
		nw->Write((float)player->pos.x);
		nw->Write((float)player->pos.y);
	}
	void Vessel::AddPlayerVessel(Player* player, NetworkWriter* nw, glm::vec2 position)
	{
		player->pos = position;
		player->currentVessel = this;

		//tell onBoardPlayers to add player
		BroadcastPlayerAddition(player, nw);

		SendMakeVesselActive(player, nw);
		
		playersOnBoard.push_back(player);
	}
	void Vessel::RemovePlayer(Player* player)
	{
		uint16_t id = (1 << 17) - 1;
		for (size_t i = 0; i < playersOnBoard.size(); i++)
		{
			if (player == playersOnBoard[i])
			{
				id = i;
				break;
			}
		}
		if (id != ((1 << 17) - 1))
		{
			for (size_t i = id; i < playersOnBoard.size() - 1; i++)
			{
				playersOnBoard[i] = playersOnBoard[i + 1];
			}
			playersOnBoard.pop_back();

			player->currentVessel = NULL;

			//if (GameManager.Instance.Initialized) {
			//	nw->SeekZero();
			//	nw->Write((ushort)ServerMessageType.RemovePlayer);
			//	nw->Write((ushort)id);
			//	for (int i = 0; i < playersOnBoard.Count; i++) {
			//		//send
			//	}
			//}
		}
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

	VesselObject* Vessel::PlaceObject(ObjectType::ObjectType type, glm::vec2 location)
	{
		VesselObject* obj = new VesselObject(type, location);

		return obj;
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
