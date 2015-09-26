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

namespace SW_Client
{
	Vessel::Vessel(VesselIndex_t index) : SW::Vessel(index)
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
			playerComp = tiles.CompartmentAt(myPlayer->pos);

			nw->WriteMessageType(ClientMessageType::RequestTiles);
			nw->WriteInt32(0);
			nw->WriteInt32(0);
			nw->WriteInt32(0);
			nw->WriteInt32(0);
		}
		else
		{
			PrintMessage((int)"NULL player added");
		}
	}
	void Vessel::Update(NetworkWriter* nw)
	{
		if (myPlayer)
		{
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				/*if (event.type == SDL_EventType::)
				{

				}*/
				if (event.key.keysym.sym == SDLK_w)
				{
					myPlayer->pos += glm::vec2(0.0f, 0.02f);
				}
				if (event.key.keysym.sym == SDLK_s)
				{
					myPlayer->pos += glm::vec2(0.0f, -0.02f);
				}
				if (event.key.keysym.sym == SDLK_a)
				{
					myPlayer->pos += glm::vec2(-0.02f, 0.0f);
				}
				if (event.key.keysym.sym == SDLK_d)
				{
					myPlayer->pos += glm::vec2(0.02f, 0.0f);
				}
				if (event.key.keysym.sym == SDLK_o)
				{
					camera.zoom -= 0.001f;
				}
				if (event.key.keysym.sym == SDLK_p)
				{
					camera.zoom += 0.001f;
				}
			}
			/*if (keyStates[SDL_SCANCODE_W])
				myPlayer->pos += glm::vec2(0.0f, 0.01f);
			if (keyStates[SDL_SCANCODE_S])
				myPlayer->pos += glm::vec2(0.0f, -0.01f);
			if (keyStates[SDL_SCANCODE_A])
				myPlayer->pos += glm::vec2(-0.01f, 0.0f);
			if (keyStates[SDL_SCANCODE_D])
				myPlayer->pos += glm::vec2(0.01f, 0.0f);

			if (keyStates[SDL_SCANCODE_KP_MEMADD])
				camera.zoom -= 0.001f;
			if (keyStates[SDL_SCANCODE_KP_MEMSUBTRACT])
				camera.zoom += 0.001f;*/
			//myPlayer->pos = glm::vec2(glm::cos(elapsedTime * 0.25f), glm::sin(elapsedTime * 0.5f)) * 11.0f;
			//UpdateChunks(false, nw);
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
			camera.pos = glm::vec3(glm::cos(elapsedTime * 0.5f), 0.0f, glm::sin(elapsedTime * 0.5f));
			camera.pos *= 0.86f;
			camera.pos.y = 0.86f;
			camera.pos *= 11.0f;
			camera.rot = glm::vec2(-(glm::pi<float>() * 0.5f) - (elapsedTime * 0.5f), glm::pi<float>() * 0.25f);
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

			DrawWalls(dynamic_cast<SW::TileSet*>(&tiles), tiles.GetAABB());

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
	void Vessel::DrawWalls(SW::TileSet* ts, SW::AABBi region)
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
		playerComp = tiles.CompartmentAt(myPlayer->pos);
	}
}