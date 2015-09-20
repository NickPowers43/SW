#pragma once
#include <SW/SW.h>
#include <SW/TileChunk.h>
#include "GraphicsCommon.h"
#include "NetworkWriter.h"

namespace SW_Client
{
	class TileChunk :
		public SW::TileChunk
	{
	public:
		TileChunk(glm::ivec2 index, TileChunkVersion_t version);
		~TileChunk();

		void DrawFloor();
		void DrawWalls();
		void DrawShadows();
		void Instantiate(TileSet* ts);
		bool WallVertexSweepCCW(TileSet* ts, SW::Tile* Orgtile, glm::ivec2 location, WallType_t type, bool end, glm::vec2 & v);
		bool WallVertexSweepCW(TileSet* ts, SW::Tile* Orgtile, glm::ivec2 location, WallType_t type, bool end, glm::vec2 & v);
		void AppendWallMesh(TileSet* ts, SW::Tile* tile, glm::ivec2 location, std::vector<float> & wVertices, std::vector<MeshIndex_t> & wIndices, std::vector<float> & sVertices, std::vector<MeshIndex_t> & sIndices);
		void GenerateMeshes(TileSet* ts);
		void Destroy();

		bool seen;

		bool instantiated;

		bool fMeshCreated;
		int fIndicesCount;
		GLuint floorVBuffer;
		GLuint floorIBuffer;

		bool wMeshCreated;
		int wIndicesCount;
		GLuint wallVBuffer;
		GLuint wallIBuffer;

		bool sMeshCreated;
		int sIndicesCount;
		GLuint shadowVBuffer;
		GLuint shadowIBuffer;
	};
}

