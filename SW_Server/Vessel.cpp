#include "stdafx.h"
#include "Vessel.h"


namespace SW_Server
{
	uint32_t Vessel::nextIndex = 0;

	Vessel::Vessel()
	{
		nextChunkIndex = 0;
		index = nextIndex++;
		aabb = AABBi();
		interiorExists = false;
		modifiedChunks.reserve(10);
		Vessel::index = index;
	}

	Vessel::~Vessel()
	{
	}

	bool Vessel::IsWallLegal(glm::ivec2 index, uint8_t type)
	{
		VesselTile* tile = TryGetTile(index);
		if (!tile)
			return false;

		VesselTile* tile = NULL;
		/*if ((tile = TryGetTile(index)) != NULL && tile->blockT != BlockType.None) {
			return false;
		}
		if ((tile = TryGetTile(glm::ivec2(index.x - 1, index.y))) != NULL  && tile->blockT != BlockType.None) {
			return false;
		}*/

		//check if foundations have been set and walls nodes are not too close
		//to the new wall
		if (!(type == WallType_OneByZero || type == WallType_ZeroByOne)) {

			int hDir = (type < WallType_ZeroByOne) ? 1 : -1;
			int diff = abs(type - WallType_ZeroByOne);

			if (hDir < 0) {
				if ((tile = TryGetTile(glm::ivec2(index.x - 1, index.y))) == NULL) {
					return false;
				}
				/*else {
					if (tile->blockT != BlockType.None)
						return false;
				}
				if (((tile = TryGetTile(glm::ivec2(index.x - 1, index.y - 1))) != NULL && tile->blockT != BlockType.None) ||
					((tile = TryGetTile(glm::ivec2(index.x - 2, index.y))) != NULL && tile->blockT != BlockType.None) ||
					((tile = TryGetTile(glm::ivec2(index.x - 1, index.y + 1))) != NULL && tile->blockT != BlockType.None) ||
					((tile = TryGetTile(glm::ivec2(index.x, index.y))) != NULL && tile->blockT != BlockType.None) ||
					((tile = TryGetTile(glm::ivec2(index.x - 2, index.y - 1))) != NULL && tile->blockT != BlockType.None) ||
					((tile = TryGetTile(glm::ivec2(index.x, index.y + 1))) != NULL && tile->blockT != BlockType.None)) {
					return false;
				}*/
				if (diff != 2) {
					/*if (((tile = TryGetTile(glm::ivec2(index.x - 1, index.y + 1))) != NULL && tile->blockT != BlockType.None) ||
						ContainsWall(glm::ivec2(index.x - 1, index.y + 1))) {
						return false;
					}*/
					if (ContainsWall(glm::ivec2(index.x - 1, index.y + 1))) {
						return false;
					}
				}
				if (diff == 3) {
					if (TryGetTile(glm::ivec2(index.x - 2, index.y)) == NULL ||
						ContainsWall(glm::ivec2(index.x - 2, index.y))) {
						return false;
					}
					/*if (((tile = TryGetTile(glm::ivec2(index.x - 3, index.y - 1))) != NULL && tile->blockT != BlockType.None) ||
						((tile = TryGetTile(glm::ivec2(index.x - 3, index.y))) != NULL && tile->blockT != BlockType.None)) {
						return false;
					}*/
				}
				else if (diff == 1) {
					if (TryGetTile(glm::ivec2(index.x - 1, index.y + 1)) == NULL ||
						ContainsWall(glm::ivec2(index.x, index.y + 2))) {
						return false;
					}
					/*if (((tile = TryGetTile(glm::ivec2(index.x - 2, index.y + 2))) != NULL && tile->blockT != BlockType.None) ||
						((tile = TryGetTile(glm::ivec2(index.x - 1, index.y + 2))) != NULL && tile->blockT != BlockType.None)) {
						return false;
					}*/
				}
			}
			else {
				if ((tile = TryGetTile(glm::ivec2(index.x, index.y))) == NULL) {
					return false;
				}
				/*else {
					if (tile->blockT != BlockType.None)
						return false;
				}
				if ((tile = TryGetTile(glm::ivec2(index.x, index.y - 1))) != NULL && tile->blockT != BlockType.None) {
					return false;
				}
				if (((tile = TryGetTile(glm::ivec2(index.x, index.y - 1))) != NULL && tile->blockT != BlockType.None) ||
					((tile = TryGetTile(glm::ivec2(index.x + 1, index.y))) != NULL && tile->blockT != BlockType.None) ||
					((tile = TryGetTile(glm::ivec2(index.x, index.y + 1))) != NULL && tile->blockT != BlockType.None) ||
					((tile = TryGetTile(glm::ivec2(index.x - 1, index.y))) != NULL && tile->blockT != BlockType.None) ||
					((tile = TryGetTile(glm::ivec2(index.x + 1, index.y - 1))) != NULL && tile->blockT != BlockType.None) ||
					((tile = TryGetTile(glm::ivec2(index.x - 1, index.y + 1))) != NULL && tile->blockT != BlockType.None)) {
					return false;
				}*/
				if (diff != 2) {
					/*if (((tile = TryGetTile(glm::ivec2(index.x + 1, index.y + 1))) != NULL && tile->blockT != BlockType.None)) {
						return false;
					}*/
					if (ContainsWall(glm::ivec2(index.x + 1, index.y + 1))) {
						return false;
					}
				}
				if (diff == 3) {
					if (ContainsWall(glm::ivec2(index.x + 2, index.y))) {
						return false;
					}
					/*if (((tile = TryGetTile(glm::ivec2(index.x + 2, index.y - 1))) != NULL && tile->blockT != BlockType.None) ||
						((tile = TryGetTile(glm::ivec2(index.x + 2, index.y))) != NULL && tile->blockT != BlockType.None)) {
						return false;
					}*/
				}
				else if (diff == 1) {
					if (TryGetTile(glm::ivec2(index.x, index.y + 1)) == NULL ||
						ContainsWall(glm::ivec2(index.x, index.y + 2))) {
						return false;
					}
					/*if (((tile = TryGetTile(glm::ivec2(index.x - 1, index.y + 2))) != NULL && tile->blockT != BlockType.None) ||
						((tile = TryGetTile(glm::ivec2(index.x, index.y + 2))) != NULL && tile->blockT != BlockType.None)) {
						return false;
					}*/
				}
			}


			if (ContainsWall(glm::ivec2(index.x, index.y + 1)) || ContainsWall(glm::ivec2(index.x + hDir, index.y))) {
				return false;
			}
		}
		else {
			if (TryGetTile(index) == NULL) {
				return false;
			}
		}

		glm::ivec2 end = index + wallOffsets[type];

		if (TooCloseToWall(end) || TooCloseToWall(index)) {
			return false;
		}

		return LegalWallStart(type, index) && LegalWallEnd(type, end);
	}
	bool Vessel::ContainsWall(glm::ivec2 index)
	{
		VesselTile* tile = TryGetTile(index);

		if (tile != NULL) {
			if (tile->flags & TileFlag_WallNode) {
				return true;
			}
			else {
				return false;
			}
		}

		return false;
	}

