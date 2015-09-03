#include "stdafx.h"
#include "AdjacentTiles.h"


namespace SW_Server
{

	AdjacentTiles::AdjacentTiles(Vessel* vessel)
	{
		AdjacentTiles::vessel = vessel;

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

	AdjacentTiles::AdjacentTiles(Vessel* vessel, glm::ivec2 start)
	{
		AdjacentTiles::vessel = vessel;

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

	AdjacentTiles::~AdjacentTiles()
	{
	}

	void AdjacentTiles::Reset(glm::ivec2 pos)
	{
		AdjacentTiles::pos = pos;

		l2Tile = vessel->TryGetTile(glm::ivec2(pos.x - 2, pos.y));
		lTile = vessel->TryGetTile(glm::ivec2(pos.x - 1, pos.y));
		blTile = vessel->TryGetTile(glm::ivec2(pos.x - 1, pos.y - 1));
		bTile = vessel->TryGetTile(glm::ivec2(pos.x, pos.y - 1));
		b2Tile = vessel->TryGetTile(glm::ivec2(pos.x, pos.y - 2));
		brTile = vessel->TryGetTile(glm::ivec2(pos.x + 1, pos.y - 1));
		b2rTile = vessel->TryGetTile(glm::ivec2(pos.x + 1, pos.y - 2));
		br2Tile = vessel->TryGetTile(glm::ivec2(pos.x + 2, pos.y - 1));
		tile = vessel->TryGetTile(glm::ivec2(pos.x, pos.y));
		rTile = vessel->TryGetTile(glm::ivec2(pos.x + 1, pos.y));
		r2Tile = vessel->TryGetTile(glm::ivec2(pos.x + 2, pos.y));
	}

	void AdjacentTiles::MoveRight()
	{

		VesselTile* nextR2Tile = vessel->TryGetTile(glm::ivec2(pos.x + 3, pos.y));
		VesselTile* nextBR2Tile = vessel->TryGetTile(glm::ivec2(pos.x + 3, pos.y - 1));
		VesselTile* nextB2RTile = vessel->TryGetTile(glm::ivec2(pos.x + 2, pos.y - 2));

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
