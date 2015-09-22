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

namespace SW_Client
{
	Vessel::Vessel(VesselIndex_t index) : SW::Vessel(index)
	{
		myPlayer = NULL;
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
			camera.position = myPlayer->pos;

			glm::mat4 viewMat(1.0f);
			camera.GenerateView(viewMat);

			if (floorVBuffer)
			{
				glUseProgram(floorProgram.program);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, floorProgram.texture);
				glUniformMatrix4fv(floorProgram.viewMat, 1, false, glm::value_ptr(viewMat));
				glUniform1i(floorProgram.textureLoc, 0);

				glBindBuffer(GL_ARRAY_BUFFER, floorVBuffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorIBuffer);

				glVertexAttribPointer(floorProgram.posAttrib, 2, GL_FLOAT, false, 16, NULL);
				glEnableVertexAttribArray(floorProgram.posAttrib);
				glVertexAttribPointer(floorProgram.uvAttrib, 2, GL_FLOAT, false, 16, (GLvoid*)8);
				glEnableVertexAttribArray(floorProgram.uvAttrib);

				glDrawElements(GL_TRIANGLES, fIndicesCount, GL_UNSIGNED_INT, 0);
			}

			DrawWalls(dynamic_cast<SW::TileSet*>(&tiles), tiles.GetAABB());