	bool Vessel::LegalWallStart(uint8_t type, glm::ivec2 index)
	{
		VesselTile* tile = TryGetTile(index);

		if (tile != NULL) {

			uint8_t wall0;
			uint8_t wall1;
			int wallCount = tile->GetWalls(&wall0, &wall1);

			if (wallCount > 1) {
				return false;
			}
			if (wallCount == 1) {
				if (abs((uint8_t )wall0 - (uint8_t )type) < 4) {
					return false;
				}
			}
		}

		for (uint8_t  i = 1; i < 9; i++) {
			VesselTile* otherTile = TryGetTile(index - wallOffsets[i]);
			if (otherTile != NULL) {
				if (otherTile->Contains((uint8_t)i)) {
					if (!NonAcuteSequence((uint8_t)i, type)) {
						return false;
					}
				}
			}
		}

		return true;
	}

	bool Vessel::TooCloseToWall(glm::ivec2 index)
	{
		VesselTile* tile;

		//0
		tile = TryGetTile(glm::ivec2(index.x - 1, index.y));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask_OneByOne | WallTypeMask_TwoByOne | WallTypeMask_OneByTwo)) {
				return true;
			}
		}
		//1
		tile = TryGetTile(glm::ivec2(index.x, index.y - 1));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask_OneByOne |
				WallTypeMask_OneByTwo |
				WallTypeMask_OneByOneFlipped |
				WallTypeMask_OneByTwoFlipped |
				WallTypeMask_TwoByOne |
				WallTypeMask_TwoByOneFlipped)) {
				return true;
			}
		}
		//2
		tile = TryGetTile(glm::ivec2(index.x + 1, index.y));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask_OneByOneFlipped | WallTypeMask_TwoByOneFlipped | WallTypeMask_OneByTwoFlipped)) {
				return true;
			}
		}
		//3
		tile = TryGetTile(glm::ivec2(index.x - 1, index.y - 1));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask_OneByTwo | WallTypeMask_TwoByOne)) {
				return true;
			}
		}
		//4
		tile = TryGetTile(glm::ivec2(index.x + 1, index.y - 1));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask_TwoByOneFlipped | WallTypeMask_OneByTwoFlipped)) {
				return true;
			}
		}
		//5
		tile = TryGetTile(glm::ivec2(index.x - 2, index.y));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask_TwoByOne)) {
				return true;
			}
		}
		//6
		tile = TryGetTile(glm::ivec2(index.x + 2, index.y));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask_TwoByOneFlipped)) {
				return true;
			}
		}
		//7
		tile = TryGetTile(glm::ivec2(index.x, index.y - 2));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask_OneByTwoFlipped | WallTypeMask_OneByTwo)) {
				return true;
			}
		}

		return false;
	}

	bool Vessel::EmptyRect(glm::ivec2 bl, glm::ivec2 size)
	{
		for (int i = 0; i < size.y; i++) {
			for (int j = 0; j < size.x; j++) {

				glm::ivec2 index = bl + glm::ivec2(j, i);

				VesselTile* tile = TryGetTile(index);

				if (tile != NULL) {
					if (!EmptyTile(index) ||
						((i != 0) && tile->ContainsMask(WallTypeMask_OneByZero)) ||
						((j != 0) && tile->ContainsMask(WallTypeMask_ZeroByOne))) {
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

	bool Vessel::EmptyTile(glm::ivec2 loc)
	{
		VesselTile* tile;

		tile = TryGetTile(loc);
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask_OneByOne | WallTypeMask_TwoByOne | WallTypeMask_OneByTwo)) {
				return false;
			}
		}
		tile = TryGetTile(glm::ivec2(loc.x - 1, loc.y));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask_TwoByOne)) {
				return false;
			}
		}
		tile = TryGetTile(glm::ivec2(loc.x + 1, loc.y));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask_OneByOneFlipped | WallTypeMask_TwoByOneFlipped)) {
				return false;
			}
		}
		tile = TryGetTile(glm::ivec2(loc.x + 2, loc.y));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask_TwoByOneFlipped)) {
				return false;
			}
		}
		tile = TryGetTile(glm::ivec2(loc.x, loc.y - 1));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask_OneByTwo)) {
				return false;
			}
		}
		tile = TryGetTile(glm::ivec2(loc.x + 1, loc.y - 1));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask_OneByTwoFlipped)) {
				return false;
			}
		}

		return true;
	}

	bool Vessel::LegalWallEnd(uint8_t type, glm::ivec2 index)
	{
		VesselTile* tile = TryGetTile(index);

		if (tile != NULL) {

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
		for (uint8_t  i = 1; i < 9; i++) {
			VesselTile* otherTile = TryGetTile(index - wallOffsets[i]);
			if (otherTile != NULL) {
				if (otherTile->Contains((uint8_t)i)) {
					if (abs((uint8_t )type - (uint8_t )i) < 4) {
						return false;
					}
				}
			}
		}

		return true;
	}

	bool Vessel::NonAcuteSequence(uint8_t wall0, uint8_t wall1)
	{
		if (wall1 < WallType_ZeroByOne) {
			if ((uint8_t )wall0 > 4 + (uint8_t )wall1) {
				return false;
			}
		}
		else if (wall1 > WallType_ZeroByOne) {
			if ((uint8_t )wall0 < (uint8_t )wall1 - 4) {
				return false;
			}
		}

		return true;
	}


	VesselTile* Vessel::TryGetTile(glm::ivec2 index)
	{
		glm::ivec2 chunkI = TileIToChunkI(index);
		VesselChunk* vc = chunks.TryGet(chunkI);

		if (vc == NULL) {
			return NULL;
		}
		else {
			index -= vc->OriginTileIndex();
			return vc->TileAt(index);
		}
	}
	VesselChunk* Vessel::Top(VesselChunk* chunk) { return chunks.TryGet(chunk->index.x, chunk->index.y + 1); }
	VesselChunk* Vessel::Bottom(VesselChunk* chunk) { return chunks.TryGet(chunk->index.x, chunk->index.y - 1); }
	VesselChunk* Vessel::Left(VesselChunk* chunk) { return chunks.TryGet(chunk->index.x - 1, chunk->index.y); }
	VesselChunk* Vessel::Right(VesselChunk* chunk) { return chunks.TryGet(chunk->index.x + 1, chunk->index.y); }
	VesselChunk* Vessel::TopLeft(VesselChunk* chunk) { return chunks.TryGet(chunk->index.x - 1, chunk->index.y + 1); }
	VesselChunk* Vessel::TopRight(VesselChunk* chunk) { return chunks.TryGet(chunk->index.x + 1, chunk->index.y + 1); }
	VesselChunk* Vessel::BottomLeft(VesselChunk* chunk) { return chunks.TryGet(chunk->index.x - 1, chunk->index.y - 1); }
	VesselChunk* Vessel::BottomRight(VesselChunk* chunk) { return chunks.TryGet(chunk->index.x + 1, chunk->index.y - 1); }
	void Vessel::InstantiateChunk(VesselChunk* chunk)
	{
		glm::vec2 chunkOffset = ChunkIToWorld(chunk->index);
		chunk->Instantiate(
			Top(chunk),
			Left(chunk),
			Right(chunk),
			Bottom(chunk),
			BottomRight(chunk),
			chunkOffset);
	}

	glm::ivec2 Vessel::TileOffset(glm::ivec2 tileI, glm::ivec2 chunkI)
	{
		glm::ivec2 output;
		output.x = (tileI.x >= 0) ? tileI.x - (chunkI.x << CHUNK_SIZE) : tileI.x - (chunkI.x << CHUNK_SIZE);
		output.y = (tileI.y >= 0) ? tileI.y - (chunkI.y << CHUNK_SIZE) : tileI.y - (chunkI.y << CHUNK_SIZE);
		return output;
	}
	glm::ivec2 Vessel::TileIToChunkI(glm::ivec2 tileI)
	{
		glm::ivec2 output;
		output.x = (tileI.x >= 0) ? tileI.x / CHUNK_SIZE : ((tileI.x + 1) / CHUNK_SIZE) - 1;
		output.y = (tileI.y >= 0) ? tileI.y / CHUNK_SIZE : ((tileI.y + 1) / CHUNK_SIZE) - 1;
		return output;
	}
	glm::ivec2 Vessel::ChunkIToTileI(glm::ivec2 chunkI)
	{
		return chunkI * CHUNK_SIZE;
	}
	glm::ivec2 Vessel::WorldToChunkI(glm::vec2 worldPosition)
	{
		return glm::ivec2(
			(worldPosition.x < 0.0f) ? -1 - (int)(worldPosition.x / -(float)CHUNK_SIZE) : (int)(worldPosition.x / (float)CHUNK_SIZE),
			(worldPosition.y < 0.0f) ? -1 - (int)(worldPosition.y / -(float)CHUNK_SIZE) : (int)(worldPosition.y / (float)CHUNK_SIZE));
	}
	glm::ivec2 Vessel::WorldToTileI(glm::vec2 worldPosition)
	{
		return glm::ivec2(
			(worldPosition.x < 0.0f) ? -(int)(-worldPosition.x) - 1 : (int)worldPosition.x,
			(worldPosition.y < 0.0f) ? -(int)(-worldPosition.y) - 1 : (int)worldPosition.y);
	}
	glm::vec2 Vessel::ChunkIToWorld(glm::ivec2 chunkI)
	{
		return glm::vec2(
			chunkI.x * (float)CHUNK_SIZE,
			chunkI.y * (float)CHUNK_SIZE);
	}
	glm::vec2 Vessel::TileIToWorld(glm::ivec2 tileI)
	{
		return glm::vec2(tileI.x, tileI.y);
	}
}