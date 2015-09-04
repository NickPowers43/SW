#include "stdafx.h"
#include "Vessel.h"
#include "AdjacentTiles.h"
#include "NetworkReader.h"
#include "NetworkWriter.h"

NetworkWriter nw (1 << 14);

namespace SW_Server
{
	uint32_t Vessel::nextIndex = 0;

	Vessel::Vessel() : aabb(glm::ivec2(0, 0), glm::ivec2(0, 0))
	{
		noPlayers = true;
		timeEmpty = 0.0f;

		messageBytes = NULL;
		updateMessageBytes = true;

		nextCompartmentIndex = 0;

		nextChunkIndex = 0;
		index = nextIndex++;
		interiorExists = false;
		modifiedChunks.reserve(10);
		Vessel::index = index;
	}

	Vessel::~Vessel()
	{
	}

	int Vessel::GetMessageBytes(void* & ref)
	{
		if (updateMessageBytes)
		{
			//TODO:
			messageBytes = malloc(32);
			messageBytesCount = 32;
		}

		ref = messageBytes;
		return messageBytesCount;
	}

	void Vessel::ReadChunkRequestMessage(Player* player, NetworkReader* nr)
	{
		nw.Reset();
		nw.Write((MessageType_t)ServerMessageType::SetChunk);
		uint8_t* response_count = (uint8_t*)nw.cursor;
		nw.Write((uint8_t)0);

		uint8_t chunk_count = nr->ReadUint8();
		for (size_t i = 0; i < chunk_count; i++)
		{
			int16_t x = nr->ReadInt16();
			int16_t y = nr->ReadInt16();
			uint32_t version = nr->ReadUint32();

			VesselChunk* vc = chunks.TryGet(glm::ivec2(x, y));
			if (vc)
			{
				if (version != vc->version)
				{
					(*response_count)++;
					vc->WriteSetChunkMessage(&nw);
				}
			}
		}

		if (*response_count > 0)
		{
			try {
				myServer.send(player->hdl, nw.buffer, nw.Position(), websocketpp::frame::opcode::binary);
			}
			catch (const websocketpp::lib::error_code& e) {
				std::cout << "Failed to send message " << std::endl;
				//TODO: disconnect playersOnBoard[i]
			}
		}
	}

