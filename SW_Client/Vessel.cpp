#include "Vessel.h"
#include "SW_Client.h"
#include <SW/CompartmentTile.h>
#include <SW/FixedCompartmentTileSet.h>
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

			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glUseProgram(shadowProgram.program);
			glUniform2fv(shadowProgram.playerPos, 1, glm::value_ptr(camera.position));
			glUniformMatrix4fv(shadowProgram.viewMat, 1, false, glm::value_ptr(viewMat));

			DrawShadows();

			glEnable(GL_CULL_FACE);
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

	void Vessel::ReadSetTilesMessage(NetworkReader* nr, NetworkWriter* nw)
	{
		glm::ivec2 origin;
		glm::ivec2 size;

		origin.x = nr->ReadInt32();
		origin.y = nr->ReadInt32();
		size.x = nr->ReadInt32();
		size.y = nr->ReadInt32();

		//std::ostringstream print;
		//print << "ReadSetTilesMessage: (";
		//print << "origin: (" << origin.x << ", " << origin.y << "), size: (" << size.x << ", " << size.y << "))";

		SW::FixedCompartmentTileSet tileBuffer(size);

		//glm::ivec2 end(origin + size);

		for (int i = 0; i < size.y; i++)
		{
			for (int j = 0; j < size.x; j++)
			{
				glm::ivec2 tileI(j, i);

				SW::CompartmentTile* tile = new SW::CompartmentTile();

				tile->flags = nr->ReadUint16();
				tile->wallMask = nr->ReadUint8();
				tile->floor0 = nr->ReadUint8();
				tile->floor1 = nr->ReadUint8();

				tileBuffer.Set(tileI, static_cast<SW::Tile*>(tile));
			}
		}

		tileBuffer.RebuildCompartments();

		for (int i = 0; i < size.y; i++)
		{
			for (int j = 0; j < size.x; j++)
			{
				//compare the two tiles
				glm::ivec2 tileI(j, i);
				SW::Tile* tile = tileBuffer.TryGet(tileI);
				tileBuffer.Set(tileI, NULL);

				tiles.Set(tileI + origin, tile);
			}
		}

		for (int i = 0; i < tiles.chunks.dim.y; i++)
		{
			for (int j = 0; j < tiles.chunks.dim.x; j++)
			{
				glm::ivec2 chunkI(tiles.chunks.origin + glm::ivec2(j, i));

				if (tiles.chunks.TryGet(chunkI))
				{
					InstantiateChunk(dynamic_cast<SW_Client::TileChunk*>(tiles.chunks.TryGet(chunkI)));
				}
			}
		}
	}

	void Vessel::InstantiateChunk(TileChunk* chunk)
	{
		chunk->Instantiate(dynamic_cast<SW::TileSet*>(&tiles));
	}
}