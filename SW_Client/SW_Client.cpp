#include "SW_Client.h"
#include "Player.h"
#include "Vessel.h"
#include "TileChunks.h"
#include "NetworkReader.h"
#include "NetworkWriter.h"

#include <string>
#include <stdio.h>
#include <sstream>
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
bool swapBytes = false;
SW_Client::NetworkWriter* nw_main = new SW_Client::NetworkWriter(1 << 14, swapBytes);

namespace SW_Client
{
	void UpdateChunks(bool force, NetworkWriter* nw)
	{
		if (myPlayer)
		{
			size_t rangeT = (PLAYER_CHUNK_RANGE * 2) + 1;
			glm::ivec2 orgChunkI = myPlayer->chunkI;
			myPlayer->chunkI = TileChunks::WorldToChunkI(myPlayer->pos);

			if (force || ((orgChunkI.x != myPlayer->chunkI.x) || (orgChunkI.y != myPlayer->chunkI.y)))
			{
				nw->WriteMessageType(ClientMessageType::RequestChunk);
				uint8_t* requestLength = (uint8_t*)nw->cursor;
				nw->WriteUint8(0);


				for (size_t i = 0; i < rangeT; i++)
				{
					for (size_t j = 0; j < rangeT; j++)
					{

						glm::ivec2 diffCurr(j - PLAYER_CHUNK_RANGE, i - PLAYER_CHUNK_RANGE);
						glm::ivec2 temp(myPlayer->chunkI + diffCurr);
						glm::ivec2 diffOrg(temp - orgChunkI);

						if (force || ((abs(diffOrg.x) > PLAYER_CHUNK_RANGE) || (abs(diffOrg.y) > PLAYER_CHUNK_RANGE)))
						{
							/*std::ostringstream print;
							print << "Requesting chunk at: ";
							print << std::to_string(temp.x);
							print << ", ";
							print << std::to_string(temp.y);
							print << ")";
							PrintMessage((int)print.str().c_str());*/

							(*requestLength)++;

							TileChunk* temp2 = static_cast<TileChunk*>(currentVessel->tiles.TryGetChunk(glm::ivec2(temp.x, temp.y)));

							nw->WriteInt16((int16_t)temp.x);
							nw->WriteInt16((int16_t)temp.y);


							if (temp2)
							{
								temp2->seen = true;
								nw->WriteUint32((TileChunkVersion_t)temp2->version);
							}
							else
							{
								nw->WriteUint32((TileChunkVersion_t)4294967295);
							}
						}
					}
				}

				std::ostringstream print;
				print << "post chunk request nw properties: (Position: ";
				print << std::to_string(nw->Position());
				print << ").";
				PrintMessage((int)print.str().c_str());

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
	void FlushBuffer(NetworkWriter* nw)
	{
		if (nw->Position() > 0)
		{
			SendMessage((int)nw->buffer, nw->Position());
			nw->Reset();
		}
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
		PrintMessage((int)"Making vessel active");

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


		UpdateChunks(true, nw_main);
	}
}

extern "C" void OnConnectionMade()
{
	glClearColor(0.0f, 0.0f, 0.2f, 1.0f);

}

extern "C" void HandleMessage(int dPtr, int length)
{
	glClearColor(0.0f, 0.2f, 0.0f, 1.0f);

	PrintMessage((int)"Handling Message");
	//std::string slength = std::to_string(length);
	//std::string sdPtr = std::to_string(dPtr);
	//PrintMessage((int)slength.c_str());
	//PrintMessage((int)sdPtr.c_str());

	SW_Client::NetworkReader nr = SW_Client::NetworkReader((void*)dPtr, length, swapBytes);
	
	nw_main->Reset();

	uint32_t ecVar;

	while (nr.Remaining() > 0)
	{
		MessageType_t messageType = nr.ReadMessageType();
		switch (messageType) {
		case ServerMessageType::PingMessage:
			PrintMessage((int)"PingMessage message received");
			SW_Client::RespondToPingMessage(&nr, nw_main);
			break;
		case ServerMessageType::SetChunk:
			PrintMessage((int)"SetChunk message received");
			if (currentVessel)
			{
				currentVessel->ReadSetChunkMessage(&nr, nw_main);
			}
			break;
		case ServerMessageType::MakeVesselActive:
			PrintMessage((int)"MakeVesselActive message received");
			ReadMakeVesselActiveMessage(&nr, nw_main);
			break;
		case ServerMessageType::EndianessCheck:
			PrintMessage((int)"EndianessCheck message received");
			ecVar = nr.ReadUint32();
			nw_main->flipped = nr.swapped = swapBytes = ecVar != (uint32_t)(255 << 16);
			if (swapBytes)
			{
				PrintMessage((int)"swapBytes set to true");
				std::ostringstream print;
				print << "ecVar: ";
				print << ecVar;
				PrintMessage((int)print.str().c_str());
			}
			else
			{
				PrintMessage((int)"swapBytes set to false");
			}
			break;
		default:
			PrintMessage((int)"Unrecognized message received");
			return;
		}
	}

	SW_Client::FlushBuffer(nw_main);
}

extern "C" void HandleClose()
{
	glClearColor(0.2f, 0.0f, 0.0f, 1.0f);
}

extern "C" void Update()
{
	glClear(GL_COLOR_BUFFER_BIT);

	if (currentVessel && myPlayer)
	{
		SW_Client::UpdateChunks(false, nw_main);

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

	SDL_GL_SwapBuffers();
}
