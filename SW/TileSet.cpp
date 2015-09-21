
#include "TileSet.h"
#include "Tile.h"

namespace SW
{
	TileSet::TileSet()
	{
	}


	TileSet::~TileSet()
	{
	}

	Tile* TileSet::CreateTile()
	{
		return new Tile();
	}
	void TileSet::DestroyTile(Tile* tile)
	{
		delete tile;
	}

	void TileSet::BuildWall(glm::ivec2 index, int count, uint8_t type)
	{
		Tile* tile;

		for (int i = 0; i < count; i++) {

			if (IsWallLegal(index, type)) {

				tile = TryGet(index);
				if (!tile) {
					tile = new Tile();
					Set(index, tile);
				}

				tile->wallMask |= (1 << (type - 1));
				tile->flags |= TileFlag::TileFlag::WallNode;

				index += wallOffsets[type];

				tile = TryGet(index);
				if (!tile) {
					tile = new Tile();
					Set(index, tile);
				}
				tile->flags |= TileFlag::TileFlag::WallNode;
			}
			else {
				index += wallOffsets[type];
			}
		}
	}
	void TileSet::BuildWall(glm::ivec2 index, int count, uint8_t type, bool reverse)
	{
		if (reverse) {
			index -= wallOffsets[type] * count;
		}

		BuildWall(index, count, type);

		if (!reverse) {
			index += wallOffsets[type] * count;
		}
	}
	void TileSet::BuildWall(glm::ivec2* index, int count, uint8_t type, bool reverse)
	{
		if (reverse) {
			*index -= wallOffsets[type] * count;
		}

		BuildWall(*index, count, type);

		if (!reverse) {
			*index += wallOffsets[type] * count;
		}
	}
	void TileSet::BuildFoundation(glm::ivec2 origin, glm::ivec2 size)
	{

		for (int i = 0; i < size.y; i++) {
			for (int j = 0; j < size.x; j++) {
				Tile* tile;
				glm::ivec2 temp = origin + glm::ivec2(j, i);
				if (!(tile = TryGet(temp))) {
					Set(temp, CreateTile());
				}
			}
		}
	}

