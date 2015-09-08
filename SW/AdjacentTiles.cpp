
#include "AdjacentTiles.h"


namespace SW
{

	AdjacentTiles::AdjacentTiles(TileSet* tileSet)
	{
		AdjacentTiles::tileSet = tileSet;
		AdjacentTiles::pos = glm::ivec2(0,0);

		l2Tile = NULL;
		lTile = NULL;
		blTile = NULL;
		bTile = NULL;
		b2Tile = NULL;
		brTile = NULL;
		b2rTile = NULL;
		br2Tile = NULL;
		tile = NULL;
		rTile = NULL;
		r2Tile = NULL;
	}

	AdjacentTiles::AdjacentTiles(TileSet* tileSet, glm::ivec2 start)
	{
		AdjacentTiles::tileSet = tileSet;
		AdjacentTiles::pos = start;

		Reset(start);
	}

	AdjacentTiles::~AdjacentTiles()
	{
	}

	void AdjacentTiles::Reset(glm::ivec2 pos)
	{
		AdjacentTiles::pos = pos;

		l2Tile = tileSet->TryGet(glm::ivec2(pos.x - 2, pos.y));
		lTile = tileSet->TryGet(glm::ivec2(pos.x - 1, pos.y));
		blTile = tileSet->TryGet(glm::ivec2(pos.x - 1, pos.y - 1));
		bTile = tileSet->TryGet(glm::ivec2(pos.x, pos.y - 1));
		b2Tile = tileSet->TryGet(glm::ivec2(pos.x, pos.y - 2));
		brTile = tileSet->TryGet(glm::ivec2(pos.x + 1, pos.y - 1));
		b2rTile = tileSet->TryGet(glm::ivec2(pos.x + 1, pos.y - 2));
		br2Tile = tileSet->TryGet(glm::ivec2(pos.x + 2, pos.y - 1));
		tile = tileSet->TryGet(glm::ivec2(pos.x, pos.y));
		rTile = tileSet->TryGet(glm::ivec2(pos.x + 1, pos.y));
		r2Tile = tileSet->TryGet(glm::ivec2(pos.x + 2, pos.y));
	}

	void AdjacentTiles::MoveRight()
	{

		Tile* nextR2Tile = tileSet->TryGet(glm::ivec2(pos.x + 3, pos.y));
		Tile* nextBR2Tile = tileSet->TryGet(glm::ivec2(pos.x + 3, pos.y - 1));
		Tile* nextB2RTile = tileSet->TryGet(glm::ivec2(pos.x + 2, pos.y - 2));

		blTile = bTile;
		bTile = brTile;
		brTile = br2Tile;
		br2Tile = nextBR2Tile;

		b2Tile = b2rTile;
		b2rTile = nextB2RTile;

		l2Tile = lTile;
		lTile = tile;
		tile = rTile;
		rTile = r2Tile;
		r2Tile = nextR2Tile;

		pos.x += 1;
	}
}
