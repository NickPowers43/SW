#include "Vessel.h"
#include "SW_Client.h"
#include <sstream>
#include <SW/Tile.h>
#include <SW/SW.h>
#include "Player.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SW_Client
{
	Vessel::Vessel(VesselIndex_t index) : SW::Vessel(index)
	{
		myPlayer = NULL;
	}


	Vessel::~Vessel()
	{
	}

	void Vessel::AddMyPlayer(Player* player, NetworkWriter* nw)
	{
		if (player)
		{
			myPlayer = player;
			UpdateChunks(true, nw);
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
			UpdateChunks(false, nw);
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
	void Vessel::UpdateChunks(bool force, NetworkWriter* nw)
	{
		if (myPlayer)
		{
			size_t rangeT = (PLAYER_CHUNK_RANGE * 2) + 1;
			glm::ivec2 orgChunkI = myPlayer->chunkI;
			myPlayer->chunkI = TileChunks::WorldToChunkI(myPlayer->pos);

			if (force || ((orgChunkI.x != myPlayer->chunkI.x) || (orgChunkI.y != myPlayer->chunkI.y)))
			{
				nw->WriteMessageType(ClientMessageType::RequestChunk);
				uint8_t requestLength = 0;
				uint8_t* requestLengthLocation = (uint8_t*)nw->cursor;
				nw->WriteUint8(0);

				for (size_t i = 0; i < rangeT; i++)
				{
					for (size_t j = 0; j < rangeT; j++)
					{

						glm::ivec2 diffCurr(j - PLAYER_CHUNK_RANGE, i - PLAYER_CHUNK_RANGE);
						glm::ivec2 temp(myPlayer->chunkI + diffCurr);
						glm::ivec2 diffOrg(temp - orgChunkI);

						if (force || ((abs(diffOrg.x) > PLAYER_CHUNK_RANGE) || (abs(diffOrg.y) > PLAYER_CHUNK_RANGE)))
						{
							std::ostringstream print;
							print << "Requesting chunk at: ";
							print << std::to_string(temp.x);
							print << ", ";
							print << std::to_string(temp.y);
							print << ")";
							PrintMessage((int)print.str().c_str());

							requestLength++;


							nw->WriteInt16((int16_t)temp.x);
							nw->WriteInt16((int16_t)temp.y);

							SW::TileChunk* temp2 = tiles.TryGetChunk(glm::ivec2(temp.x, temp.y));
							if (temp2)
							{
								TileChunk* temp3 = static_cast<TileChunk*>(temp2);
								temp3->seen = true;

								nw->WriteUint32((TileChunkVersion_t)temp2->version);
							}
							else
							{
								nw->WriteUint32((TileChunkVersion_t)4294967295);
							}
						}
					}
				}

				nw->WriteUint8(requestLength, (char*)requestLengthLocation);

				//destroy chunks that are no longer visible
				for (size_t i = 0; i < tiles.chunks.dim.y; i++)
				{
					for (size_t j = 0; j < tiles.chunks.dim.x; j++)
					{
						glm::ivec2 chunkI(tiles.chunks.origin + glm::ivec2(j, i));
						SW::TileChunk* swChunk = tiles.TryGetChunk(glm::ivec2(chunkI.x, chunkI.y));
						if (swChunk)
						{
							SW_Client::TileChunk* chunk = static_cast<SW_Client::TileChunk*>(swChunk);
							if (chunk->seen)
							{
								InstantiateChunk(chunk);
							}
							else
							{
								chunk->Destroy();
							}

							chunk->seen = false;
						}
					}
				}
			}
		}
	}

	void Vessel::DrawWorld()
	{
		glm::mat4 viewMat(1.0f);
		camera.GenerateView(viewMat);

		glUseProgram(floorProgram.program);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorProgram.texture);
		glUniformMatrix4fv(floorProgram.viewMat, 1, false, glm::value_ptr(viewMat));
		glUniform1i(floorProgram.textureLoc, 0);

		DrawFloor();

		glUseProgram(coloredVertexProgram.program);
		glm::vec4 color(0.5f, 0.5f, 0.5f, 1.0f);
		glUniform4fv(coloredVertexProgram.color, 1, &color.x);
		glUniformMatrix4fv(coloredVertexProgram.viewMat, 1, false, glm::value_ptr(viewMat));

		DrawWalls();

		glUseProgram(shadowProgram.program);
		glUniform2fv(shadowProgram.playerPos, 1, &myPlayer->pos.x);
		glUniformMatrix4fv(shadowProgram.viewMat, 1, false, glm::value_ptr(viewMat));

		DrawShadows();

	}
	void Vessel::DrawFloor()
	{
		for (size_t i = 0; i < tiles.chunks.dim.y; i++)
		{
			for (size_t j = 0; j < tiles.chunks.dim.x; j++)
			{
				glm::ivec2 chunkI(currentVessel->tiles.chunks.origin + glm::ivec2(j, i));
				SW::TileChunk* swChunk = tiles.TryGetChunk(glm::ivec2(chunkI.x, chunkI.y));
				if (swChunk)
				{
					SW_Client::TileChunk* chunk = static_cast<SW_Client::TileChunk*>(swChunk);
					if (chunk->instantiated)
					{
						chunk->DrawFloor();
					}
				}
			}
		}
	}
	void Vessel::DrawWalls()
	{

	}
	void Vessel::DrawShadows()
	{
		for (size_t i = 0; i < tiles.chunks.dim.y; i++)
		{
			for (size_t j = 0; j < tiles.chunks.dim.x; j++)
			{
				glm::ivec2 chunkI(currentVessel->tiles.chunks.origin + glm::ivec2(j, i));
				SW::TileChunk* swChunk = tiles.TryGetChunk(glm::ivec2(chunkI.x, chunkI.y));
				if (swChunk)
				{
					SW_Client::TileChunk* chunk = static_cast<SW_Client::TileChunk*>(swChunk);
					if (chunk->instantiated)
					{
						chunk->DrawShadows();
					}
				}
			}
		}
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

			newChunks[i] = chunk;
		}

		//Reinstantiate chunks
		for (size_t i = 0; i < chunkCount; i++)
		{
			TileChunk* chunk = newChunks[i];
			SW::TileChunk* swChunk = tiles.TryGetChunk(chunk->index);
			if (swChunk)
			{
				TileChunk* existingChunk = static_cast<TileChunk*>(swChunk);
				existingChunk->Destroy();
				delete existingChunk;
			}

			tiles.SetChunk(static_cast<SW::TileChunk*>(chunk));
		}
		for (size_t i = 0; i < chunkCount; i++)
		{
			TileChunk* chunk = newChunks[i];

			glm::ivec2 diff(chunk->index - myPlayer->chunkI);
			if ((abs<int>(diff.x) <= PLAYER_CHUNK_RANGE) || (abs<int>(diff.y) <= PLAYER_CHUNK_RANGE)) {

				InstantiateChunk(chunk);
			}

		}

		delete newChunks;
	}

	void Vessel::InstantiateChunk(TileChunk* chunk)
	{
		chunk->Instantiate(dynamic_cast<SW::TileSet*>(&tiles));
	}
}