	void Vessel::AddPlayerVessel(Player* player, glm::vec2 position)
	{
		player->pos = position;
		player->chunkI = WorldToChunkI(position);
		player->currentVessel = this;

		//tell onBoardPlayers to add player

		nw.Reset();
		nw.Write((uint8_t)ServerMessageType::AddPlayer);
		nw.Write((float)player->pos.x);
		nw.Write((float)player->pos.y);
		for (size_t i = 0; i < playersOnBoard.size(); i++)
		{
			try {
				myServer.send(playersOnBoard[i]->hdl, nw.buffer, nw.Position(), websocketpp::frame::opcode::binary);
			}
			catch (const websocketpp::lib::error_code& e) {
				std::cout << "Failed to send message " << std::endl;
				//TODO: disconnect playersOnBoard[i]
			}
		}

		nw.Reset();
		nw.Write((uint8_t)ServerMessageType::MakeVesselActive);
		nw.Write((uint32_t)index);
		nw.Write((uint16_t)playersOnBoard.size());
		for (size_t i = 0; i < playersOnBoard.size(); i++)
		{
			nw.Write((float)playersOnBoard[i]->pos.x);
			nw.Write((float)playersOnBoard[i]->pos.y);
		}
		//write information for yourself
		nw.Write((float)player->pos.x);
		nw.Write((float)player->pos.y);

		try {
			myServer.send(player->hdl, nw.buffer, nw.Position(), websocketpp::frame::opcode::binary);
		}
		catch (const websocketpp::lib::error_code& e) {
			std::cout << "Failed to send message " << std::endl;
			//TODO: disconnect player
		}

		playersOnBoard.push_back(player);
		InstantiateNearbyChunks(player);
		noPlayers = false;
		timeEmpty = 0.0f;
	}
	void Vessel::InstantiateNearbyChunks(Player* player)
	{

	}
	void Vessel::RemovePlayer(Player* player)
	{
		uint16_t id = (1 << 17) - 1;
		for (size_t i = 0; i < playersOnBoard.size(); i++)
		{
			if (player == playersOnBoard[i])
			{
				id = i;
				break;
			}
		} 
		if (id != ((1 << 17) - 1))
		{
			for (size_t i = id; i < playersOnBoard.size() - 1; i++)
			{
				playersOnBoard[i] = playersOnBoard[i + 1];
			}
			playersOnBoard.pop_back();

			player->currentVessel = NULL;

			if (playersOnBoard.size() == 0) {
				noPlayers = true;
			}

			//if (GameManager.Instance.Initialized) {
			//	nw.SeekZero();
			//	nw.Write((ushort)ServerMessageType.RemovePlayer);
			//	nw.Write((ushort)id);
			//	for (int i = 0; i < playersOnBoard.Count; i++) {
			//		//send
			//	}
			//}
		}
	}
	void Vessel::BuildFoundation(glm::ivec2 origin, glm::ivec2 size)
	{
		VesselTile* tile;

		for (int i = 0; i < size.y; i++) {
			for (int j = 0; j < size.x; j++) {

				glm::ivec2 temp = origin + glm::ivec2(j, i);
				if ((tile = TryGetTile(temp)) == NULL) {
					SetTile(temp, new VesselTile());
				}
			}
		}
	}
	//virtual void Vessel::PlaceBlock(uint16_t type, glm::ivec2 location){}
	VesselChunk* Vessel::CreateChunk(glm::ivec2 index)
	{
		VesselChunk* temp = new VesselChunk(index);
		chunks.Set(index.x, index.y, temp);

		aabb.FitWhole(index);

		return temp;
	}
	void Vessel::AddModifiedChunk(VesselChunk* vc)
	{
		vc->modified = true;

		if (modifiedChunks.size() != 0) {
			for (int i = 0; i < modifiedChunks.size(); i++) {
				if (modifiedChunks[i] == vc)
				{
					vc = modifiedChunks[0];
					modifiedChunks[0] = modifiedChunks[i];
					modifiedChunks[i] = vc;
					return;
				}
			}

			int j = modifiedChunks.size();
			modifiedChunks.push_back(vc);
			vc = modifiedChunks[0];
			modifiedChunks[0] = modifiedChunks[j];
			modifiedChunks[j] = vc;
		}
		else {
			modifiedChunks.push_back(vc);
		}
	}
	void Vessel::SetTile(glm::ivec2 index, VesselTile* val)
	{
		glm::ivec2 index2 = index;
		glm::ivec2 chunkI = TileIToChunkI(index);
		VesselChunk* vc = chunks.TryGet(chunkI);

		if (vc == NULL) {
			vc = CreateChunk(chunkI);
		}

		try {
			index -= vc->OriginTileIndex();
			vc->SetTile(index, val);

			AddModifiedChunk(vc);
		}
		catch (std::exception ex) {
			//Debug.Log("Failed to set tile at " + index2.ToString() + "/" + index.ToString() + " at chunk " + chunkI.ToString());
		}
	}
	void Vessel::BuildWall(glm::ivec2 index, int count, uint8_t type)
	{
		VesselTile* tile;

		for (int i = 0; i < count; i++) {

			if (IsWallLegal(index, type)) {

				tile = TryGetTile(index);
				if (tile == NULL) {
					tile = new VesselTile();
					SetTile(index, tile);
				}

				tile->wallMask |= (byte)(1 << ((byte)type - 1));
				tile->flags |= TileFlag::TileFlag::WallNode;

				index += wallOffsets[(byte)type];

				tile = TryGetTile(index);
				if (tile == NULL) {
					tile = new VesselTile();
					SetTile(index, tile);
				}
				tile->flags |= TileFlag::TileFlag::WallNode;
			}
			else {
				index += wallOffsets[(byte)type];
			}
		}
	}
	void Vessel::BuildWall(glm::ivec2 index, int count, uint8_t type, bool reverse)
	{
		if (reverse) {
			index -= wallOffsets[type] * count;
		}

		BuildWall(index, count, type);

		if (!reverse) {
			index += wallOffsets[type] * count;
		}
	}
	void Vessel::BuildWall(glm::ivec2* index, int count, uint8_t type, bool reverse)
	{
		if (reverse) {
			*index -= wallOffsets[type] * count;
		}

		BuildWall(*index, count, type);

		if (!reverse) {
			*index += wallOffsets[type] * count;
		}
	}
	void Vessel::FillTile(AdjacentTiles* t)
	{
		if ((t->tile->flags & TileFlag::TileFlag::SolidBlock) > 0) {
			t->tile->c0 = NULL;
			t->tile->c1 = NULL;
		}

		//take the compartments from left or bottom no matter what in these cases
		if ((t->bTile != NULL && t->bTile->ContainsMask(WallTypeMask::WallTypeMask::OneByTwo)) || (t->brTile != NULL && t->brTile->ContainsMask(WallTypeMask::WallTypeMask::OneByTwoFlipped))) {
			t->tile->c0 = t->bTile->c0->Instance();
			t->tile->c1 = t->bTile->c1->Instance();
			return;
		}
		if ((t->lTile != NULL && t->lTile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOne)) || (t->rTile != NULL && t->rTile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOneFlipped))) {
			t->tile->c0 = t->lTile->c0->Instance();
			t->tile->c1 = t->lTile->c1->Instance();
			return;
		}
		//

		//try take from bottom
		Compartment* b = NULL;
		if (t->bTile != NULL) {
			if ((t->bTile->flags & TileFlag::TileFlag::SolidBlock) > 0)
				b = NULL;
			else if (!t->tile->ContainsMask(WallTypeMask::WallTypeMask::OneByZero)) {
				//set the appropriate compartment field depending on how "bTile" is cut
				if (
					(t->blTile != NULL && t->blTile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOne)) ||
					(t->bTile->ContainsMask(WallTypeMask::WallTypeMask::OneByOne)) ||
					(t->b2Tile != NULL && t->b2Tile->ContainsMask(WallTypeMask::WallTypeMask::OneByTwo))) {
					b = t->bTile->c1->Instance();
					//} else if (
					//	(t->b2rTile != NULL && t->b2rTile->ContainsMask(WallTypeMask::WallTypeMask::OneByTwoFlipped)) ||
					//	(t->brTile != NULL && t->brTile->ContainsMask(WallTypeMask::WallTypeMask::OneByOneFlipped)) ||
					//	(t->br2Tile != NULL && t->br2Tile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOneFlipped))){
					//	b = t->bTile.c0->Instance();
				}
				else {
					b = t->bTile->c0->Instance();
				}
			}
		}
		Compartment* l = NULL;
		if (t->lTile != NULL) {
			if ((t->lTile->flags & TileFlag::TileFlag::SolidBlock) > 0)
				l = NULL;
			else if (!t->tile->ContainsMask(WallTypeMask::WallTypeMask::ZeroByOne)) {
				l = t->lTile->c0->Instance();
			}
		}

		if (
			(t->r2Tile != NULL && t->r2Tile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOneFlipped)) ||
			(t->rTile != NULL && t->rTile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::OneByOneFlipped | WallTypeMask::WallTypeMask::OneByTwoFlipped)))) {
			if (l != NULL && b != NULL) {
				l->Combine(b);
			}
			if (b != NULL) {
				t->tile->c1 = b;
			}
			else if (l != NULL) {
				t->tile->c1 = l;
			}

			t->tile->c0 = new Compartment(nextCompartmentIndex++);

			return;
		}
		else if (t->tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::TwoByOne | WallTypeMask::WallTypeMask::OneByOne | WallTypeMask::WallTypeMask::OneByTwo))) {
			if (b == NULL) {
				b = new Compartment(nextCompartmentIndex++);
			}
			t->tile->c0 = b;
			if (l == NULL) {
				l = new Compartment(nextCompartmentIndex++);
			}
			t->tile->c1 = l;
			return;
		}
		else {
			if (l != NULL && b != NULL) {
				l->Combine(b);
			}

			l = (l == NULL) ? b : l;
			if (l == NULL) {
				l = new Compartment(nextCompartmentIndex++);
			}
			t->tile->c0 = l->Instance();
		}
	}
	void Vessel::RebuildCompartments()
	{
		glm::ivec2 start = ChunkIToTileI(aabb.bl);
		glm::ivec2 end = ChunkIToTileI(aabb.tr);

		AdjacentTiles* at = new AdjacentTiles(this);

		for (int i = start.y; i < end.y; i++) {

			at->Reset(glm::ivec2(start.x, i));

			for (int j = start.x; j < end.x; j++) {
				if (at->tile != NULL) {

					FillTile(at);

				}
				at->MoveRight();
			}
		}

		return;
	}
	Compartment* Vessel::CompartmentAt(glm::vec2 world)
	{
		glm::ivec2 tileI = WorldToTileI(world);
		glm::vec2 diff = world - TileIToWorld(tileI);

		VesselTile* oTile = TryGetTile(tileI);
		VesselTile* tile = oTile;

		if (tile != NULL) {
			for (int i = (int)WallType::WallType::TwoByOne; i < (int)WallType::WallType::ZeroByOne; i++) {
				if (tile->Contains((WallType::WallType)i)) {
					if (glm::dot(diff, glm::vec2(-wallOffsets[i].y, wallOffsets[i].x)) > 0.0f) {
						return oTile->c1->Instance();
					}
					else {
						return oTile->c0->Instance();
					}
				}
			}

			glm::ivec2 tempI = glm::ivec2(tileI.x, tileI.y - 1);
			if ((tile = TryGetTile(tempI)) != NULL && tile->Contains(WallType::WallType::OneByTwo)) {
				diff = world - TileIToWorld(tempI);
				if (glm::dot(diff, glm::vec2(-wallOffsets[(int)WallType::WallType::OneByTwo].y, wallOffsets[(int)WallType::WallType::OneByTwo].x)) > 0.0f) {
					return oTile->c1->Instance();
				}
				else {
					return oTile->c0->Instance();
				}
			}
			tempI = glm::ivec2(tileI.x + 1, tileI.y - 1);
			if (((tile = TryGetTile(tempI))) != NULL && tile->Contains(WallType::WallType::OneByTwoFlipped)) {
				diff = world - TileIToWorld(tempI);
				if (glm::dot(diff, glm::vec2(-wallOffsets[(int)WallType::WallType::OneByTwoFlipped].y, wallOffsets[(int)WallType::WallType::OneByTwoFlipped].x)) > 0.0f) {
					return oTile->c1->Instance();
				}
				else {
					return oTile->c0->Instance();
				}
			}
			tempI = glm::ivec2(tileI.x + 1, tileI.y);
			if ((tile = TryGetTile(tempI)) != NULL && tile->Contains(WallType::WallType::OneByOneFlipped)) {
				diff = world - TileIToWorld(tempI);
				if (glm::dot(diff, glm::vec2(-wallOffsets[(int)WallType::WallType::OneByOneFlipped].y, wallOffsets[(int)WallType::WallType::OneByOneFlipped].x)) > 0.0f) {
					return oTile->c1->Instance();
				}
				else {
					return oTile->c0->Instance();
				}
			}

			tempI = glm::ivec2(tileI.x - 1, tileI.y);
			if ((tile = TryGetTile(tempI)) != NULL && tile->Contains(WallType::WallType::TwoByOne)) {
				diff = world - TileIToWorld(tempI);
				if (glm::dot(diff, glm::vec2(-wallOffsets[(int)WallType::WallType::TwoByOne].y, wallOffsets[(int)WallType::WallType::TwoByOne].x)) > 0.0f) {
					return oTile->c1->Instance();
				}
				else {
					return oTile->c0->Instance();
				}
			}

			for (int i = 1; i < 3; i++) {
				tempI = glm::ivec2(tileI.x + i, tileI.y);
				if ((tile = TryGetTile(tempI)) != NULL && tile->Contains(WallType::WallType::TwoByOneFlipped)) {
					diff = world - TileIToWorld(tempI);
					if (glm::dot(diff, glm::vec2(-wallOffsets[(int)WallType::WallType::TwoByOneFlipped].y, wallOffsets[(int)WallType::WallType::TwoByOneFlipped].x)) > 0.0f) {
						return oTile->c1->Instance();
					}
					else {
						return oTile->c0->Instance();
					}
				}
			}

			return oTile->c0->Instance();
		}

		return NULL;
	}
	void Vessel::SetCompartmentFloor(uint8_t type, Compartment* c)
	{
		glm::ivec2 start = ChunkIToTileI(aabb.bl);
		glm::ivec2 end = ChunkIToTileI(aabb.tr);

		for (int i = start.y; i < end.y; i++) {
			for (int j = start.x; j < end.x; j++) {
				glm::ivec2 tileI = glm::ivec2(j, i);
				VesselTile* tile;
				if ((tile = TryGetTile(tileI)) != NULL) {
					bool modified = false;
					if (tile->c0 != NULL && tile->c0->Instance() == c) {
						tile->floor0 = type;
						modified = true;
					}
					if (tile->c1 != NULL && tile->c1->Instance() == c) {
						tile->floor1 = type;
						modified |= true;
					}

					if (modified) {
						AddModifiedChunk(chunks.Get(TileIToChunkI(tileI)));
					}
				}
			}
		}
	}
	VesselObject* Vessel::PlaceObject(ObjectType::ObjectType type, glm::vec2 location)
	{
		VesselObject* obj = new VesselObject(type, location);

		return obj;
	}

	bool Vessel::IsWallLegal(glm::ivec2 index, uint8_t type)
	{
		VesselTile* tile = TryGetTile(index);
		if (!tile)
			return false;

		/*if ((tile = TryGetTile(index)) != NULL && tile->blockT != ObjectType.None) {
			return false;
		}
		if ((tile = TryGetTile(glm::ivec2(index.x - 1, index.y))) != NULL  && tile->blockT != ObjectType.None) {
			return false;
		}*/

		//check if walls nodes are not too close
		//to the new wall
		if (!(type == WallType::WallType::OneByZero || type == WallType::WallType::ZeroByOne)) {

			int hDir = (type < WallType::WallType::ZeroByOne) ? 1 : -1;
			int diff = abs(type - WallType::WallType::ZeroByOne);

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
	bool Vessel::ContainsWall(glm::ivec2 index)
	{
		VesselTile* tile = TryGetTile(index);

		if (tile != NULL) {
			if (tile->flags & TileFlag::TileFlag::WallNode) {
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
			if (tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::OneByOne | WallTypeMask::WallTypeMask::TwoByOne | WallTypeMask::WallTypeMask::OneByTwo))) {
				return true;
			}
		}
		//1
		tile = TryGetTile(glm::ivec2(index.x, index.y - 1));
		if (tile != NULL) {
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
		tile = TryGetTile(glm::ivec2(index.x + 1, index.y));
		if (tile != NULL) {
			if (tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::OneByOneFlipped | WallTypeMask::WallTypeMask::TwoByOneFlipped | WallTypeMask::WallTypeMask::OneByTwoFlipped))) {
				return true;
			}
		}
		//3
		tile = TryGetTile(glm::ivec2(index.x - 1, index.y - 1));
		if (tile != NULL) {
			if (tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::OneByTwo | WallTypeMask::WallTypeMask::TwoByOne))) {
				return true;
			}
		}
		//4
		tile = TryGetTile(glm::ivec2(index.x + 1, index.y - 1));
		if (tile != NULL) {
			if (tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::TwoByOneFlipped | WallTypeMask::WallTypeMask::OneByTwoFlipped))) {
				return true;
			}
		}
		//5
		tile = TryGetTile(glm::ivec2(index.x - 2, index.y));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOne)) {
				return true;
			}
		}
		//6
		tile = TryGetTile(glm::ivec2(index.x + 2, index.y));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOneFlipped)) {
				return true;
			}
		}
		//7
		tile = TryGetTile(glm::ivec2(index.x, index.y - 2));
		if (tile != NULL) {
			if (tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::OneByTwoFlipped | WallTypeMask::WallTypeMask::OneByTwo))) {
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

	bool Vessel::EmptyTile(glm::ivec2 loc)
	{
		VesselTile* tile;

		tile = TryGetTile(loc);
		if (tile != NULL) {
			if (tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::OneByOne | WallTypeMask::WallTypeMask::TwoByOne | WallTypeMask::WallTypeMask::OneByTwo))) {
				return false;
			}
		}
		tile = TryGetTile(glm::ivec2(loc.x - 1, loc.y));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOne)) {
				return false;
			}
		}
		tile = TryGetTile(glm::ivec2(loc.x + 1, loc.y));
		if (tile != NULL) {
			if (tile->ContainsMask((WallTypeMask::WallTypeMask)(WallTypeMask::WallTypeMask::OneByOneFlipped | WallTypeMask::WallTypeMask::TwoByOneFlipped))) {
				return false;
			}
		}
		tile = TryGetTile(glm::ivec2(loc.x + 2, loc.y));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask::WallTypeMask::TwoByOneFlipped)) {
				return false;
			}
		}
		tile = TryGetTile(glm::ivec2(loc.x, loc.y - 1));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask::WallTypeMask::OneByTwo)) {
				return false;
			}
		}
		tile = TryGetTile(glm::ivec2(loc.x + 1, loc.y - 1));
		if (tile != NULL) {
			if (tile->ContainsMask(WallTypeMask::WallTypeMask::OneByTwoFlipped)) {
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
		if (wall1 < WallType::WallType::ZeroByOne) {
			if ((uint8_t )wall0 > 4 + (uint8_t )wall1) {
				return false;
			}
		}
		else if (wall1 > WallType::WallType::ZeroByOne) {
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