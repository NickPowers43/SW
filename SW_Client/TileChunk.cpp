#include "TileChunk.h"
#include "SW_Client.h"
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
				glm::vec2 offset = TileIToWorld(OriginTileIndex());
				viewMat = glm::translate(viewMat, glm::vec3(offset.x, offset.y, 0.0f));
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
	void TileChunk::Instantiate(TileChunk* t, TileChunk* l, TileChunk* r, TileChunk* b, TileChunk* br)
	{
		if (!instantiated)
		{
			GenerateFloorMesh(t, l, r, b, br);

			instantiated = true;
		}
	}
	void TileChunk::GenerateFloorMesh(TileChunk* t, TileChunk* l, TileChunk* r, TileChunk* b, TileChunk* br)
	{
		//glm::vec2 position = TileIToWorld(OriginTileIndex());

		std::vector<float> vertices;
		std::vector<MeshIndex_t> indices;

		for (size_t i = 0; i < CHUNK_SIZE; i++) {
			for (size_t j = 0; j < CHUNK_SIZE; j++) {

				SW::Tile* tile = TryGet(glm::ivec2(i, j));

				if (tile) {

					if (tile->floor0 != FloorType::None || tile->floor1 != FloorType::None) {

						glm::vec2 offset = glm::vec2(i, j);

						SW::Tile* lTile = NULL;
						SW::Tile* rTile = NULL;
						SW::Tile* r2Tile = NULL;
						SW::Tile* bTile = NULL;
						SW::Tile* brTile = NULL;

						if (i == 0) {
							if (l) {
								lTile = l->TryGet(glm::ivec2(CHUNK_SIZE - 1, j));
							}
						}
						else {
							lTile = TryGet(glm::ivec2(i - 1, j));
						}

						if (i == CHUNK_SIZE - 1) {
							if (r) {
								rTile = r->TryGet(glm::ivec2(0, j));
							}
						}
						else {
							rTile = TryGet(glm::ivec2(i + 1, j));
						}

						if (i >= CHUNK_SIZE - 2) {
							if (r) {
								r2Tile = r->TryGet(glm::ivec2(i - (CHUNK_SIZE - 2), j));
							}
						}
						else {
							r2Tile = TryGet(glm::ivec2(i + 2, j));
						}

						if (j == 0) {
							if (b) {
								bTile = b->TryGet(glm::ivec2(i, CHUNK_SIZE - 1));
							}
						}
						else {
							bTile = TryGet(glm::ivec2(i, j - 1));
						}

						if (j == 0) {
							if (i < CHUNK_SIZE - 1) {
								if (b) {
									brTile = b->TryGet(glm::ivec2(i + 1, CHUNK_SIZE - 1));
								}
							}
							else {
								if (br) {
									brTile = br->TryGet(glm::ivec2(0, CHUNK_SIZE - 1));
								}
							}
						}
						else {
							if (i != CHUNK_SIZE - 1) {
								brTile = TryGet(glm::ivec2(i + 1, j - 1));
							}
							else {
								if (r) {
									brTile = r->TryGet(glm::ivec2(0, j - 1));
								}
							}
						}

						if (tile->floor0 == tile->floor1) {
							AppendMeshData(GetFloorMesh(tile->floor0, FloorType::None, WallType::None, 0), vertices, indices, offset);
						}
						else {
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
