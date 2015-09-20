#include "CompartmentTileSet.h"
#include "AABBi.h"
#include "AdjacentTiles.h"
#include "Compartment.h"
#include "CompartmentTile.h"



namespace SW
{
	CompartmentTileSet::CompartmentTileSet()
	{
	}
	CompartmentTileSet::~CompartmentTileSet()
	{
	}

	Tile* CompartmentTileSet::CreateTile()
	{
		return new SW::CompartmentTile();
	}
	void CompartmentTileSet::DestroyTile(Tile* tile)
	{
		delete tile;
	}

	Compartment* CompartmentTileSet::CreateCompartment()
	{
		return new Compartment(0);
	}
	void CompartmentTileSet::CopyCompartments(Tile* src, Tile* dst)
	{
		SetC0(dst, static_cast<CompartmentTile*>(src)->c0);
		SetC1(dst, static_cast<CompartmentTile*>(src)->c1);
	}
	void CompartmentTileSet::SetC0(Tile* tile, Compartment* c)
	{
		(static_cast<CompartmentTile*>(tile))->c0 = c;
	}
	void CompartmentTileSet::SetC1(Tile* tile, Compartment* c)
	{
		(static_cast<CompartmentTile*>(tile))->c1 = c;
	}
	void CompartmentTileSet::SetCompartmentFloor(uint8_t type, Compartment* c)
	{
		AABBi aabb = GetAABB();

		for (int i = aabb.bl.y; i < aabb.tr.y; i++) {
			for (int j = aabb.bl.x; j < aabb.tr.x; j++) {
				glm::ivec2 tileI = glm::ivec2(j, i);
				Tile* tile;
				if ((tile = TryGet(tileI))) {
					if (GetC0(tile) && GetC0(tile)->Instance() == c) {
						//cout << "Changing floor0 type at (" << tileI.x << "," << tileI.y << ")" << std::endl;
						tile->floor0 = type;
					}
					if (GetC1(tile) && GetC1(tile)->Instance() == c) {
						//cout << "Changing floor1 type at (" << tileI.x << "," << tileI.y << ")" << std::endl;
						tile->floor1 = type;
					}
				}
			}
		}
	}
	void CompartmentTileSet::SetCompartmentFloorAt(FloorType_t type, glm::vec2 world)
	{
		SW::Compartment* c = CompartmentAt(world);
		if (c)
		{
			Compartment* sC = static_cast<Compartment*>(c);
			SetCompartmentFloor(FloorType::Basic, sC->Instance());
		}
	}

	Compartment* CompartmentTileSet::GetC0(Tile* tile)
	{
		return (static_cast<CompartmentTile*>(tile))->c0;
	}
	Compartment* CompartmentTileSet::GetC1(Tile* tile)
	{
		return (static_cast<CompartmentTile*>(tile))->c1;
	}


