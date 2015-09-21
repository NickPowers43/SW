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
#include <SDL/SDL_input.h>
#include <SDL/SDL_scancode.h>
#include <SDL/SDL_events.h>

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
		if (myPlayer)
		{
			camera.position = myPlayer->pos;

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
			glUniform4fv(coloredVertexProgram.color, 1, glm::value_ptr(color));
			glUniformMatrix4fv(coloredVertexProgram.viewMat, 1, false, glm::value_ptr(viewMat));

			DrawWalls();

			//glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glUseProgram(shadowProgram.program);
			glUniform2fv(shadowProgram.playerPos, 1, glm::value_ptr(camera.position));
			glUniformMatrix4fv(shadowProgram.viewMat, 1, false, glm::value_ptr(viewMat));

			DrawShadows();

			//glEnable(GL_CULL_FACE);
			glDisable(GL_BLEND);
		}
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
						chunk->DrawWalls();
					}
				}
			}
		}
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