#include "TileChunk.h"
#include "SW_Client.h"
#include <sstream>
#include <SW/AABBi.h>
#include <SW/Tile.h>
#include <glm/trigonometric.hpp>

namespace SW_Client
{
	TileChunk::TileChunk(glm::ivec2 index, TileChunkVersion_t version) : SW::TileChunk(index, version)
	{
		seen = false;
		instantiated = false;
		wMeshCreated = false;
		sMeshCreated = false;
		fMeshCreated = false;
	}


	TileChunk::~TileChunk()
	{
	}

	void TileChunk::DrawFloor()
	{
		if (instantiated)
		{
			if (fMeshCreated)
			{

				//PrintMessage((int)"Drawing Mesh");
				/*glm::vec2 offset = TileIToWorld(OriginTileIndex());
				viewMat = glm::translate(viewMat, glm::vec3(offset.x, offset.y, 0.0f));*/
				//cout << "Drawing chunk";

				//viewMat = glm::transpose(viewMat);


				glBindBuffer(GL_ARRAY_BUFFER, floorVBuffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorIBuffer);

				glVertexAttribPointer(floorProgram.posAttrib, 2, GL_FLOAT, false, 16, NULL);
				glEnableVertexAttribArray(floorProgram.posAttrib);
				glVertexAttribPointer(floorProgram.uvAttrib, 2, GL_FLOAT, false, 16, (GLvoid*)8);
				glEnableVertexAttribArray(floorProgram.uvAttrib);

				glDrawElements(GL_TRIANGLES, fIndicesCount, GL_UNSIGNED_INT, 0);
			}
		}
	}
	void TileChunk::DrawWalls()
	{
		if (instantiated)
		{
			if (wMeshCreated)
			{
				glBindBuffer(GL_ARRAY_BUFFER, wallVBuffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallIBuffer);

				glVertexAttribPointer(coloredVertexProgram.worldPosAttrib, 2, GL_FLOAT, false, 8, NULL);
				glEnableVertexAttribArray(coloredVertexProgram.worldPosAttrib);

				glDrawElements(GL_TRIANGLES, wIndicesCount, GL_UNSIGNED_INT, 0);
			}
		}
	}
	void TileChunk::DrawShadows()
	{
		if (instantiated)
		{
			if (sMeshCreated)
			{

				//PrintMessage((int)"Drawing Mesh");
				/*glm::vec2 offset = TileIToWorld(OriginTileIndex());
				viewMat = glm::translate(viewMat, glm::vec3(offset.x, offset.y, 0.0f));*/
				//cout << "Drawing chunk";

				//viewMat = glm::transpose(viewMat);


				glBindBuffer(GL_ARRAY_BUFFER, shadowVBuffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shadowIBuffer);

				glVertexAttribPointer(shadowProgram.worldPosAttrib, 4, GL_FLOAT, false, 32, NULL);
				glEnableVertexAttribArray(shadowProgram.worldPosAttrib);
				glVertexAttribPointer(shadowProgram.shadowAttrib, 4, GL_FLOAT, false, 32, (GLvoid*)16);
				glEnableVertexAttribArray(shadowProgram.shadowAttrib);

				glDrawElements(GL_TRIANGLES, sIndicesCount, GL_UNSIGNED_INT, 0);
			}
		}
	}
	void TileChunk::Instantiate(TileSet* ts)
	{
		if (!instantiated)
		{
			GenerateMeshes(ts);

			instantiated = true;
		}
	}
	//point between vectors if considering CCW convention
	glm::vec2 WallCorner(glm::vec2 wall0, glm::vec2 wall1)
	{
		float cosTheta = glm::dot(wall0, wall1);
		glm::vec2 dir(-wall0.y, wall0.x);

		if (cosTheta < -0.93f)
		{
			//probably opposites
			return dir * HALF_WALL_THICKNESS;
		}
		else
		{
			float cosThetaDir = glm::dot(dir, wall1);
			glm::vec2 avg = glm::normalize((wall0 + wall1) * 0.5f);

			float theta = glm::acos(cosTheta);
			float cMag = (1.0f / glm::cos(theta * 0.5f));

			if (cosThetaDir > 0.0f)
			{
				return avg * cMag * HALF_WALL_THICKNESS;
			}
			else
			{
				return avg * cMag * -HALF_WALL_THICKNESS;
			}
		}
	}
	//bool TryGetBeginCornerCCW(SW::TileSet* ts, SW::Tile* orgtile, glm::ivec2 location, WallType_t type, glm::vec2 & v)
	//{
	//	for (int i = type + 4; i < 9; i++)
	//	{
	//		if (orgtile->Contains(i))
	//		{
	//			//calculate the corner vertice location
	//			v = CCWCorner(SW::wallVectorsNormalized[type], SW::wallVectorsNormalized[i]);
	//			return false;
	//		}
	//	}
	//	return false;
	//}
	bool TileChunk::WallVertexSweepCCW(TileSet* ts, SW::Tile* orgtile, glm::ivec2 location, WallType_t type, bool end, glm::vec2 & v)
	{
		SW::Tile* tile;

		if (!end)
		{
			for (int i = type; i < 9; i++)
			{
				if (abs(i - type) > 3)
				{
					if (orgtile->Contains(i))
					{
						//calculate the corner vertice location
						v = WallCorner(SW::wallVectorsNormalized[type], SW::wallVectorsNormalized[i]);
						return false;
					}
				}
			}
			for (int i = 1; i < 9; i++)
			{
				if (abs(i - type) <= 4)
				{
					if ((tile = ts->TryGet(location - SW::wallOffsets[i])) && tile->Contains(i))
					{
						//calculate the corner vertice location
						v = WallCorner(SW::wallVectorsNormalized[type], -SW::wallVectorsNormalized[i]);
						return false;
					}
				}
			}
			for (int i = 1; i < type; i++)
			{
				if (abs(i - type) > 3)
				{
					if (orgtile->Contains(i))
					{
						//calculate the corner vertice location
						v = WallCorner(SW::wallVectorsNormalized[type], SW::wallVectorsNormalized[i]);
						return false;
					}
				}
			}
			glm::vec2 temp = SW::wallVectorsNormalized[type];
			v = (temp * HALF_WALL_THICKNESS) + (glm::vec2(temp.y, -temp.x) * HALF_WALL_THICKNESS);
			return true;
		}
		else
		{
			for (int i = type; i < 9; i++)
			{
				if (abs(i - type) > 3)
				{
					if ((tile = ts->TryGet(location - SW::wallOffsets[i])) && tile->Contains(i))
					{
						//calculate the corner vertice location
						v = WallCorner(-SW::wallVectorsNormalized[type], -SW::wallVectorsNormalized[i]);
						return false;
					}
				}
			}
			for (int i = 1; i < 9; i++)
			{
				if (abs(i - type) <= 4)
				{
					if (orgtile->Contains(i))
					{
						//calculate the corner vertice location
						v = WallCorner(-SW::wallVectorsNormalized[type], SW::wallVectorsNormalized[i]);
						return false;
					}
				}
			}
			for (int i = 1; i < type; i++)
			{
				if (abs(i - type) > 3)
				{
					if ((tile = ts->TryGet(location - SW::wallOffsets[i])) && tile->Contains(i))
					{
						//calculate the corner vertice location
						v = WallCorner(-SW::wallVectorsNormalized[type], -SW::wallVectorsNormalized[i]);
						return false;
					}
				}
			}
			glm::vec2 temp = SW::wallVectorsNormalized[type];
			v = (temp * HALF_WALL_THICKNESS) + (-glm::vec2(temp.y, -temp.x) * HALF_WALL_THICKNESS);
			return true;
		}
	}
	bool TileChunk::WallVertexSweepCW(TileSet* ts, SW::Tile* orgtile, glm::ivec2 location, WallType_t type, bool end, glm::vec2 & v)
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
			v = (temp * HALF_WALL_THICKNESS) + (-glm::vec2(temp.y, -temp.x) * HALF_WALL_THICKNESS);
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
			glm::vec2 temp = SW::wallVectorsNormalized[type];
			v = (temp * HALF_WALL_THICKNESS) + (glm::vec2(temp.y, -temp.x) * HALF_WALL_THICKNESS);
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
	void TileChunk::AppendWallMesh(TileSet* ts, SW::Tile* tile, glm::ivec2 location, std::vector<float> & wVertices, std::vector<MeshIndex_t> & wIndices, std::vector<float> & sVertices, std::vector<MeshIndex_t> & sIndices)
	{
		//append wall data
		WallType_t wall0;
		WallType_t wall1;
		int wCount = tile->GetWalls(&wall0, &wall1);
		if (wCount > 0)
		{
			SW::Tile* orgtile;
			bool open;
			bool openEnd;
			glm::vec2 v0;//cw
			glm::vec2 v1;//ccw
			glm::vec2 v0End;//ccw
			glm::vec2 v1End;//cw
			glm::ivec2 end = location + SW::wallOffsets[wall0];
			glm::vec2 startOrigin(location);
			glm::vec2 endOrigin(end);

			orgtile = tile;
			open = WallVertexSweepCW(ts, orgtile, location, wall0, false, v0);
			v0 += startOrigin;
			open = WallVertexSweepCCW(ts, orgtile, location, wall0, false, v1);
			v1 += startOrigin;

			orgtile = ts->TryGet(end);
			openEnd = WallVertexSweepCCW(ts, orgtile, end, wall0, true, v0End);
			v0End += endOrigin;
			openEnd = WallVertexSweepCW(ts, orgtile, end, wall0, true, v1End);
			v1End += endOrigin;

			static float influence0 = 0.0f;
			static float influence1 = 1.0f;

			//append wall mesh
			AppendQuadIndices(wIndices, wVertices, WALL_VERTEX_F_COUNT);
			wVertices.push_back(v0.x);
			wVertices.push_back(v0.y);
			wVertices.push_back(v1.x);
			wVertices.push_back(v1.y);
			wVertices.push_back(v0End.x);
			wVertices.push_back(v0End.y);
			wVertices.push_back(v1End.x);
			wVertices.push_back(v1End.y);
			//shadow triangles
			AppendQuadIndices(sIndices, sVertices, SHADOW_VERTEX_F_COUNT);
			AppendShadowVertex(startOrigin, influence1, 0.0f, 1.0f, sVertices);
			AppendShadowVertex(startOrigin, influence0, 0.0f, 1.0f, sVertices);
			AppendShadowVertex(endOrigin, influence1, 0.0f, 1.0f, sVertices);
			AppendShadowVertex(endOrigin, influence0, 0.0f, 1.0f, sVertices);
			if (open)
			{
				//append triangle to close connection
			}

			//append shadow mesh
			if (openEnd)
			{
				//append triangle to close connection
			}

			if (wCount > 1)
			{
				end = location + SW::wallOffsets[wall1];
				endOrigin = glm::vec2(end);

				orgtile = tile;
				open = WallVertexSweepCW(ts, orgtile, location, wall1, false, v0);
				v0 += startOrigin;
				open = WallVertexSweepCCW(ts, orgtile, location, wall1, false, v1);
				v1 += startOrigin;

				orgtile = ts->TryGet(end);
				openEnd = WallVertexSweepCCW(ts, orgtile, end, wall1, true, v0End);
				v0End += endOrigin;
				openEnd = WallVertexSweepCW(ts, orgtile, end, wall1, true, v1End);
				v1End += endOrigin;

				//append wall mesh
				AppendQuadIndices(wIndices, wVertices, WALL_VERTEX_F_COUNT);
				wVertices.push_back(v0.x);
				wVertices.push_back(v0.y);
				wVertices.push_back(v1.x);
				wVertices.push_back(v1.y);
				wVertices.push_back(v0End.x);
				wVertices.push_back(v0End.y);
				wVertices.push_back(v1End.x);
				wVertices.push_back(v1End.y);
				//shadow triangles
				AppendQuadIndices(sIndices, sVertices, SHADOW_VERTEX_F_COUNT);
				AppendShadowVertex(startOrigin, influence1, 0.0f, 1.0f, sVertices);
				AppendShadowVertex(startOrigin, influence0, 0.0f, 1.0f, sVertices);
				AppendShadowVertex(endOrigin, influence1, 0.0f, 1.0f, sVertices);
				AppendShadowVertex(endOrigin, influence0, 0.0f, 1.0f, sVertices);
			}
		}
	}
	void TileChunk::GenerateMeshes(TileSet* ts)
	{
		//glm::vec2 position = TileIToWorld(OriginTileIndex());
		//std::ostringstream print;
		//print << "GenerateFloorMesh: index: (" << index.x << ", " << index.y << "), ";

		std::vector<float> fVertices;
		std::vector<MeshIndex_t> fIndices;

		std::vector<float> wVertices;
		std::vector<MeshIndex_t> wIndices;

		std::vector<float> sVertices;
		std::vector<MeshIndex_t> sIndices;

		SW::AABBi aabb = GetAABB();
		//print << "aabb: (" << aabb.bl.x << ", " << aabb.bl.y << "),(" << aabb.tr.x << ", " << aabb.tr.y << ")\n";
		for (int i = aabb.bl.y; i < aabb.tr.y; i++)
		{
			for (int j = aabb.bl.x; j < aabb.tr.x; j++)
			{
				glm::ivec2 tileI(j, i);
				SW::Tile* tile = ts->TryGet(tileI);

				if (tile) {

					glm::vec2 offset = glm::vec2(j, i);

					//print << "non-NULL-tile at: (" << j << ", " << i << ")\n";
					if (tile->wallMask > 0)
					{
						AppendWallMesh(ts, tile, tileI, wVertices, wIndices, sVertices, sIndices);
					}

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

		//PrintMessage((int)print.str().c_str());

		if (wIndices.size() > 0)
		{
			glGenBuffers(1, &shadowIBuffer);
			glGenBuffers(1, &shadowVBuffer);

			glBindBuffer(GL_ARRAY_BUFFER, shadowVBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sVertices.size(), (GLvoid*)&sVertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shadowIBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MeshIndex_t) * sIndices.size(), (GLvoid*)&sIndices[0], GL_STATIC_DRAW);
			sIndicesCount = sIndices.size();

			glGenBuffers(1, &wallIBuffer);
			glGenBuffers(1, &wallVBuffer);

			glBindBuffer(GL_ARRAY_BUFFER, wallVBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * wVertices.size(), (GLvoid*)&wVertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallIBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MeshIndex_t) * wIndices.size(), (GLvoid*)&wIndices[0], GL_STATIC_DRAW);
			wIndicesCount = wIndices.size();

			wMeshCreated = true;
			sMeshCreated = true;
		}
		else
		{
			wMeshCreated = false;
			sMeshCreated = false;
		}

		if (fIndices.size() > 0)
		{
			glGenBuffers(1, &floorIBuffer);
			glGenBuffers(1, &floorVBuffer);

			glBindBuffer(GL_ARRAY_BUFFER, floorVBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * fVertices.size(), (GLvoid*)&fVertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorIBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MeshIndex_t) * fIndices.size(), (GLvoid*)&fIndices[0], GL_STATIC_DRAW);
			fIndicesCount = fIndices.size();

			fMeshCreated = true;
			//PrintMessage((int)"Mesh created");
		}
		else
		{
			fMeshCreated = false;
		}

	}
	void TileChunk::Destroy()
	{
		if (instantiated)
		{
			if (fMeshCreated)
			{
				glDeleteBuffers(1, &floorIBuffer);
				glDeleteBuffers(1, &floorVBuffer);
				fMeshCreated = false;
			}

			if (wMeshCreated)
			{
				glDeleteBuffers(1, &wallIBuffer);
				glDeleteBuffers(1, &wallVBuffer);
				wMeshCreated = false;
			}

			if (sMeshCreated)
			{
				glDeleteBuffers(1, &shadowIBuffer);
				glDeleteBuffers(1, &shadowVBuffer);
				sMeshCreated = false;
			}

			instantiated = false;
		}
	}
}