	void CompartmentTileSet::FillTile(AdjacentTiles* t)
	{
		if (t->tile->flags & TileFlag::TileFlag::SolidBlock) {
			SetC0(t->tile, NULL);
			SetC1(t->tile, NULL);
		}

		//take the compartments from left or bottom no matter what in these cases
		if ((t->bTile && t->bTile->ContainsMask(WallTypeMask::WallTypeMask::OneByTwo)) || (t->brTile && t->brTile->ContainsMask(WallTypeMask::WallTypeMask::OneByTwoFlipped))) {
			CopyCompartments(t->bTile, t->tile);
			return;
		}
		if ((t->lTile && t->lTile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOne)) || (t->rTile && t->rTile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOneFlipped))) {
			CopyCompartments(t->lTile, t->tile);
			return;
		}
		//

		//try take from bottom
		Compartment* b = NULL;
		if (t->bTile) {
			if (t->bTile->flags & TileFlag::TileFlag::SolidBlock)
				b = NULL;
			else if (!t->tile->ContainsMask(WallTypeMask::WallTypeMask::OneByZero)) {
				//set the appropriate compartment field depending on how "bTile" is cut
				if ((t->blTile && t->blTile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOne)) ||
					(t->bTile->ContainsMask(WallTypeMask::WallTypeMask::OneByOne)) ||
					(t->b2Tile && t->b2Tile->ContainsMask(WallTypeMask::WallTypeMask::OneByTwo))) {
					b = GetC1(t->bTile);
				}
				else {
					b = GetC0(t->bTile);
				}
			}
		}
		Compartment* l = NULL;
		if (t->lTile) {
			if ((t->lTile->flags & TileFlag::TileFlag::SolidBlock) > 0)
				l = NULL;
			else if (!t->tile->ContainsMask(WallTypeMask::WallTypeMask::ZeroByOne)) {
				l = GetC0(t->lTile);
			}
		}

		if (
			(t->r2Tile && t->r2Tile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOneFlipped)) ||
			(t->rTile && t->rTile->ContainsMask(WallTypeMask::WallTypeMask::OneByOneFlipped | WallTypeMask::WallTypeMask::OneByTwoFlipped))) {
			if (l && b) {
				l->Reference(b);
			}
			if (t->tile->floor1)
			{
				l = (l) ? l : b;
				if (!l)
				{
					l = CreateCompartment();
				}
				SetC1(t->tile, l->Instance());
			}
			if (t->tile->floor0)
			{
				SetC0(t->tile, CreateCompartment());
			}
			return;
		}
		else if (t->tile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOne | WallTypeMask::WallTypeMask::OneByOne | WallTypeMask::WallTypeMask::OneByTwo)) {

			if (t->tile->floor0)
				SetC0(t->tile, (b) ? b : CreateCompartment());
			else
				SetC0(t->tile, NULL);

			if (t->tile->floor1)
				SetC1(t->tile, (b) ? b : CreateCompartment());
			else
				SetC1(t->tile, NULL);

			return;
		}
		else {
			if (l && b) {
				l->Reference(b);
			}

			if (t->tile->floor0)
			{
				l = (l) ? l : b;
				SetC0(t->tile, (l) ? l->Instance() : CreateCompartment());
			}
			else
				SetC0(t->tile, NULL);
			SetC1(t->tile, NULL);
			return;
		}
	}
	void CompartmentTileSet::FillTileFloorless(AdjacentTiles* t)
	{
		if (t->tile->flags & TileFlag::TileFlag::SolidBlock) {
			SetC0(t->tile, NULL);
			SetC1(t->tile, NULL);
		}

		//take the compartments from left or bottom no matter what in these cases
		if ((t->bTile && t->bTile->ContainsMask(WallTypeMask::WallTypeMask::OneByTwo)) || (t->brTile && t->brTile->ContainsMask(WallTypeMask::WallTypeMask::OneByTwoFlipped))) {
			CopyCompartments(t->bTile, t->tile);
			return;
		}
		if ((t->lTile && t->lTile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOne)) || (t->rTile && t->rTile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOneFlipped))) {
			CopyCompartments(t->lTile, t->tile);
			return;
		}
		//

		//try take from bottom
		Compartment* b = NULL;
		if (t->bTile) {
			if (t->bTile->flags & TileFlag::TileFlag::SolidBlock)
				b = NULL;
			else if (!t->tile->ContainsMask(WallTypeMask::WallTypeMask::OneByZero)) {
				//set the appropriate compartment field depending on how "bTile" is cut
				if ((t->blTile && t->blTile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOne)) ||
					(t->bTile->ContainsMask(WallTypeMask::WallTypeMask::OneByOne)) ||
					(t->b2Tile && t->b2Tile->ContainsMask(WallTypeMask::WallTypeMask::OneByTwo))) {
					b = GetC1(t->bTile);
				}
				else {
					b = GetC0(t->bTile);
				}
			}
		}
		Compartment* l = NULL;
		if (t->lTile) {
			if ((t->lTile->flags & TileFlag::TileFlag::SolidBlock) > 0)
				l = NULL;
			else if (!t->tile->ContainsMask(WallTypeMask::WallTypeMask::ZeroByOne)) {
				l = GetC0(t->lTile);
			}
		}


		if (
			(t->r2Tile && t->r2Tile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOneFlipped)) ||
			(t->rTile && t->rTile->ContainsMask(WallTypeMask::WallTypeMask::OneByOneFlipped | WallTypeMask::WallTypeMask::OneByTwoFlipped))) {
			if (l)
			{
				if (b)
				{
					Compartment* c = b->Instance();
					l->Reference(c);
					SetC1(t->tile, c);
				}
				else
				{
					SetC1(t->tile, l->Instance());
				}
			}
			else
			{
				if (b)
				{
					SetC1(t->tile, b->Instance());
				}
				else
				{
					SetC1(t->tile, CreateCompartment());
				}
			}
			SetC0(t->tile, CreateCompartment());
			return;
		}
		else if (t->tile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOne | WallTypeMask::WallTypeMask::OneByOne | WallTypeMask::WallTypeMask::OneByTwo)) {
			SetC0(t->tile, (b) ? b : CreateCompartment());
			SetC1(t->tile, (l) ? l : CreateCompartment());
			return;
		}
		else {
			if (l)
			{
				if (b)
				{
					Compartment* c = b->Instance();
					l->Reference(c);
					SetC0(t->tile, c);
				}
				else
				{
					SetC0(t->tile, l->Instance());
				}
			}
			else
			{
				if (b)
				{
					SetC0(t->tile, b->Instance());
				}
				else
				{
					SetC0(t->tile, CreateCompartment());
				}
			}
			SetC1(t->tile, NULL);
			return;
		}
	}
	void CompartmentTileSet::RebuildCompartments()
	{
		AABBi aabb = GetAABB();

		AdjacentTiles* at = new AdjacentTiles(this);

		for (int i = aabb.bl.y; i < aabb.tr.y; i++) {

			at->Reset(glm::ivec2(aabb.bl.x, i));

			for (int j = aabb.bl.x; j < aabb.tr.x; j++) {
				if (at->tile) {

					FillTile(at);

				}
				at->MoveRight();
			}
		}

		return;
	}
	void CompartmentTileSet::RebuildCompartmentsFloorless()
	{
		AABBi aabb = GetAABB();

		AdjacentTiles* at = new AdjacentTiles(this);

		for (int i = aabb.bl.y; i < aabb.tr.y; i++) {

			at->Reset(glm::ivec2(aabb.bl.x, i));

			for (int j = aabb.bl.x; j < aabb.tr.x; j++) {
				if (at->tile) {

					FillTileFloorless(at);

				}
				at->MoveRight();
			}
		}

		return;
	}
	Compartment* CompartmentTileSet::CompartmentAt(glm::vec2 world)
	{
		glm::ivec2 tileI = WorldToTileI(world);
		glm::vec2 diff = world - TileIToWorld(tileI);

		Tile* oTile = TryGet(tileI);
		Tile* tile = oTile;

		if (tile) {
			for (int i = (int)WallType::WallType::TwoByOne; i < (int)WallType::WallType::ZeroByOne; i++) {
				if (tile->Contains((WallType::WallType)i)) {
					if (glm::dot(diff, glm::vec2(-wallOffsets[i].y, wallOffsets[i].x)) > 0.0f) {
						return GetC1(oTile);
					}
					else {
						return GetC0(oTile);
					}
				}
			}

			glm::ivec2 tempI = glm::ivec2(tileI.x, tileI.y - 1);
			if ((tile = TryGet(tempI)) && tile->Contains(WallType::WallType::OneByTwo)) {
				diff = world - TileIToWorld(tempI);
				if (glm::dot(diff, glm::vec2(-wallOffsets[(int)WallType::WallType::OneByTwo].y, wallOffsets[(int)WallType::WallType::OneByTwo].x)) > 0.0f) {
					return GetC1(oTile);
				}
				else {
					return GetC0(oTile);
				}
			}
			tempI = glm::ivec2(tileI.x + 1, tileI.y - 1);
			if (((tile = TryGet(tempI))) && tile->Contains(WallType::WallType::OneByTwoFlipped)) {
				diff = world - TileIToWorld(tempI);
				if (glm::dot(diff, glm::vec2(-wallOffsets[(int)WallType::WallType::OneByTwoFlipped].y, wallOffsets[(int)WallType::WallType::OneByTwoFlipped].x)) > 0.0f) {
					return GetC1(oTile);
				}
				else {
					return GetC0(oTile);
				}
			}
			tempI = glm::ivec2(tileI.x + 1, tileI.y);
			if ((tile = TryGet(tempI)) && tile->Contains(WallType::WallType::OneByOneFlipped)) {
				diff = world - TileIToWorld(tempI);
				if (glm::dot(diff, glm::vec2(-wallOffsets[(int)WallType::WallType::OneByOneFlipped].y, wallOffsets[(int)WallType::WallType::OneByOneFlipped].x)) > 0.0f) {
					return GetC1(oTile);
				}
				else {
					return GetC0(oTile);
				}
			}

			tempI = glm::ivec2(tileI.x - 1, tileI.y);
			if ((tile = TryGet(tempI)) && tile->Contains(WallType::WallType::TwoByOne)) {
				diff = world - TileIToWorld(tempI);
				if (glm::dot(diff, glm::vec2(-wallOffsets[(int)WallType::WallType::TwoByOne].y, wallOffsets[(int)WallType::WallType::TwoByOne].x)) > 0.0f) {
					return GetC1(oTile);
				}
				else {
					return GetC0(oTile);
				}
			}

			for (int i = 1; i < 3; i++) {
				tempI = glm::ivec2(tileI.x + i, tileI.y);
				if ((tile = TryGet(tempI)) && tile->Contains(WallType::WallType::TwoByOneFlipped)) {
					diff = world - TileIToWorld(tempI);
					if (glm::dot(diff, glm::vec2(-wallOffsets[(int)WallType::WallType::TwoByOneFlipped].y, wallOffsets[(int)WallType::WallType::TwoByOneFlipped].x)) > 0.0f) {
						return GetC1(oTile);
					}
					else {
						return GetC0(oTile);
					}
				}
			}

			return GetC0(oTile);
		}

		return NULL;
	}
}
