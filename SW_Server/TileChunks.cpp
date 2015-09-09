#include "stdafx.h"
#include "TileChunks.h"
#include "TileChunk.h"
#include "Player.h"
#include <SW\NetworkWriter.h>
#include <SW\NetworkReader.h>
#include <SW\CompartmentTile.h>



namespace SW_Server
{
	TileChunks::TileChunks()
	{
	}


	TileChunks::~TileChunks()
	{
	}

	SW::TileChunk* TileChunks::CreateChunkAt(glm::ivec2 index, SW::TileChunkVersion_t version)
	{
		return new TileChunk(index, version);
	}

	void TileChunks::ReadChunkRequestMessage(Player* player, NetworkWriter* nw, NetworkReader* nr)
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

			TileChunk* vc = static_cast<TileChunk*>(TryGetChunk(glm::ivec2(x, y)));
			if (vc)
			{
				if (version != vc->version)
				{
					if (nw->Remaining() < SW::MAX_VESSELCHUNK_MESSAGE_SIZE)
					{
						//start a new message
						player->FlushBuffer(nw);
						nw->Reset();
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
	void TileChunks::MarkTileAsModified(glm::ivec2 index)
	{
		TileChunk* vc = static_cast<TileChunk*>(TryGetChunk(TileIToChunkI(index)));

		if (modifiedChunks.size() != 0) {
			for (int i = 0; i < modifiedChunks.size(); i++) {
				if (modifiedChunks[i] == vc)
				{
					vc = modifiedChunks[0];
					modifiedChunks[0] = modifiedChunks[i];
					modifiedChunks[i] = vc;
					return;
				}
			}

			int j = modifiedChunks.size();
			modifiedChunks.push_back(vc);
			vc = modifiedChunks[0];
			modifiedChunks[0] = modifiedChunks[j];
			modifiedChunks[j] = vc;
		}
		else {
			modifiedChunks.push_back(vc);
		}
	}
}
