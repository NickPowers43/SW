#pragma once
#include "SW.h"
#include "TileSet.h"

namespace SW
{
	class AdjacentTiles
	{
	public:
		AdjacentTiles(TileSet* tileSet);
		AdjacentTiles(TileSet* tileSet, glm::ivec2 start);
		~AdjacentTiles();

		void Reset(glm::ivec2 pos);
		void MoveRight();

		Tile* l2Tile;
		Tile* lTile;
		Tile* blTile;
		Tile* bTile;
		Tile* b2Tile;
		Tile* brTile;
		Tile* b2rTile;
		Tile* br2Tile;
		Tile* tile;
		Tile* rTile;
		Tile* r2Tile;

	protected:
		
		TileSet* tileSet;
		glm::ivec2 pos;
	};
}

