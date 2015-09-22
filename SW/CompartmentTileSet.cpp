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
	void CompartmentTileSet::CopyCompartments(Tile* src, Tile* dst)
	{
		SetC0(dst, GetC0(src));
		SetC1(dst, GetC1(src));
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


	void CompartmentTileSet::FillTile(AdjacentTiles* t)
	{
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
			if (t->tile->floor1)
			{
				if (l)
				{
					if (b)
					{
						l->Reference(b);
						SetC1(t->tile, b);
					}
					else
					{
						SetC1(t->tile, l);
					}
				}
				else
				{
					if (b)
					{
						SetC1(t->tile, b);
					}
					else
					{
						SetC1(t->tile, CreateCompartment());
					}
				}
			}
			else
			{
				SetC1(t->tile, NULL);
			}

			if (t->tile->floor0)
			{
				SetC0(t->tile, CreateCompartment());
			}
			else
			{
				SetC0(t->tile, NULL);
			}

			return;
		}
		else if (t->tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::TwoByOne | WallTypeMask::WallTypeMask::OneByOne | WallTypeMask::WallTypeMask::OneByTwo))) {
			if (t->tile->floor0)
			{
				SetC0(t->tile, (b) ? b : CreateCompartment());
			}
			else
			{
				SetC0(t->tile, NULL);
			}
			if (t->tile->floor1)
			{
				SetC1(t->tile, (l) ? l : CreateCompartment());
			}
			else
			{
				SetC1(t->tile, NULL);
			}
			return;
		}
		else {
			if (t->tile->floor0)
			{
				if (l)
				{
					if (b)
					{
						l->Reference(b);
						SetC0(t->tile, l->Instance());
					}
					else
					{
						SetC0(t->tile, l);
					}
				}
				else
				{
					if (b)
					{
						SetC0(t->tile, b);
					}
					else
					{
						SetC0(t->tile, CreateCompartment());
					}
				}
			}
			else
			{
				SetC0(t->tile, NULL);
			}

			SetC1(t->tile, NULL);

			return;
		}
	}
	void CompartmentTileSet::FillTileFloorless(AdjacentTiles* t)
	{
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
			if (l)
			{
				if (b)
				{
					l->Reference(b);
					SetC1(t->tile, b);
				}
				else
				{
					SetC1(t->tile, l);
				}
			}
			else
			{
				if (b)
				{
					SetC1(t->tile, b);
				}
				else
				{
					SetC1(t->tile, CreateCompartment());
				}
			}
			SetC0(t->tile, CreateCompartment());
			return;
		}
		else if (t->tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::TwoByOne | WallTypeMask::WallTypeMask::OneByOne | WallTypeMask::WallTypeMask::OneByTwo))) {
			SetC0(t->tile, (b) ? b : CreateCompartment());
			SetC1(t->tile, (l) ? l : CreateCompartment());
			return;
		}
		else {
			if (l)
			{
				if (b)
				{
					l->Reference(b);
					SetC0(t->tile, l->Instance());
				}
				else
				{
					SetC0(t->tile, l);
				}
			}
			else
			{
				if (b)
				{
					SetC0(t->tile, b);
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
