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
		(static_cast<CompartmentTile*>(dst))->c0 = (static_cast<CompartmentTile*>(src))->c0->Instance();
		(static_cast<CompartmentTile*>(dst))->c1 = (static_cast<CompartmentTile*>(src))->c1->Instance();
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
					b = (static_cast<CompartmentTile*>(t->bTile))->c1->Instance();
				}
				else {
					b = (static_cast<CompartmentTile*>(t->bTile))->c0->Instance();
				}
			}
		}
		Compartment* l = NULL;
		if (t->lTile) {
			if ((t->lTile->flags & TileFlag::TileFlag::SolidBlock) > 0)
				l = NULL;
			else if (!t->tile->ContainsMask(WallTypeMask::WallTypeMask::ZeroByOne)) {
				l = (static_cast<CompartmentTile*>(t->lTile))->c0->Instance();
			}
		}

		if (
			(t->r2Tile && t->r2Tile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOneFlipped)) ||
			(t->rTile && t->rTile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::OneByOneFlipped | WallTypeMask::WallTypeMask::OneByTwoFlipped)))) {
			if (l && b) {
				l->Combine(b);
			}
			if (b) {
				SetC1(t->tile, b);
			}
			else if (l) {
				SetC1(t->tile, l);
			}

			SetC0(t->tile, CreateCompartment());

			return;
		}
		else if (t->tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::TwoByOne | WallTypeMask::WallTypeMask::OneByOne | WallTypeMask::WallTypeMask::OneByTwo))) {
			if (b == NULL) {
				b = CreateCompartment();
			}
			SetC0(t->tile, b);
			if (l == NULL) {
				l = CreateCompartment();
			}
			SetC1(t->tile, l);
			return;
		}
		else {
			if (l && b) {
				l->Combine(b);
			}

			l = (l == NULL) ? b : l;
			if (l == NULL) {
				l = CreateCompartment();
			}
			SetC0(t->tile, l->Instance());
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
						return GetC1(oTile)->Instance();
					}
					else {
						return GetC0(oTile)->Instance();
					}
				}
			}

			glm::ivec2 tempI = glm::ivec2(tileI.x, tileI.y - 1);
			if ((tile = TryGet(tempI)) && tile->Contains(WallType::WallType::OneByTwo)) {
				diff = world - TileIToWorld(tempI);
				if (glm::dot(diff, glm::vec2(-wallOffsets[(int)WallType::WallType::OneByTwo].y, wallOffsets[(int)WallType::WallType::OneByTwo].x)) > 0.0f) {
					return GetC1(oTile)->Instance();
				}
				else {
					return GetC0(oTile)->Instance();
				}
			}
			tempI = glm::ivec2(tileI.x + 1, tileI.y - 1);
			if (((tile = TryGet(tempI))) && tile->Contains(WallType::WallType::OneByTwoFlipped)) {
				diff = world - TileIToWorld(tempI);
				if (glm::dot(diff, glm::vec2(-wallOffsets[(int)WallType::WallType::OneByTwoFlipped].y, wallOffsets[(int)WallType::WallType::OneByTwoFlipped].x)) > 0.0f) {
					return GetC1(oTile)->Instance();
				}
				else {
					return GetC0(oTile)->Instance();
				}
			}
			tempI = glm::ivec2(tileI.x + 1, tileI.y);
			if ((tile = TryGet(tempI)) && tile->Contains(WallType::WallType::OneByOneFlipped)) {
				diff = world - TileIToWorld(tempI);
				if (glm::dot(diff, glm::vec2(-wallOffsets[(int)WallType::WallType::OneByOneFlipped].y, wallOffsets[(int)WallType::WallType::OneByOneFlipped].x)) > 0.0f) {
					return GetC1(oTile)->Instance();
				}
				else {
					return GetC0(oTile)->Instance();
				}
			}

			tempI = glm::ivec2(tileI.x - 1, tileI.y);
			if ((tile = TryGet(tempI)) && tile->Contains(WallType::WallType::TwoByOne)) {
				diff = world - TileIToWorld(tempI);
				if (glm::dot(diff, glm::vec2(-wallOffsets[(int)WallType::WallType::TwoByOne].y, wallOffsets[(int)WallType::WallType::TwoByOne].x)) > 0.0f) {
					return GetC1(oTile)->Instance();
				}
				else {
					return GetC0(oTile)->Instance();
				}
			}

			for (int i = 1; i < 3; i++) {
				tempI = glm::ivec2(tileI.x + i, tileI.y);
				if ((tile = TryGet(tempI)) && tile->Contains(WallType::WallType::TwoByOneFlipped)) {
					diff = world - TileIToWorld(tempI);
					if (glm::dot(diff, glm::vec2(-wallOffsets[(int)WallType::WallType::TwoByOneFlipped].y, wallOffsets[(int)WallType::WallType::TwoByOneFlipped].x)) > 0.0f) {
						return GetC1(oTile)->Instance();
					}
					else {
						return GetC0(oTile)->Instance();
					}
				}
			}

			return GetC0(oTile)->Instance();
		}

		return NULL;
	}
}
