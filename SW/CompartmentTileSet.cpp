#include "CompartmentTileSet.h"
#include "AABBi.h"
#include "AdjacentTiles.h"
#include "Compartment.h"
#include "CompartmentTile.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>



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
	void CompartmentTileSet::CopyCompartments(Tile* src, glm::ivec2 location, Tile* dst)
	{
		SetC0(dst, location, GetC0(src));
		SetC1(dst, location, GetC1(src));
	}
	float CompartmentTileSet::C0Area(Tile* tile, glm::ivec2 location)
	{
		float c0Area;
		SW::Tile* lTile = TryGet(glm::ivec2(location.x - 1, location.y));
		SW::Tile* rTile = TryGet(glm::ivec2(location.x + 1, location.y));
		SW::Tile* r2Tile = TryGet(glm::ivec2(location.x + 2, location.y));
		SW::Tile* bTile = TryGet(glm::ivec2(location.x, location.y - 1));
		SW::Tile* brTile = TryGet(glm::ivec2(location.x + 1, location.y - 1));

		if (tile->ContainsMask(WallTypeMask::TwoByOne)) { //this tile contains a TwoByOne
			c0Area = 0.25f;
		}
		else if (tile->ContainsMask(WallTypeMask::OneByTwo)) { //this tile contains a OneByTwo
			c0Area = 0.75f;
		}
		else if (tile->ContainsMask(WallTypeMask::OneByOne)) { //this tile contains a OneByOne
			c0Area = 0.5f;
		}
		else if (lTile && lTile->ContainsMask(WallTypeMask::TwoByOne)) { //left tile contains a TwoByOne
			c0Area = 0.75f;
		}
		else if (bTile && bTile->ContainsMask(WallTypeMask::OneByTwo)) { //bottom tile contains a OneByTwo
			c0Area = 0.25f;
		}
		else if (brTile && brTile->ContainsMask(WallTypeMask::OneByTwoFlipped)) { //br tile contains a OneByTwoFlipped
			c0Area = 0.75f;
		}
		else if (rTile && rTile->ContainsMask(WallTypeMask::TwoByOneFlipped)) { //r tile contains a TwoByOneFlipped
			c0Area = 0.75f;
		}
		else if (rTile && rTile->ContainsMask(WallTypeMask::OneByOneFlipped)) { //r tile contains a OneByOneFlipped
			c0Area = 0.5f;
		}
		else if (rTile && rTile->ContainsMask(WallTypeMask::OneByTwoFlipped)) { //r tile contains a OneByTwoFlipped
			c0Area = 0.25f;
		}
		else if (r2Tile && r2Tile->ContainsMask(WallTypeMask::TwoByOneFlipped)) { //r2 tile contains a TwoByOneFlipped
			c0Area = 0.25f;
		}
		else {
			c0Area = 1.0f;
		}
		return c0Area;
	}
	float CompartmentTileSet::C0Area(AdjacentTiles* t)
	{
		float c0Area;

		if (t->tile->ContainsMask(WallTypeMask::TwoByOne)) { //this tile contains a TwoByOne
			c0Area = 0.25f;
		}
		else if (t->tile->ContainsMask(WallTypeMask::OneByTwo)) { //this tile contains a OneByTwo
			c0Area = 0.75f;
		}
		else if (t->tile->ContainsMask(WallTypeMask::OneByOne)) { //this tile contains a OneByOne
			c0Area = 0.5f;
		}
		else if (t->lTile && t->lTile->ContainsMask(WallTypeMask::TwoByOne)) { //left tile contains a TwoByOne
			c0Area = 0.75f;
		}
		else if (t->bTile && t->bTile->ContainsMask(WallTypeMask::OneByTwo)) { //bottom tile contains a OneByTwo
			c0Area = 0.25f;
		}
		else if (t->brTile && t->brTile->ContainsMask(WallTypeMask::OneByTwoFlipped)) { //br tile contains a OneByTwoFlipped
			c0Area = 0.75f;
		}
		else if (t->rTile && t->rTile->ContainsMask(WallTypeMask::TwoByOneFlipped)) { //r tile contains a TwoByOneFlipped
			c0Area = 0.75f;
		}
		else if (t->rTile && t->rTile->ContainsMask(WallTypeMask::OneByOneFlipped)) { //r tile contains a OneByOneFlipped
			c0Area = 0.5f;
		}
		else if (t->rTile && t->rTile->ContainsMask(WallTypeMask::OneByTwoFlipped)) { //r tile contains a OneByTwoFlipped
			c0Area = 0.25f;
		}
		else if (t->r2Tile && t->r2Tile->ContainsMask(WallTypeMask::TwoByOneFlipped)) { //r2 tile contains a TwoByOneFlipped
			c0Area = 0.25f;
		}
		else {
			c0Area = 1.0f;
		}

		return c0Area;
	}
	void CompartmentTileSet::SetC0(AdjacentTiles* t, Compartment* c)
	{
		float area = C0Area(t);
		SetC0(t->tile, t->pos, area, c);
	}
	void CompartmentTileSet::SetC1(AdjacentTiles* t, Compartment* c)
	{
		float area = 1.0f - C0Area(t);
		SetC1(t->tile, t->pos, area, c);
	}
	void CompartmentTileSet::SetC0(Tile* tile, glm::ivec2 location, Compartment* c)
	{
		float area = C0Area(tile, location);
		SetC0(tile, location, area, c);
	}
	void CompartmentTileSet::SetC1(Tile* tile, glm::ivec2 location, Compartment* c)
	{
		float area = 1.0f - C0Area(tile, location);
		SetC1(tile, location, area, c);
	}
	void CompartmentTileSet::SetC0(Tile* tile, glm::ivec2 location, float area, Compartment* c)
	{
		if (c)
		{
			//increment c reference counter
			//add area
			c->aabb.FitWhole(location);
		}
		Compartment* c0Old = (static_cast<CompartmentTile*>(tile))->c0;
		(static_cast<CompartmentTile*>(tile))->c0 = c;
		if (c0Old)
		{
			//decrement c0Old reference counter
			//subtract area
		}
	}
	void CompartmentTileSet::SetC1(Tile* tile, glm::ivec2 location, float area, Compartment* c)
	{
		if (c)
		{
			//increment c reference counter
			//add area
			c->aabb.FitWhole(location);
		}
		Compartment* c1Old = (static_cast<CompartmentTile*>(tile))->c1;
		(static_cast<CompartmentTile*>(tile))->c1 = c;
		if (c1Old)
		{
			//decrement c0Old reference counter
			//subtract area
		}
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
						//std::cout << "Changing floor0 type at (" << tileI.x << "," << tileI.y << ")" << std::endl;
						tile->floor0 = type;
					}
					if (GetC1(tile) && GetC1(tile)->Instance() == c) {
						//std::cout << "Changing floor1 type at (" << tileI.x << "," << tileI.y << ")" << std::endl;
						tile->floor1 = type;
					}
				}
			}
		}
	}
	void CompartmentTileSet::SetCompartmentFloorAt(FloorType_t type, glm::vec2 world)
	{
		SetCompartmentFloor(type, CompartmentAt(world));
	}

	Compartment* CompartmentTileSet::GetC0(Tile* tile)
	{
		Compartment* temp = (static_cast<CompartmentTile*>(tile))->c0;
		if (temp)
		{
			temp = temp->Instance();
		}
		return temp;
	}
	Compartment* CompartmentTileSet::GetC1(Tile* tile)
	{
		Compartment* temp = (static_cast<CompartmentTile*>(tile))->c1;
		if (temp)
		{
			temp = temp->Instance();
		}
		return temp;
	}

	void CompartmentTileSet::FillTile(bool force, AdjacentTiles* t)
	{
		//take the compartments from left or bottom no matter what in these cases
		if ((t->bTile && t->bTile->ContainsMask(WallTypeMask::WallTypeMask::OneByTwo)) || (t->brTile && t->brTile->ContainsMask(WallTypeMask::WallTypeMask::OneByTwoFlipped))) {
			CopyCompartments(t->bTile, t->pos, t->tile);
			return;
		}
		if ((t->lTile && t->lTile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOne)) || (t->rTile && t->rTile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOneFlipped))) {
			CopyCompartments(t->lTile, t->pos, t->tile);
			return;
		}
		//

		//try take from bottom
		Compartment* b = NULL;
		if (t->bTile)
		{
			if (!t->tile->ContainsMask(WallTypeMask::WallTypeMask::OneByZero))
			{
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
		if (t->lTile)
		{
			if (!t->tile->ContainsMask(WallTypeMask::WallTypeMask::ZeroByOne))
			{
				l = GetC0(t->lTile);
			}
		}

		if ((t->r2Tile && t->r2Tile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOneFlipped)) ||
			(t->rTile && t->rTile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::OneByOneFlipped | WallTypeMask::WallTypeMask::OneByTwoFlipped)))) {
			if (!(!force && !t->tile->floor1))
			{
				if (l)
				{
					if (b)
					{
						l->Reference(b);
						SetC1(t, b);
					}
					else
					{
						SetC1(t, l);
					}
				}
				else
				{
					if (b)
					{
						SetC1(t, b);
					}
					else
					{
						SetC1(t, CreateCompartment());
					}
				}
			}
			else
			{
				SetC1(t, NULL);
			}

			if (!(!force && !t->tile->floor0))
			{
				SetC0(t, CreateCompartment());
			}
			else
			{
				SetC0(t, NULL);
			}
			return;
		}
		else if (t->tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::TwoByOne | WallTypeMask::WallTypeMask::OneByOne | WallTypeMask::WallTypeMask::OneByTwo))) {
			if (!(!force && !t->tile->floor0))
			{
				SetC0(t, (b) ? b : CreateCompartment());
			}
			else
			{
				SetC0(t, NULL);
			}
			if (!(!force && !t->tile->floor1))
			{
				SetC1(t, (l) ? l : CreateCompartment());
			}
			else
			{
				SetC1(t, NULL);
			}
			return;
		}
		else {
			if (!(!force && !t->tile->floor0))
			{
				if (l)
				{
					if (b)
					{
						l->Reference(b);
						SetC0(t, l->Instance());
					}
					else
					{
						SetC0(t, l);
					}
				}
				else
				{
					if (b)
					{
						SetC0(t, b);
					}
					else
					{
						SetC0(t, CreateCompartment());
					}
				}
			}
			else
			{
				SetC0(t, NULL);
			}

			SetC1(t, NULL);
			return;
		}
	}
	void CompartmentTileSet::EraseCompartments(AABBi aabb)
	{
		for (int i = aabb.bl.y; i < aabb.tr.y; i++) {

			for (int j = aabb.bl.x; j < aabb.tr.x; j++) {
				glm::ivec2 tileI(j, i);
				Tile* tile = TryGet(tileI);
				if (tile)
				{
					SetC0(tile, tileI, NULL);
					SetC1(tile, tileI, NULL);
				}
			}
		}
	}
	void CompartmentTileSet::RebuildCompartmentsForRegion(bool force, AABBi aabb)
	{
		AdjacentTiles* at = new AdjacentTiles(this);

		for (int i = aabb.bl.y; i < aabb.tr.y; i++) {

			at->Reset(glm::ivec2(aabb.bl.x, i));

			for (int j = aabb.bl.x; j <= aabb.tr.x; j++) {
				if (at->tile) {

					FillTile(force, at);

				}
				at->MoveRight();
			}
		}

		return;
	}
	void CompartmentTileSet::RebuildCompartments(bool force)
	{
		RebuildCompartmentsForRegion(force, GetAABB());
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
