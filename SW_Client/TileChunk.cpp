#include "TileChunk.h"
#include "SW_Client.h"
#include <sstream>
#include <SW/AABBi.h>
#include <SW/Tile.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SW_Client
{
	TileChunk::TileChunk(glm::ivec2 index, TileChunkVersion_t version) : SW::TileChunk(index, version)
	{
		seen = false;
		instantiated = false;
		meshCreated = false;
	}


	TileChunk::~TileChunk()
	{
	}

	void TileChunk::Draw()
	{
		if (instantiated)
		{
			if (meshCreated)
			{

				//PrintMessage((int)"Drawing Mesh");
				glm::mat4 viewMat(1.0f);
				camera.GenerateView(viewMat);
				/*glm::vec2 offset = TileIToWorld(OriginTileIndex());
				viewMat = glm::translate(viewMat, glm::vec3(offset.x, offset.y, 0.0f));*/
				//cout << "Drawing chunk";

				//viewMat = glm::transpose(viewMat);

				glUseProgram(floorProgram.program);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, floorProgram.texture);
				glUniform1i(floorProgram.textureLoc, 0);

				glUniformMatrix4fv(floorProgram.viewMat, 1, false, glm::value_ptr(viewMat));

				glBindBuffer(GL_ARRAY_BUFFER, floorVBuffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorIBuffer);

				glVertexAttribPointer(floorProgram.posAttrib, 2, GL_FLOAT, false, 16, NULL);
				glEnableVertexAttribArray(floorProgram.posAttrib);
				glVertexAttribPointer(floorProgram.uvAttrib, 2, GL_FLOAT, false, 16, (GLvoid*)8);
				glEnableVertexAttribArray(floorProgram.uvAttrib);

				glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
			}
		}
	}
	void TileChunk::Instantiate(TileSet* ts)
	{
		if (!instantiated)
		{
			GenerateFloorMesh(ts);

			instantiated = true;
		}
	}
	void TileChunk::GenerateFloorMesh(TileSet* ts)
	{
		//glm::vec2 position = TileIToWorld(OriginTileIndex());
		//std::ostringstream print;
		//print << "GenerateFloorMesh: index: (" << index.x << ", " << index.y << "), ";

		std::vector<float> vertices;
		std::vector<MeshIndex_t> indices;

		SW::AABBi aabb = GetAABB();
		//print << "aabb: (" << aabb.bl.x << ", " << aabb.bl.y << "),(" << aabb.tr.x << ", " << aabb.tr.y << ")\n";
		for (int i = aabb.bl.y; i < aabb.tr.y; i++)
		{
			for (int j = aabb.bl.x; j < aabb.tr.x; j++)
			{
				SW::Tile* tile = ts->TryGet(glm::ivec2(j, i));

				if (tile) {

					//print << "non-NULL-tile at: (" << j << ", " << i << ")\n";

					if (tile->floor0 != FloorType::None || tile->floor1 != FloorType::None) {

						glm::vec2 offset = glm::vec2(j, i);

						if (tile->floor0 == tile->floor1) {
							AppendMeshData(GetFloorMesh(tile->floor0, FloorType::None, WallType::None, 0), vertices, indices, offset);
						}
						else {

							SW::Tile* lTile = ts->TryGet(glm::ivec2(j - 1, i));
							SW::Tile* rTile = ts->TryGet(glm::ivec2(j + 1, i));
							SW::Tile* r2Tile = ts->TryGet(glm::ivec2(j + 2, i));
							SW::Tile* bTile = ts->TryGet(glm::ivec2(j, i - 1));
							SW::Tile* brTile = ts->TryGet(glm::ivec2(j + 1, i - 1));

							if (tile->ContainsMask(WallTypeMask::TwoByOne)) { //this tile contains a TwoByOne
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::TwoByOne, 0), vertices, indices, offset);
							}
							else if (tile->ContainsMask(WallTypeMask::OneByTwo)) { //this tile contains a OneByTwo
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::OneByTwo, 0), vertices, indices, offset);
							}
							else if (tile->ContainsMask(WallTypeMask::OneByOne)) { //this tile contains a OneByOne
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::OneByOne, 0), vertices, indices, offset);
							}
							else if (lTile && lTile->ContainsMask(WallTypeMask::TwoByOne)) { //left tile contains a TwoByOne
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::TwoByOne, 1), vertices, indices, offset);
							}
							else if (bTile && bTile->ContainsMask(WallTypeMask::OneByTwo)) { //bottom tile contains a OneByTwo
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::OneByTwo, 1), vertices, indices, offset);
							}
							else if (brTile && brTile->ContainsMask(WallTypeMask::OneByTwoFlipped)) { //br tile contains a OneByTwoFlipped
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::OneByTwoFlipped, 1), vertices, indices, offset);
							}
							else if (rTile && rTile->ContainsMask(WallTypeMask::TwoByOneFlipped)) { //r tile contains a TwoByOneFlipped
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::TwoByOneFlipped, 0), vertices, indices, offset);
							}
							else if (rTile && rTile->ContainsMask(WallTypeMask::OneByOneFlipped)) { //r tile contains a OneByOneFlipped
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::OneByOneFlipped, 0), vertices, indices, offset);
							}
							else if (rTile && rTile->ContainsMask(WallTypeMask::OneByTwoFlipped)) { //r tile contains a OneByTwoFlipped
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::OneByTwoFlipped, 0), vertices, indices, offset);
							}
							else if (r2Tile && r2Tile->ContainsMask(WallTypeMask::TwoByOneFlipped)) { //r2 tile contains a TwoByOneFlipped
								AppendMeshData(GetFloorMesh(tile->floor0, tile->floor1, WallType::TwoByOneFlipped, 1), vertices, indices, offset);
							}
							else {
								//no walls cut this tile
								AppendMeshData(GetFloorMesh(tile->floor0, FloorType::None, WallType::None, 0), vertices, indices, offset);
							}
						}
					}
				}
			}
		}

		//PrintMessage((int)print.str().c_str());

		if (indices.size() > 0)
		{
			glGenBuffers(1, &floorIBuffer);
			glGenBuffers(1, &floorVBuffer);

			glBindBuffer(GL_ARRAY_BUFFER, floorVBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), (GLvoid*)&vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorIBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MeshIndex_t) * indices.size(), (GLvoid*)&indices[0], GL_STATIC_DRAW);
			indicesCount = indices.size();

			meshCreated = true;
			//PrintMessage((int)"Mesh created");
		}
		else
		{
			meshCreated = false;
		}

	}
	void TileChunk::Destroy()
	{
		if (instantiated)
		{
			if (meshCreated)
			{
				glDeleteBuffers(1, &floorIBuffer);
				glDeleteBuffers(1, &floorVBuffer);
				meshCreated = false;
			}
			instantiated = false;
		}
	}
}