	bool TileSet::IsWallLegal(glm::ivec2 index, uint8_t type)
	{
		Tile* tile = TryGet(index);
		if (!tile)
			return false;

		/*if ((tile = TryGet(index)) && tile->blockT != ObjectType.None) {
		return false;
		}
		if ((tile = TryGet(glm::ivec2(index.x - 1, index.y)))  && tile->blockT != ObjectType.None) {
		return false;
		}*/

		//check if walls nodes are not too close
		//to the new wall
		if (!(type == WallType::WallType::OneByZero || type == WallType::WallType::ZeroByOne)) {

			int hDir = (type < WallType::WallType::ZeroByOne) ? 1 : -1;
			int diff = abs<int>(type - WallType::WallType::ZeroByOne);

			if (hDir < 0) {
				if (ContainsWall(glm::ivec2(index.x + hDir, index.y))) {
					return false;
				}
				if (diff != 2) {
					if (ContainsWall(glm::ivec2(index.x - 1, index.y + 1))) {
						return false;
					}
					if (diff == 3) {
						if (ContainsWall(glm::ivec2(index.x - 2, index.y))) {
							return false;
						}
					}
					else if (diff == 1) {
						if (ContainsWall(glm::ivec2(index.x, index.y + 2))) {
							return false;
						}
					}
				}
			}
			else {
				if (ContainsWall(glm::ivec2(index.x, index.y + 1))) {
					return false;
				}
				if (diff != 2) {
					if (ContainsWall(glm::ivec2(index.x + 1, index.y + 1))) {
						return false;
					}
					if (diff == 3) {
						if (ContainsWall(glm::ivec2(index.x + 2, index.y))) {
							return false;
						}
					}
					else if (diff == 1) {
						if (ContainsWall(glm::ivec2(index.x, index.y + 2))) {
							return false;
						}
					}
				}
			}
		}

		glm::ivec2 end = index + wallOffsets[type];

		if (TooCloseToWall(end) || TooCloseToWall(index)) {
			return false;
		}

		return LegalWallStart(type, index) && LegalWallEnd(type, end);
	}
	bool TileSet::ContainsWall(glm::ivec2 index)
	{
		Tile* tile = TryGet(index);

		if (tile) {
			if (tile->flags & TileFlag::TileFlag::WallNode) {
				return true;
			}
			else {
				return false;
			}
		}

		return false;
	}
	bool TileSet::LegalWallStart(uint8_t type, glm::ivec2 index)
	{
		Tile* tile = TryGet(index);

		if (tile) {

			uint8_t wall0;
			uint8_t wall1;
			int wallCount = tile->GetWalls(&wall0, &wall1);

			if (wallCount > 1) {
				return false;
			}
			if (wallCount == 1) {
				if (abs<int>((uint8_t)wall0 - (uint8_t)type) < 4) {
					return false;
				}
			}
		}

		for (uint8_t i = 1; i < 9; i++) {
			Tile* otherTile = TryGet(index - wallOffsets[i]);
			if (otherTile) {
				if (otherTile->Contains((uint8_t)i)) {
					if (!NonAcuteSequence((uint8_t)i, type)) {
						return false;
					}
				}
			}
		}

		return true;
	}
	bool TileSet::TooCloseToWall(glm::ivec2 index)
	{
		Tile* tile;

		//0
		tile = TryGet(glm::ivec2(index.x - 1, index.y));
		if (tile) {
			if (tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::OneByOne | WallTypeMask::WallTypeMask::TwoByOne | WallTypeMask::WallTypeMask::OneByTwo))) {
				return true;
			}
		}
		//1
		tile = TryGet(glm::ivec2(index.x, index.y - 1));
		if (tile) {
			if (tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::OneByOne |
				WallTypeMask::WallTypeMask::OneByTwo |
				WallTypeMask::WallTypeMask::OneByOneFlipped |
				WallTypeMask::WallTypeMask::OneByTwoFlipped |
				WallTypeMask::WallTypeMask::TwoByOne |
				WallTypeMask::WallTypeMask::TwoByOneFlipped))) {
				return true;
			}
		}
		//2
		tile = TryGet(glm::ivec2(index.x + 1, index.y));
		if (tile) {
			if (tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::OneByOneFlipped | WallTypeMask::WallTypeMask::TwoByOneFlipped | WallTypeMask::WallTypeMask::OneByTwoFlipped))) {
				return true;
			}
		}
		//3
		tile = TryGet(glm::ivec2(index.x - 1, index.y - 1));
		if (tile) {
			if (tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::OneByTwo | WallTypeMask::WallTypeMask::TwoByOne))) {
				return true;
			}
		}
		//4
		tile = TryGet(glm::ivec2(index.x + 1, index.y - 1));
		if (tile) {
			if (tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::TwoByOneFlipped | WallTypeMask::WallTypeMask::OneByTwoFlipped))) {
				return true;
			}
		}
		//5
		tile = TryGet(glm::ivec2(index.x - 2, index.y));
		if (tile) {
			if (tile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOne)) {
				return true;
			}
		}
		//6
		tile = TryGet(glm::ivec2(index.x + 2, index.y));
		if (tile) {
			if (tile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOneFlipped)) {
				return true;
			}
		}
		//7
		tile = TryGet(glm::ivec2(index.x, index.y - 2));
		if (tile) {
			if (tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::OneByTwoFlipped | WallTypeMask::WallTypeMask::OneByTwo))) {
				return true;
			}
		}

		return false;
	}
	bool TileSet::EmptyRect(glm::ivec2 bl, glm::ivec2 size)
	{
		for (int i = 0; i < size.y; i++) {
			for (int j = 0; j < size.x; j++) {

				glm::ivec2 index = bl + glm::ivec2(j, i);

				Tile* tile = TryGet(index);

				if (tile) {
					if (!EmptyTile(index) ||
						((i != 0) && tile->ContainsMask(WallTypeMask::WallTypeMask::OneByZero)) ||
						((j != 0) && tile->ContainsMask(WallTypeMask::WallTypeMask::ZeroByOne))) {
						return false;
					}
				}
				else {
					if (!EmptyTile(index)) {
						return false;
					}
				}


			}
		}

		return true;
	}
	bool TileSet::EmptyTile(glm::ivec2 loc)
	{
		Tile* tile;

		tile = TryGet(loc);
		if (tile) {
			if (tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::OneByOne | WallTypeMask::WallTypeMask::TwoByOne | WallTypeMask::WallTypeMask::OneByTwo))) {
				return false;
			}
		}
		tile = TryGet(glm::ivec2(loc.x - 1, loc.y));
		if (tile) {
			if (tile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOne)) {
				return false;
			}
		}
		tile = TryGet(glm::ivec2(loc.x + 1, loc.y));
		if (tile) {
			if (tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::OneByOneFlipped | WallTypeMask::WallTypeMask::TwoByOneFlipped))) {
				return false;
			}
		}
		tile = TryGet(glm::ivec2(loc.x + 2, loc.y));
		if (tile) {
			if (tile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOneFlipped)) {
				return false;
			}
		}
		tile = TryGet(glm::ivec2(loc.x, loc.y - 1));
		if (tile) {
			if (tile->ContainsMask(WallTypeMask::WallTypeMask::OneByTwo)) {
				return false;
			}
		}
		tile = TryGet(glm::ivec2(loc.x + 1, loc.y - 1));
		if (tile) {
			if (tile->ContainsMask(WallTypeMask::WallTypeMask::OneByTwoFlipped)) {
				return false;
			}
		}

		return true;
	}
	bool TileSet::LegalWallEnd(uint8_t type, glm::ivec2 index)
	{
		Tile* tile = TryGet(index);

		if (tile) {

			uint8_t wall0;
			uint8_t wall1;
			int wallCount = tile->GetWalls(&wall0, &wall1);

			//check with the walls originating from index
			if (wallCount > 0) {
				if (!NonAcuteSequence(type, wall0)) {
					return false;
				}
				if (wallCount > 1) {
					if (!NonAcuteSequence(type, wall1)) {
						return false;
					}
				}
			}
		}

		//check other walls that touch index
		for (uint8_t i = 1; i < 9; i++) {
			Tile* otherTile = TryGet(index - wallOffsets[i]);
			if (otherTile) {
				if (otherTile->Contains((uint8_t)i)) {
					if (abs<int>((uint8_t)type - (uint8_t)i) < 4) {
						return false;
					}
				}
			}
		}

		return true;
	}
	bool TileSet::NonAcuteSequence(uint8_t wall0, uint8_t wall1)
	{
		if (wall1 < WallType::WallType::ZeroByOne) {
			if ((uint8_t)wall0 > 4 + (uint8_t)wall1) {
				return false;
			}
		}
		else if (wall1 > WallType::WallType::ZeroByOne) {
			if ((uint8_t)wall0 < (uint8_t)wall1 - 4) {
				return false;
			}
		}

		return true;
	}

	glm::ivec2 TileSet::WorldToTileI(glm::vec2 worldPosition)
	{
		return glm::ivec2(
			(worldPosition.x < 0.0f) ? -(int)(-worldPosition.x) - 1 : (int)worldPosition.x,
			(worldPosition.y < 0.0f) ? -(int)(-worldPosition.y) - 1 : (int)worldPosition.y);
	}
	glm::vec2 TileSet::TileIToWorld(glm::ivec2 tileI)
	{
		return glm::vec2(tileI.x, tileI.y);
	}
}
