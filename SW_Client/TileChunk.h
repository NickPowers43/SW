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

		void Draw();
		void Instantiate(TileSet* ts);
		void GenerateFloorMesh(TileSet* ts);
		void Destroy();

		bool seen;

		int indicesCount;
		bool meshCreated;
		bool instantiated;
		GLuint floorVBuffer;
		GLuint floorIBuffer;
	};
}

