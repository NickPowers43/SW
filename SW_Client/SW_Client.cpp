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
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "BufferedMeshArray.h"
#include <emscripten\html5.h>
#include <SW/CompartmentTile.h>

using namespace SW_Client;

uint8_t* keyStates = NULL;
Player* myPlayer = NULL;
Vessel* currentVessel = NULL;
SW::Compartment* playerComp;
std::map<VesselID_t, Vessel*> activeVessels;
std::map<VesselID_t, Vessel*> vessels;
std::map<PlayerID_t, Player*> players;
bool swapBytes = false;
NetworkWriter* nw_main = new NetworkWriter(1 << 14, swapBytes);

bool fMeshCreated = false;
int fIndicesCount = 0;
GLuint floorVBuffer = 0;
GLuint floorIBuffer = 0;

bool pointerLockRequested = false;
bool pointerLocked = false;

EM_BOOL myMouseMoveCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
	/*if (!pointerLockRequested && !pointerLocked)
	{
	PrintMessage((int)"Requesting pointer lock");
	emscripten_request_pointerlock("canvas", true);
	pointerLockRequested = true;
	}*/

	Vessel* v = (Vessel*)userData;

	myPlayer->rot.x += (0.006f) * (float)mouseEvent->movementX;
	myPlayer->rot.y += (0.006f) * (float)mouseEvent->movementY;

	myPlayer->rot.y = glm::clamp(myPlayer->rot.y, -glm::half_pi<float>(), glm::half_pi<float>());

	return 0;
}

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
	void StepCurrentVessel()
	{
		if (myPlayer)
		{
			myPlayer->StepXZ();

			if (keyStates[SDLK_i])
			{
				//command ship
			}
			else if (keyStates[SDLK_b])
			{
				//build ship
			}
			else
			{


			}

			if (keyStates[SDLK_o])
			{
				camera.zoom -= 0.001f;
			}
			if (keyStates[SDLK_p])
			{
				camera.zoom += 0.001f;
			}
			//myPlayer->pos = glm::vec2(glm::cos(elapsedTime * 0.25f), glm::sin(elapsedTime * 0.5f)) * 11.0f;
			//StepChunks(false, nw);
		}

		DrawWorld();
	}
	void DrawWorld()
	{
		if (myPlayer)
		{
			/*camera.pos = glm::vec3(glm::cos(elapsedTime * 0.5f), 0.0f, glm::sin(elapsedTime * 0.5f));
			camera.pos *= glm::cos(glm::pi<float>() * 0.2f);
			camera.pos.y = glm::sin(glm::pi<float>() * 0.2f);
			camera.pos *= 14.0f;
			camera.rot = glm::vec2(-(glm::pi<float>() * 0.5f) - (elapsedTime * 0.5f), glm::pi<float>() * 0.25f);*/

			camera.pos = glm::vec3(myPlayer->pos.x, 1.0f, myPlayer->pos.z);
			camera.rot = glm::vec2(myPlayer->rot.x, myPlayer->rot.y);

			glm::mat4 viewMat(1.0f);
			camera.GenerateView(viewMat);
			glm::mat4 projMat(1.0f);
			camera.GenerateProjection(projMat);

			float lightIntensity = 1.0f;
			glm::vec3 lightPosition(glm::cos(elapsedTime), 0.5f, glm::sin(elapsedTime));

			glUseProgram(litColoredVertexProgram.program);
			glUniform1f(litColoredVertexProgram.lightIntensity, lightIntensity);
			glUniform3fv(litColoredVertexProgram.lightPosition, 1, glm::value_ptr(lightPosition));
			glUniformMatrix4fv(litColoredVertexProgram.viewMat, 1, false, glm::value_ptr(viewMat));
			glUniformMatrix4fv(litColoredVertexProgram.projMat, 1, false, glm::value_ptr(projMat));

			wallMeshes->Bind();
			glVertexAttribPointer(litColoredVertexProgram.posAttrib, 4, GL_FLOAT, false, 48, (GLvoid*)0);
			glVertexAttribPointer(litColoredVertexProgram.normalAttrib, 4, GL_FLOAT, false, 48, (GLvoid*)16);
			glVertexAttribPointer(litColoredVertexProgram.colorAttrib, 4, GL_FLOAT, false, 48, (GLvoid*)32);

			DrawTiles(dynamic_cast<SW::TileSet*>(&currentVessel->tiles), currentVessel->tiles.GetAABB());

			if (floorVBuffer)
			{
				glUseProgram(floorProgram.program);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, floorProgram.texture);
				glUniform1i(floorProgram.textureLoc, 0);

				glUniformMatrix4fv(floorProgram.viewMat, 1, false, glm::value_ptr(viewMat));
				glUniformMatrix4fv(floorProgram.projMat, 1, false, glm::value_ptr(projMat));

				glBindBuffer(GL_ARRAY_BUFFER, floorVBuffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorIBuffer);

				glVertexAttribPointer(floorProgram.posAttrib, 2, GL_FLOAT, false, 16, NULL);
				glEnableVertexAttribArray(floorProgram.posAttrib);
				glVertexAttribPointer(floorProgram.uvAttrib, 2, GL_FLOAT, false, 16, (GLvoid*)8);
				glEnableVertexAttribArray(floorProgram.uvAttrib);

				glDrawElements(GL_TRIANGLES, fIndicesCount, GL_UNSIGNED_INT, 0);
			}


			/*glUseProgram(litColoredVertexProgram.program);
			glm::vec4 color(0.5f, 0.5f, 0.5f, 1.0f);
			glUniform4fv(litColoredVertexProgram.color, 1, glm::value_ptr(color));
			glUniformMatrix4fv(litColoredVertexProgram.viewMat, 1, false, glm::value_ptr(viewMat));

			DrawWalls();

			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glUseProgram(shadowProgram.program);
			glUniform2fv(shadowProgram.playerPos, 1, glm::value_ptr(camera.position));
			glUniformMatrix4fv(shadowProgram.viewMat, 1, false, glm::value_ptr(viewMat));

			DrawShadows();

			glEnable(GL_CULL_FACE);
			glDisable(GL_BLEND);*/
		}
	}
	int GetWallMeshIndex(WallType_t type)
	{
		switch (type)
		{
		case WallType::OneByZero:
		case WallType::ZeroByOne:
			return 0;
		case WallType::OneByOne:
		case WallType::OneByOneFlipped:
			return 1;
		case WallType::TwoByOne:
		case WallType::TwoByOneFlipped:
		case WallType::OneByTwo:
		case WallType::OneByTwoFlipped:
			return 2;
		default:
			return -1;
		}
	}
	void DrawWallRight(WallType_t type, int x, int z)
	{
		glm::mat4 objMat(1.0f);
		objMat = glm::translate(objMat, glm::vec3(x, 0.0f, z));
		objMat = glm::rotate(objMat, SW::wallRotations[type], glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(litColoredVertexProgram.objMat, 1, false, glm::value_ptr(objMat));

		int wallMesh = GetWallMeshIndex(type);
		wallMeshes->meshes[wallMesh * 2].Draw();
	}
	void DrawWallLeft(WallType_t type, int x, int z)
	{
		glm::mat4 objMat(1.0f);
		objMat = glm::translate(objMat, glm::vec3(x, 0.0f, z));
		objMat = glm::rotate(objMat, SW::wallRotations[type] + glm::pi<float>(), glm::vec3(0.0f, -1.0f, 0.0f));
		objMat = glm::scale(objMat, glm::vec3(-1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(litColoredVertexProgram.objMat, 1, false, glm::value_ptr(objMat));

		int wallMesh = GetWallMeshIndex(type);
		wallMeshes->meshes[(wallMesh * 2) + 1].Draw();
	}
	void DrawTiles(SW::TileSet* ts, SW::AABBi region)
	{
		for (int i = region.bl.y; i < region.tr.y; i++)
		{
			for (int j = region.bl.x; j < region.tr.x; j++)
			{
				glm::ivec2 tileI(j, i);
				SW::Tile* tile = ts->TryGet(tileI);

				if (tile)
				{
					//glm::vec4 offset(j, 0.0f, i, 1.0f);

					WallType_t wall0;
					WallType_t wall1;
					int wCount = tile->GetWalls(&wall0, &wall1);
					if (wCount > 0)
					{
						DrawWallRight(wall0, j, i);
						DrawWallLeft(wall0, j, i);

						if (wCount > 1)
						{
							DrawWallRight(wall1, j, i);
							DrawWallLeft(wall1, j, i);
						}
					}
				}
			}
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
			vessel = new Vessel(vIndex, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			vessels[vIndex] = vessel;
		}
		else
		{
			vessel = vessels[vIndex];
		}

		if (currentVessel)
		{
			currentVessel = NULL;
		}

		currentVessel = vessel;

		uint16_t otherPlayerCount = nr->ReadUint16();
		for (size_t i = 0; i < otherPlayerCount; i++) {
			//add the ith player
			PlayerID_t id = nr->ReadUint32();
			float x = nr->ReadSingle();
			float z = nr->ReadSingle();
			glm::vec3 pos = glm::vec3(x, 0.0f, z);
			Player* player = new Player(id, glm::vec3(0.0f, 0.0f, 0.0f), 1000.0f, pos, glm::vec3(0.0f, 0.0f, 0.0f));
			players[id] = player;
		}

		//add ourselves
		PlayerID_t id = nr->ReadUint32();
		float x = nr->ReadSingle();
		float z = nr->ReadSingle();
		glm::vec3 pos = glm::vec3(x, 0.0f, z);
		if (!myPlayer)
		{
			std::stringstream print;
			print << "creating myPlayer with id (" << id << ")";
			PrintMessage((int)print.str().c_str());
			emscripten_set_mousemove_callback(0, 0, true, myMouseMoveCallback);
			myPlayer = new Player(id, glm::vec3(0.0f, 0.0f, 0.0f), 10.0f, pos, glm::vec3(0.0f, 0.0f, 0.0f));
			players[id] = myPlayer;
		}
		else
		{
			myPlayer->pos = pos;
		}

		playerComp = currentVessel->tiles.CompartmentAt(myPlayer->posXZ());

		nw->WriteMessageType(ClientMessageType::RequestTiles);
		nw->WriteInt32(0);
		nw->WriteInt32(0);
		nw->WriteInt32(0);
		nw->WriteInt32(0);

	}
	void ReadUpdatePlayerMessage(NetworkReader* nr)
	{
		PlayerID_t id = nr->ReadUint32();

		if (players.count(id))
		{
			players[id]->ReadUpdateMessage(nr);
		}
		else
		{
			std::stringstream print;
			print << "player with id (" << id << ") not found";
			PrintMessage((int)print.str().c_str());
			nr->ReadSingle();//skip x
			nr->ReadSingle();//skip z
		}

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
	void ReadSetTilesMessage(NetworkReader* nr, NetworkWriter* nw)
	{
		glm::ivec2 origin;
		glm::ivec2 size;

		origin.x = nr->ReadInt32();
		origin.y = nr->ReadInt32();
		size.x = nr->ReadInt32();
		size.y = nr->ReadInt32();

		glm::ivec2 end(origin + size);

		SW::AABBi compRegion(origin - glm::ivec2(2, 2), end + glm::ivec2(2, 2));
		currentVessel->tiles.EraseCompartments(compRegion);

		for (int i = origin.y; i < end.y; i++)
		{
			for (int j = origin.x; j < end.x; j++)
			{
				glm::ivec2 tileI(j, i);

				SW::Tile* tile;
				if (!(tile = currentVessel->tiles.TryGet(tileI)))
				{
					tile = static_cast<SW::Tile*>(new SW::CompartmentTile());
					currentVessel->tiles.Set(tileI, static_cast<SW::Tile*>(tile));
				}

				tile->flags = nr->ReadUint16();
				tile->wallMask = nr->ReadUint8();
				tile->floor0 = nr->ReadUint8();
				tile->floor1 = nr->ReadUint8();
			}
		}

		currentVessel->tiles.RebuildCompartmentsForRegion(false, compRegion);
		//tileBuffer.RebuildCompartments();

		//for (int i = 0; i < size.y; i++)
		//{
		//	for (int j = 0; j < size.x; j++)
		//	{
		//		//compare the two tiles
		//		glm::ivec2 tileI(j, i);
		//		SW::Tile* tile = tileBuffer.TryGet(tileI);
		//		tileBuffer.Set(tileI, NULL);

		//		tiles.Set(tileI + origin, tile);
		//	}
		//}

		GenerateFloorMesh(dynamic_cast<SW::TileSet*>(&currentVessel->tiles), currentVessel->tiles.GetAABB());
		playerComp = currentVessel->tiles.CompartmentAt(myPlayer->posXZ());
	}
	void AppendQuadIndices(std::vector<MeshIndex_t> & indices, std::vector<float> & vertices, int vertexSize)
	{
		MeshIndex_t start = vertices.size() / vertexSize;
		indices.push_back(start + 0);
		indices.push_back(start + 3);
		indices.push_back(start + 1);
		indices.push_back(start + 0);
		indices.push_back(start + 2);
		indices.push_back(start + 3);
	}
	void AppendShadowVertex(glm::vec2 position, float influence, float offset, float alpha, std::vector<float> & vertices)
	{
		vertices.push_back(position.x);
		vertices.push_back(position.y);
		vertices.push_back(0.0f);
		vertices.push_back(1.0f);
		vertices.push_back(influence);
		vertices.push_back(offset);
		vertices.push_back(alpha);
		vertices.push_back(0.0f);
	}
	void GenerateFloorMesh(SW::TileSet* ts, SW::AABBi region)
	{
		std::vector<float> fVertices;
		std::vector<MeshIndex_t> fIndices;

		for (int i = region.bl.y; i < region.tr.y; i++)
		{
			for (int j = region.bl.x; j < region.tr.x; j++)
			{
				glm::ivec2 tileI(j, i);
				SW::Tile* tile = ts->TryGet(tileI);

				if (tile)
				{
					glm::vec2 offset = glm::vec2(j, i);

					if (tile->floor0 || tile->floor1) {

						if (tile->floor0 == tile->floor1) {
							AppendMeshData(GetFloorMesh(tile->floor0, FloorType::None, WallType::None, 0), fVertices, fIndices, offset);
						}
						else {

							SW::Tile* lTile = ts->TryGet(glm::ivec2(j - 1, i));
							SW::Tile* rTile = ts->TryGet(glm::ivec2(j + 1, i));
							SW::Tile* r2Tile = ts->TryGet(glm::ivec2(j + 2, i));
							SW::Tile* bTile = ts->TryGet(glm::ivec2(j, i - 1));
							SW::Tile* brTile = ts->TryGet(glm::ivec2(j + 1, i - 1));

							if (tile->ContainsMask(WallTypeMask::TwoByOne)) { //this tile contains a TwoByOne
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::TwoByOne, 0), fVertices, fIndices, offset);
							}
							else if (tile->ContainsMask(WallTypeMask::OneByTwo)) { //this tile contains a OneByTwo
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::OneByTwo, 0), fVertices, fIndices, offset);
							}
							else if (tile->ContainsMask(WallTypeMask::OneByOne)) { //this tile contains a OneByOne
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::OneByOne, 0), fVertices, fIndices, offset);
							}
							else if (lTile && lTile->ContainsMask(WallTypeMask::TwoByOne)) { //left tile contains a TwoByOne
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::TwoByOne, 1), fVertices, fIndices, offset);
							}
							else if (bTile && bTile->ContainsMask(WallTypeMask::OneByTwo)) { //bottom tile contains a OneByTwo
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::OneByTwo, 1), fVertices, fIndices, offset);
							}
							else if (brTile && brTile->ContainsMask(WallTypeMask::OneByTwoFlipped)) { //br tile contains a OneByTwoFlipped
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::OneByTwoFlipped, 1), fVertices, fIndices, offset);
							}
							else if (rTile && rTile->ContainsMask(WallTypeMask::TwoByOneFlipped)) { //r tile contains a TwoByOneFlipped
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::TwoByOneFlipped, 0), fVertices, fIndices, offset);
							}
							else if (rTile && rTile->ContainsMask(WallTypeMask::OneByOneFlipped)) { //r tile contains a OneByOneFlipped
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::OneByOneFlipped, 0), fVertices, fIndices, offset);
							}
							else if (rTile && rTile->ContainsMask(WallTypeMask::OneByTwoFlipped)) { //r tile contains a OneByTwoFlipped
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::OneByTwoFlipped, 0), fVertices, fIndices, offset);
							}
							else if (r2Tile && r2Tile->ContainsMask(WallTypeMask::TwoByOneFlipped)) { //r2 tile contains a TwoByOneFlipped
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::TwoByOneFlipped, 1), fVertices, fIndices, offset);
							}
							else {
								//no walls cut this tile
								AppendMeshData(GetFloorMesh(tile->floor0, FloorType::None, WallType::None, 0), fVertices, fIndices, offset);
							}
						}
					}
				}
			}
		}

		if (!floorVBuffer)
		{
			glGenBuffers(1, &floorIBuffer);
			glGenBuffers(1, &floorVBuffer);
		}

		if (fIndices.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, floorVBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * fVertices.size(), (GLvoid*)&fVertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorIBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MeshIndex_t) * fIndices.size(), (GLvoid*)&fIndices[0], GL_STATIC_DRAW);
			fIndicesCount = fIndices.size();

			fMeshCreated = true;
		}
		else
		{
			fMeshCreated = false;
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


	//PrintMessage((int)"Handling Message");
	//std::string slength = std::to_string(length);
	//std::string sdPtr = std::to_string(dPtr);
	//PrintMessage((int)slength.c_str());
	//PrintMessage((int)sdPtr.c_str());

	NetworkReader nr = NetworkReader((char*)dPtr, length, swapBytes);
	
	nw_main->Reset();

	while (nr.Remaining() > 0)
	{
		MessageType_t messageType = nr.ReadMessageType();
		switch (messageType) {
		case ServerMessageType::PingMessage:
			RespondToPingMessage(&nr, nw_main);
			break;
		case ServerMessageType::SetTiles:
			ReadSetTilesMessage(&nr, nw_main);
			break;
		case ServerMessageType::UpdatePlayer:
			ReadUpdatePlayerMessage(&nr);
			break;
		case ServerMessageType::MakeVesselActive:
			ReadMakeVesselActiveMessage(&nr, nw_main);
			break;
		case ServerMessageType::EndianessCheck:
			ReadEndianessCheckMessage(&nr, nw_main);
			break;
		default:
			PrintMessage((int)"Unrecognized message received");
			return;
		}
	}

	FlushBuffer(nw_main);
}

