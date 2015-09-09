#pragma once

#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <map>

#include <glm/geometric.hpp>
#include <glm/common.hpp>
#include <glm/vec2.hpp>

using namespace std;

namespace SW
{
	typedef float VesselValueType;
	typedef glm::tvec2<float> VesselVecType;

	static int PLAYER_CHUNK_RANGE = 2;
	static int CHUNK_SIZE_POW = 3;
	static int CHUNK_SIZE = 1 << CHUNK_SIZE_POW;
	static int CHUNK_DATA_COUNT = CHUNK_SIZE * CHUNK_SIZE;
	static int CHUNK_OFFSET_MASK = (1 << (CHUNK_SIZE_POW + 1)) - 1;

	static int MODULE_TYPE_COUNT = 2;

	static float CHUNK_UPDATE_INTERVAL = 1.0f;

	typedef uint8_t MessageType_t;

	typedef uint8_t ChunkFlag_t;

	typedef uint16_t TileFlag_t;
	typedef uint8_t FloorType_t;
	typedef uint8_t WallType_t;
	typedef uint8_t WallTypeMask_t;

	typedef uint8_t ObjectType_t;

	typedef uint32_t VesselIndex_t;
	typedef uint32_t CompartmentIndex_t;

	typedef uint8_t VMFlags_t;
	typedef uint16_t VMType_t;

	typedef uint32_t TileChunkVersion_t;

	class Vessel;
	class Player;
	class Tile;
	class CompartmentTile;
	class TileChunk;
	class TileChunks;
	class FixedTileSet;
	class TrackedTileSet;
	class CompartmentTileSet;
	class TileSet;
	class AABBi;
	class VesselObject;
	class Compartment;
	class VesselModule;
	class AdjacentTiles;
	class VesselModuleTemplate;
	class VesselModuleTileSet;
	class NetworkReader;
	class NetworkWriter;

	template<typename T>
	class RigidBody;
	template<typename T>
	class LinkedListNode;
	template<typename T>
	class DynamicArray2D;

	static int MAX_MODULE_MESSAGE_SIZE = sizeof(VMType_t) + (2 * sizeof(uint32_t));
	static int VESSEL_TILE_MESSAGE_SIZE =
		sizeof(uint16_t) + /*index*/
		sizeof(TileFlag_t) +
		sizeof(WallTypeMask_t) +
		(sizeof(FloorType_t) * 2) +
		(sizeof(CompartmentIndex_t) * 2);
	static int MAX_VESSELCHUNK_MESSAGE_SIZE =
		(sizeof(int16_t) * 2) + /*index*/
		sizeof(uint32_t) + /*version*/
		sizeof(uint16_t) + /*tile_count*/
		(CHUNK_DATA_COUNT * VESSEL_TILE_MESSAGE_SIZE) /*tiles*/;

	void Initialize();

	extern VesselModuleTemplate** vesselModuleTemplates;
	extern glm::ivec2* wallOffsets;
	extern glm::vec2* wallVectorsNormalized;
}

void swap_16(void* val);
void swap_32(void* val);

namespace VMFlags
{
	enum VMFlags : SW::VMType_t {
		None = 0,
		FlipVertical = 1,
		FlipHorizontal = 2
	};
}

namespace VMType
{
	enum VMType : SW::VMType_t {
		SimpleFrigateCore = 0,
		SimpleFrigateBridge = 1
	};
}

namespace TileFlag
{
	enum TileFlag : SW::TileFlag_t {
		None = 0,
		Hatch0 = 1,
		Hatch1 = 2,
		SolidBlock = 4,
		WallNode = 8,
		Lock0 = 16,
		Lock1 = 32,
		Ignore = 64
	};
}

namespace ChunkFlag
{
	enum ChunkFlag : SW::ChunkFlag_t {
		None = 0,
		Seen = 1,
		Modified = 2
	};
}

namespace ObjectType
{
	enum ObjectType : SW::ObjectType_t {
		Spawner = 0,
		Turret = 1
	};
}

namespace FloorType
{
	enum FloorType : SW::FloorType_t {
		None = 0,
		Basic = 1,
		SmoothWhite = 2
	};
}

namespace ClientMessageType
{
	enum ClientMessageType : SW::MessageType_t {
		RequestChunk = 0,
		Inputs = 1,
		FillAt = 2,
		PingMessageResponse = 3,
		RequestModule = 4
	};
}

namespace ServerMessageType
{
	enum ServerMessageType : SW::MessageType_t {
		SetChunk = 0,
		SyncVessel = 1,
		UpdatePlayer = 2,
		RemovePlayer = 3,
		AddPlayer = 4,
		AddYourself = 5,
		MakeVesselActive = 6,
		PingMessage = 7,
		SetModule = 8
	};
}

namespace WallTypeMask
{
	enum WallTypeMask : SW::WallTypeMask_t {
		None = 0,
		OneByZero = 1,
		TwoByOne = 2,
		OneByOne = 4,
		OneByTwo = 8,
		ZeroByOne = 16,
		OneByTwoFlipped = 32,
		OneByOneFlipped = 64,
		TwoByOneFlipped = 128
	};
}

namespace WallType
{
	enum WallType : SW::WallType_t {
		None = 0,
		OneByZero = 1,
		TwoByOne = 2,
		OneByOne = 3,
		OneByTwo = 4,
		ZeroByOne = 5,
		OneByTwoFlipped = 6,
		OneByOneFlipped = 7,
		TwoByOneFlipped = 8
	};
}
