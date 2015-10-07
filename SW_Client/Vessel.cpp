#include "Vessel.h"
#include "SW_Client.h"
#include <SW/CompartmentTile.h>
#include <SW/FixedCompartmentTileSet.h>
#include <sstream>
#include <SW/Tile.h>
#include <SW/SW.h>
#include <SW/AABBi.h>
#include "Player.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL/SDL_input.h>
#include <SDL/SDL_scancode.h>
#include <SDL/SDL_events.h>
#include "BufferedMeshArray.h"
#include <emscripten\html5.h>

bool pointerLockRequested = false;
bool pointerLocked = false;

EM_BOOL key_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData)
{
	if (eventType == EMSCRIPTEN_EVENT_KEYPRESS && (!strcmp(e->key, "l") || e->which == 112)) {
		EmscriptenPointerlockChangeEvent plce;
		EMSCRIPTEN_RESULT ret = emscripten_get_pointerlock_status(&plce);
		if (!plce.isActive) {
			PrintMessage((int)"emscripten_request_pointerlock");
			ret = emscripten_request_pointerlock(0, 1);
		}
		else {
			ret = emscripten_exit_pointerlock();
			ret = emscripten_get_pointerlock_status(&plce);
			if (plce.isActive) {
				PrintMessage((int)"Pointer lock exit did not work!\n");
			}
		}
	}

	return 0;
}

EM_BOOL pointerlockchange_callback_func(int eventType, const EmscriptenPointerlockChangeEvent *pointerlockChangeEvent, void *userData)
{
	pointerLocked = pointerlockChangeEvent->isActive;

	pointerLockRequested = !pointerLocked;

	return 0;

}

EM_BOOL myMouseMoveCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
	/*if (!pointerLockRequested && !pointerLocked)
	{
		PrintMessage((int)"Requesting pointer lock");
		emscripten_request_pointerlock("canvas", true);
		pointerLockRequested = true;
	}*/

	SW_Client::Vessel* v = (SW_Client::Vessel*)userData;

	v->myPlayer->rot.x += (0.006f) * (float)mouseEvent->movementX;
	v->myPlayer->rot.y += (0.006f) * (float)mouseEvent->movementY;

	v->myPlayer->rot.y = glm::clamp(v->myPlayer->rot.y, -glm::half_pi<float>(), glm::half_pi<float>());

	return 0;
}

namespace SW_Client
{
	Vessel::Vessel(VesselIndex_t index, glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 rot) : SW::Vessel(index, vel, m, pos, rot)
	{
		myPlayer = NULL;
		playerComp = NULL;
		fMeshCreated = false;
		floorVBuffer = 0;
		floorIBuffer = 0;

	}


	Vessel::~Vessel()
	{
		if (floorVBuffer)
		{
			glDeleteBuffers(1, &floorIBuffer);
			glDeleteBuffers(1, &floorVBuffer);
			fMeshCreated = false;
		}
	}

	void Vessel::AddMyPlayer(Player* player, NetworkWriter* nw)
	{
		if (player)
		{
			myPlayer = player;
			playerComp = tiles.CompartmentAt(myPlayer->posXZ());

			nw->WriteMessageType(ClientMessageType::RequestTiles);
			nw->WriteInt32(0);
			nw->WriteInt32(0);
			nw->WriteInt32(0);
			nw->WriteInt32(0);

			//emscripten_set_keydown_callback(0, 0, 1, key_callback);
			//emscripten_set_pointerlockchange_callback(NULL, NULL, true, pointerlockchange_callback_func);
			emscripten_set_mousemove_callback(0, this, true, myMouseMoveCallback);
			//emscripten_request_pointerlock("canvas", true);
		}
		else
		{
			PrintMessage((int)"NULL player added");
		}
	}
	void Vessel::ProcessInputs(NetworkWriter* nw)
	{
		if (myPlayer)
		{
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
		}
	}
	void Vessel::Step(NetworkWriter* nw)
	{
		StepXZ();

		if (myPlayer)
		{
			ProcessInputs(nw);
			myPlayer->StepXZ();

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
	void Vessel::Clear()
	{
		/*for (size_t i = 0; i < players.size(); i++)
		{
			delete players[i];
		}
		players.clear();*/

		myPlayer = NULL;
	}

	void Vessel::DrawWorld()
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

			DrawTiles(dynamic_cast<SW::TileSet*>(&tiles), tiles.GetAABB());

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
	void Vessel::DrawTiles(SW::TileSet* ts, SW::AABBi region)
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
	//point between vectors if considering CCW convention

	
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
	void Vessel::GenerateFloorMesh(SW::TileSet* ts, SW::AABBi region)
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
		}
		else
		{
			fMeshCreated = false;
		}

	}

	void Vessel::ReadSetTilesMessage(NetworkReader* nr, NetworkWriter* nw)
	{
		glm::ivec2 origin;
		glm::ivec2 size;

		origin.x = nr->ReadInt32();
		origin.y = nr->ReadInt32();
		size.x = nr->ReadInt32();
		size.y = nr->ReadInt32();

		glm::ivec2 end(origin + size);

		SW::AABBi compRegion(origin - glm::ivec2(2, 2), end + glm::ivec2(2, 2));
		tiles.EraseCompartments(compRegion);

		for (int i = origin.y; i < end.y; i++)
		{
			for (int j = origin.x; j < end.x; j++)
			{
				glm::ivec2 tileI(j, i);

				SW::Tile* tile;
				if (!(tile = tiles.TryGet(tileI)))
				{
					tile = static_cast<SW::Tile*>(new SW::CompartmentTile());
					tiles.Set(tileI, static_cast<SW::Tile*>(tile));
				}

				tile->flags = nr->ReadUint16();
				tile->wallMask = nr->ReadUint8();
				tile->floor0 = nr->ReadUint8();
				tile->floor1 = nr->ReadUint8();
			}
		}

		tiles.RebuildCompartmentsForRegion(false, compRegion);
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

		GenerateFloorMesh(dynamic_cast<SW::TileSet*>(&tiles), tiles.GetAABB());
		playerComp = tiles.CompartmentAt(myPlayer->posXZ());
	}
}