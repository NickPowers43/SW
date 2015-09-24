#include "SW_Client.h"
#include <sstream>
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
SW_Client::Vessel* currentVessel = NULL;
bool swapBytes = false;
SW_Client::NetworkWriter* nw_main = new SW_Client::NetworkWriter(1 << 14, swapBytes);

namespace SW_Client
{
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
			currentVessel->Clear();
			currentVessel = NULL;
		}

		currentVessel = vessel;

		uint16_t otherPlayerCount = nr->ReadUint16();
		for (size_t i = 0; i < otherPlayerCount; i++) {
			//add the ith player
			float x = nr->ReadSingle();
			float y = nr->ReadSingle();
			glm::vec2 pos = glm::vec2(x, y);
			//players.push_back(new Player(glm::vec2(0.0f, 0.0f), 1.0f, pos, 0.0f, SW::TileChunks::WorldToChunkI(pos)));
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

		currentVessel->AddMyPlayer(myPlayer, nw_main);
	}

	void ReadEndianessCheckMessage(NetworkReader* nr, NetworkWriter* nw)
	{
		uint32_t ecVar = nr->ReadUint32();
		nw_main->flipped = nr->swapped = swapBytes = ecVar != (uint32_t)(255 << 16);
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

	SW_Client::NetworkReader nr = SW_Client::NetworkReader((char*)dPtr, length, swapBytes);
	
	nw_main->Reset();

	while (nr.Remaining() > 0)
	{
		MessageType_t messageType = nr.ReadMessageType();
		switch (messageType) {
		case ServerMessageType::PingMessage:
			//PrintMessage((int)"PingMessage message received");
			SW_Client::RespondToPingMessage(&nr, nw_main);
			break;
		case ServerMessageType::SetTiles:
			//PrintMessage((int)"SetChunk message received");
			currentVessel->ReadSetTilesMessage(&nr, nw_main);
			break;
		case ServerMessageType::MakeVesselActive:
			//PrintMessage((int)"MakeVesselActive message received");
			ReadMakeVesselActiveMessage(&nr, nw_main);
			break;
		case ServerMessageType::EndianessCheck:
			//PrintMessage((int)"EndianessCheck message received");
			SW_Client::ReadEndianessCheckMessage(&nr, nw_main);
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

	deltaTime = 0.005f;
	elapsedTime += deltaTime;

	SDL_PumpEvents();
	if (!(SW_Client::keyStates = SDL_GetKeyboardState(NULL)))
	{
		PrintMessage((int)"keyStates is NULL");
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (currentVessel)
	{
		currentVessel->Update(nw_main);
	}

	SDL_GL_SwapBuffers();
}