			/*glUseProgram(coloredVertexProgram.program);
			glm::vec4 color(0.5f, 0.5f, 0.5f, 1.0f);
			glUniform4fv(coloredVertexProgram.color, 1, glm::value_ptr(color));
			glUniformMatrix4fv(coloredVertexProgram.viewMat, 1, false, glm::value_ptr(viewMat));

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

					glm::vec2 offset = glm::vec2(j, i);
				}
			}
		}
	}
	//point between vectors if considering CCW convention
	float Determinant(glm::vec2 c0, glm::vec2 c1)
	{
		return (c0.x * c1.y) - (c0.y * c1.x);
	}
	glm::vec2 WallCorner(glm::vec2 wall0, glm::vec2 wall1)
	{
		glm::vec2 d(-wall0.y, wall0.x);

		float uDenom = wall1.x - wall0.x;
		float uNum;

		if (abs(uDenom) > 0.001f)
		{
			uNum = d.x - wall1.y;
		}
		else
		{
			//may be straight
			uDenom = wall1.y - wall0.y;
			uNum = d.y + wall1.x;
		}

		return d + ((wall0)* (uNum / uDenom));
	}
	WallType_t CCWReverseWallSweepOpposite(int start, int stop, SW::TileSet* ts, glm::ivec2 location, WallType_t type)
	{
		for (int i = start; i < stop; i++)
		{
			if (abs(i - type) <= 4)
			{
				SW::Tile* tile;
				if ((tile = ts->TryGet(location - SW::wallOffsets[i])) && tile->Contains(i))
				{
					return i;
				}
			}
		}
		return 0;
	}
	WallType_t CCWReverseWallSweepLocal(int start, int stop, SW::TileSet* ts, glm::ivec2 location, WallType_t type)
	{
		for (int i = start; i < stop; i++)
		{
			if (abs(i - type) > 3)
			{
				SW::Tile* tile;
				if ((tile = ts->TryGet(location - SW::wallOffsets[i])) && tile->Contains(i))
				{
					return i;
				}
			}
		}
		return 0;
	}
	WallType_t CCWWallSweepOpposite(int start, int stop, SW::Tile* orgtile, WallType_t type)
	{
		for (int i = start; i < stop; i++)
		{
			if (abs(i - type) <= 4)
			{
				if (orgtile->Contains(i))
				{
					return i;
				}
			}
		}
		return 0;
	}
	WallType_t CCWWallSweepLocal(int start, int stop, SW::Tile* orgtile, WallType_t type)
	{
		for (int i = start; i < stop; i++)
		{
			if (abs(i - type) > 3)
			{
				if (orgtile->Contains(i))
				{
					return i;
				}
			}
		}
		return 0;
	}

	static glm::vec2 Rotate90CCW(glm::vec2 v)
	{
		return glm::vec2(-v.y, v.x);
	}
	static glm::vec2 Rotate90CW(glm::vec2 v)
	{
		return glm::vec2(v.y, -v.x);
	}
	bool WallVertexSweepCCW(SW::TileSet* ts, SW::Tile* orgtile, glm::ivec2 location, WallType_t type, bool end, glm::vec2 & v)
	{
		WallType_t otherWall = 0;

		if (!end)
		{
			if ((otherWall = CCWWallSweepLocal(type, 9, orgtile, type)))
			{
				v = WallCorner(SW::wallVectorsNormalized[type], SW::wallVectorsNormalized[otherWall]);
				return false;
			}
			if ((otherWall = CCWReverseWallSweepOpposite(1, 9, ts, location, type)))
			{
				v = WallCorner(SW::wallVectorsNormalized[type], -SW::wallVectorsNormalized[otherWall]);
				return false;
			}
			if ((otherWall = CCWWallSweepLocal(1, type, orgtile, type)))
			{
				v = WallCorner(SW::wallVectorsNormalized[type], SW::wallVectorsNormalized[otherWall]);
				return false;
			}
			glm::vec2 temp = SW::wallVectorsNormalized[type];
			v = Rotate90CCW(temp) - temp;
			return true;
		}
		else
		{
			if ((otherWall = CCWReverseWallSweepLocal(type, 9, ts, location, type)))
			{
				v = WallCorner(-SW::wallVectorsNormalized[type], -SW::wallVectorsNormalized[otherWall]);
				return false;
			}
			if ((otherWall = CCWWallSweepOpposite(1, 9, orgtile, type)))
			{
				v = WallCorner(-SW::wallVectorsNormalized[type], SW::wallVectorsNormalized[otherWall]);
				return false;
			}
			if ((otherWall = CCWReverseWallSweepLocal(1, type, ts, location, type)))
			{
				v = WallCorner(-SW::wallVectorsNormalized[type], -SW::wallVectorsNormalized[otherWall]);
				return false;
			}
			glm::vec2 temp = -SW::wallVectorsNormalized[type];
			v = Rotate90CCW(temp) - temp;
			return true;
		}
	}
	bool WallVertexSweepCW(SW::TileSet* ts, SW::Tile* orgtile, glm::ivec2 location, WallType_t type, bool end, glm::vec2 & v)
	{
		SW::Tile* tile;

		if (!end)
		{
			for (int i = type; i > 0; i--)
			{
				if (abs(i - type) > 3)
				{
					if (orgtile->Contains(i))
					{
						//calculate the corner vertice location
						v = WallCorner(SW::wallVectorsNormalized[i], SW::wallVectorsNormalized[type]);
						return false;
					}
				}
			}
			for (int i = 8; i > 0; i--)
			{
				if (abs(i - type) <= 4)
				{
					if ((tile = ts->TryGet(location - SW::wallOffsets[i])) && tile->Contains(i))
					{
						//calculate the corner vertice location
						v = WallCorner(-SW::wallVectorsNormalized[i], SW::wallVectorsNormalized[type]);
						return false;
					}
				}
			}
			for (int i = 8; i > type; i--)
			{
				if (abs(i - type) > 3)
				{
					if (orgtile->Contains(i))
					{
						//calculate the corner vertice location
						v = WallCorner(SW::wallVectorsNormalized[i], SW::wallVectorsNormalized[type]);
						return false;
					}
				}
			}
			glm::vec2 temp = SW::wallVectorsNormalized[type];
			v = Rotate90CW(temp) - temp;
			return true;
		}
		else
		{
			for (int i = type; i > 0; i--)
			{
				if (abs(i - type) > 3)
				{
					if ((tile = ts->TryGet(location - SW::wallOffsets[i])) && tile->Contains(i))
					{
						//calculate the corner vertice location
						v = WallCorner(-SW::wallVectorsNormalized[i], -SW::wallVectorsNormalized[type]);
						return false;
					}
				}
			}
			for (int i = 8; i > 0; i--)
			{
				if (abs(i - type) <= 4)
				{
					if (orgtile->Contains(i))
					{
						//calculate the corner vertice location
						v = WallCorner(SW::wallVectorsNormalized[i], -SW::wallVectorsNormalized[type]);
						return false;
					}
				}
			}
			for (int i = 8; i > type; i--)
			{
				if (abs(i - type) > 3)
				{
					if ((tile = ts->TryGet(location - SW::wallOffsets[i])) && tile->Contains(i))
					{
						//calculate the corner vertice location
						v = WallCorner(-SW::wallVectorsNormalized[i], -SW::wallVectorsNormalized[type]);
						return false;
					}
				}
			}
			glm::vec2 temp = -SW::wallVectorsNormalized[type];
			v = Rotate90CW(temp) - temp;
			return true;
		}
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
	}
}