extern "C" void HandleClose()
{
	glClearColor(0.2f, 0.0f, 0.0f, 1.0f);
}

using namespace SW_Client;

Uint32 old_time, current_time;

extern "C" void Login(char* username, char* password)
{
	PrintMessage((int)"Logging in as: ");
	PrintMessage((int)username);
	PrintMessage((int)password);
}

extern "C" void LoginAsGuest()
{
	PrintMessage((int)"Logging in as guest");
}

extern "C" void Update()
{

	old_time = current_time;
	current_time = SDL_GetTicks();
	deltaTime = 0.016f;// (current_time - old_time) / 1000.0f;
	elapsedTime += deltaTime;

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_KEYDOWN)
		{
			keyStates[event.key.keysym.sym] = 1;
		}
		if (event.type == SDL_KEYUP)
		{
			keyStates[event.key.keysym.sym] = 0;
		}
	}

	if (keyStates[SDLK_ESCAPE])
	{
	}

	/*SDL_PumpEvents();
	if (!(keyStates = SDL_GetKeyboardState(NULL)))
	{
		PrintMessage((int)"keyStates is NULL");
	}*/

	if (myPlayer)
	{
		nw_main->Reset();
		myPlayer->ProcessInputs(nw_main);
		FlushBuffer(nw_main);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (currentVessel)
	{
		StepCurrentVessel();
		currentVessel->Step(nw_main);
	}

	/*if (nw_main->Position() > 0)
	{
		FlushBuffer(nw_main);
	}*/

	SDL_GL_SwapBuffers();
}
