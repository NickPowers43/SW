#include "Vessel.h"
#include "SW_Client.h"
#include <SW/Tile.h>
#include <SW/SW.h>
#include "Player.h"

namespace SW_Client
{
	Vessel::Vessel(VesselIndex_t index) : SW::Vessel(index)
	{
	}


	Vessel::~Vessel()
	{
	}

	void Vessel::ReadSetChunkMessage(NetworkReader* nr, NetworkWriter* nw)
	{
		uint8_t chunkCount = nr->ReadUint8();
		TileChunk** newChunks = new TileChunk*[chunkCount];

		for (size_t i = 0; i < chunkCount; i++) {

			int16_t x = nr->ReadInt16();
			int16_t y = nr->ReadInt16();
			TileChunkVersion_t version = nr->ReadUint32();

			TileChunk* chunk = new TileChunk(glm::ivec2(x, y), version);


			//console.log("Received chunk at (" + x + ", " + y + ")");

			int16_t tile_count = nr->ReadUint16();
			for (size_t j = 0; j < tile_count; j++) {
				int16_t tileLinearI = nr->ReadUint16();


				SW::Tile* tile = new SW::Tile();

				tile->flags = nr->ReadUint16();
				tile->wallMask = nr->ReadUint8();
				tile->floor0 = nr->ReadUint8();
				tile->floor1 = nr->ReadUint8();
				//tile->c0 = nr->ReadUint32();
				//tile->c1 = nr->ReadUint32();

				//PrintMessage((int)"Creating tile");

				chunk->Set(glm::ivec2(tileLinearI % CHUNK_SIZE, tileLinearI / CHUNK_SIZE), tile);
			}

			/*glm::ivec2 diff(chunk->index - myPlayer->chunkI);
			if ((abs<int>(diff.x) <= PLAYER_CHUNK_RANGE) || (abs<int>(diff.y) <= PLAYER_CHUNK_RANGE)) {
				newChunks.push_back(chunk);
			}*/
			newChunks.push_back(chunk);
		}

		//Reinstantiate chunks
		while (newChunks.size() > 0) {
			TileChunk* chunk = newChunks.back();
			newChunks.pop_back();
			SW::TileChunk* swChunk = currentVessel->tiles.TryGetChunk(chunk->index);
			if (swChunk)
			{
				TileChunk* existingChunk = static_cast<TileChunk*>(swChunk);
				existingChunk->Destroy();
				delete existingChunk;
			}

			//cout << "Instantiating chunk (" << chunk->index.x << "," << chunk->index.y << ")";


			tiles.SetChunk(static_cast<SW::TileChunk*>(chunk));
			InstantiateChunk(chunk);
		}
	}

	void Vessel::InstantiateChunk(TileChunk* chunk)
	{
		chunk->Instantiate(
			tiles.ClientChunkTop(chunk),
			tiles.ClientChunkLeft(chunk),
			tiles.ClientChunkRight(chunk),
			tiles.ClientChunkBottom(chunk),
			tiles.ClientChunkBottomRight(chunk));
	}
}