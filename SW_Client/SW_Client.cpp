#include "SW_Client.h"
#include "Player.h"
#include "Vessel.h"
#include "TileChunks.h"
#include "NetworkReader.h"
#include "NetworkWriter.h"

#include <string>
#include <stdio.h>
#include <map>
#include <vector>
#include <GLES2/gl2.h>
#include <SDL\SDL.h>
#include "SW_Client.h"
#include "GraphicsCommon.h"

std::map<VesselIndex_t, SW_Client::Vessel*> vessels;
SW_Client::Player* myPlayer = NULL;
std::vector<SW_Client::Player*> players;
SW_Client::Vessel* currentVessel = NULL;
SW_Client::NetworkWriter* nw_main = new SW_Client::NetworkWriter(1 << 14, true);
bool swapBytes = false;

namespace SW_Client
{
	void UpdateChunks(bool force)
	{
		if (myPlayer)
		{
			size_t rangeT = (PLAYER_CHUNK_RANGE * 2) + 1;
			glm::ivec2 orgChunkI = myPlayer->chunkI;
			myPlayer->chunkI = TileChunks::WorldToChunkI(myPlayer->pos);

			if (force || ((orgChunkI.x != myPlayer->chunkI.x) || (orgChunkI.y != myPlayer->chunkI.y)))
			{
				nw_main->Reset();
				nw_main->WriteMessageType(ClientMessageType::RequestChunk);
				uint8_t* requestLength = (uint8_t*)nw_main->cursor;
				nw_main->WriteUint8(0);

				for (size_t i = 0; i < rangeT; i++)
				{
					for (size_t j = 0; j < rangeT; j++)
					{

						glm::ivec2 diffCurr(j - PLAYER_CHUNK_RANGE, i - PLAYER_CHUNK_RANGE);
						glm::ivec2 temp(myPlayer->chunkI + diffCurr);
						glm::ivec2 diffOrg(temp - orgChunkI);

						if (force || ((abs(diffOrg.x) > PLAYER_CHUNK_RANGE) || (abs(diffOrg.y) > PLAYER_CHUNK_RANGE)))
						{

							(*requestLength)++;

							TileChunk* temp2 = static_cast<TileChunk*>(currentVessel->tiles.TryGetChunk(glm::ivec2(temp.x, temp.y)));

							nw_main->WriteInt16(temp.x);
							nw_main->WriteInt16(temp.y);

							if (temp2)
							{
								temp2->seen = true;
								nw_main->Write((TileChunkVersion_t)temp2->version);
							}
							else
							{
								nw_main->Write((TileChunkVersion_t)(0xffffffffffffffff));
							}
						}
					}
				}
				if (requestLength > 0)
				{
					printf("Requesting chunks from server");
				}

				//destroy chunks that are no longer visible
				for (size_t i = 0; i < currentVessel->tiles.chunks.dim.y; i++)
				{
					for (size_t j = 0; j < currentVessel->tiles.chunks.dim.x; j++)
					{
						glm::ivec2 chunkI(currentVessel->tiles.chunks.origin + glm::ivec2(j, i));
						SW::TileChunk* swChunk = currentVessel->tiles.TryGetChunk(glm::ivec2(chunkI.x, chunkI.y));
						if (swChunk)
						{
							SW_Client::TileChunk* chunk = static_cast<SW_Client::TileChunk*>(swChunk);
							if (chunk->seen)
							{
								currentVessel->InstantiateChunk(chunk);
							}
							else
							{
								chunk->Destroy();
							}

							chunk->seen = false;
						}
					}
				}
			}
		}
	}

	int ClearCurrentVessel()
	{
		for (size_t i = 0; i < players.size(); i++)
		{
			delete players[i];
		}
		players.clear();

		currentVessel = NULL;

		return 0;
	}

	void RespondToPingMessage(NetworkReader* nr, NetworkWriter* nw)
	{
		nw->WriteMessageType(ClientMessageType::PingMessageResponse);
		uint32_t messageLength = nr->ReadUint32();
		for (size_t i = 0; i < messageLength; i++)
		{
			nr->ReadUint8();
		}
		nw->WriteUint32(messageLength);
	}

	void ReadMakeVesselActiveMessage(NetworkReader* nr, NetworkWriter* nw)
	{
		VesselIndex_t vIndex = nr->ReadUint32();

		Vessel* vessel = NULL;

		if (!vessels.count(vIndex))
		{
			printf("Creating vessel");
			vessel = new Vessel(vIndex);
			vessels[vIndex] = vessel;
		}
		else
		{
			vessel = vessels[vIndex];
		}

		if (currentVessel)
		{
			if (ClearCurrentVessel())
			{
				//disconnect immediately
			}
		}

		currentVessel = vessel;

		uint16_t otherPlayerCount = nr->ReadUint16();
		for (size_t i = 0; i < otherPlayerCount; i++) {
			//add the ith player
			float x = nr->ReadSingle();
			float y = nr->ReadSingle();
			glm::vec2 pos = glm::vec2(x, y);
			players.push_back(new Player(glm::vec2(0.0f, 0.0f), 1.0f, pos, 0.0f, SW::TileChunks::WorldToChunkI(pos)));
		}

		//add ourselves
		float x = nr->ReadSingle();
		float y = nr->ReadSingle();
		glm::vec2 pos = glm::vec2(x, y);

		if (!myPlayer)
		{
			myPlayer = new Player(glm::vec2(0.0f, 0.0f), 1.0f, pos, 0.0f, SW::TileChunks::WorldToChunkI(pos));
		}
		else
		{
			myPlayer->pos = pos;
			myPlayer->chunkI = SW::TileChunks::WorldToChunkI(pos);
		}


		UpdateChunks(true);
	}
}

extern "C" void Update()
{
	printf("Update called");

	if (currentVessel && myPlayer)
	{
		SW_Client::UpdateChunks(false);

		for (size_t i = 0; i < currentVessel->tiles.chunks.dim.y; i++)
		{
			for (size_t j = 0; j < currentVessel->tiles.chunks.dim.x; j++)
			{
				glm::ivec2 chunkI(currentVessel->tiles.chunks.origin + glm::ivec2(j, i));
				SW::TileChunk* swChunk = currentVessel->tiles.TryGetChunk(glm::ivec2(chunkI.x, chunkI.y));
				if (swChunk)
				{
					SW_Client::TileChunk* chunk = static_cast<SW_Client::TileChunk*>(swChunk);
					if (chunk->instantiated)
					{
						//chunk->Draw();
					}
				}
			}
		}
	}

	glClear(GL_COLOR_BUFFER_BIT);
	PrintRefresh();
	SDL_GL_SwapBuffers();
}
