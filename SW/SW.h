#pragma once

#include <glm/geometric.hpp>
#include <glm/common.hpp>
#include <glm/vec2.hpp>

#define FloorCount 3
#define MODULE_TYPE_COUNT 2
#define WallTypeCount 9

#define CHUNK_SIZE 8
#define CHUNK_DATA_COUNT (CHUNK_SIZE * CHUNK_SIZE)
#define PLAYER_CHUNK_RANGE 2

#define WALL_THICKNESS 0.08f
#define HALF_WALL_THICKNESS (WALL_THICKNESS* 0.5f)

typedef uint8_t MessageType_t;

typedef uint8_t ChunkFlag_t;

typedef uint16_t TileFlag_t;
typedef uint8_t FloorType_t;
typedef uint8_t WallType_t;
typedef uint8_t WallTypeMask_t;
typedef uint8_t VesselControlMode_t;

typedef uint8_t ObjectType_t;

typedef uint32_t VesselIndex_t;
typedef uint32_t CompartmentIndex_t;

typedef uint8_t VMFlags_t;
typedef uint16_t VMType_t;
typedef uint16_t InputFlags_t;

typedef uint32_t TileChunkVersion_t;

#define INPUT_FLAG_W 1
#define INPUT_FLAG_A 2
#define INPUT_FLAG_S 4
#define INPUT_FLAG_D 8

extern float deltaTime;
extern float elapsedTime;

namespace SW
{
	typedef float VesselValueType;
	typedef glm::tvec2<float> VesselVecType;

	static int CHUNK_SIZE_POW = 3;
	static int CHUNK_OFFSET_MASK = (1 << (CHUNK_SIZE_POW + 1)) - 1;

	static float CHUNK_UPDATE_INTERVAL = 1.0f;

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

	class RigidBody;
	class GameObject;

	template<typename T>
	class LinkedListNode;
	template<typename T>
	class DynamicArray2D;

	//Server message sizes
	static int MAX_MODULE_MESSAGE_SIZE = sizeof(VMType_t) + (2 * sizeof(uint32_t));
	static int TILE_MESSAGE_SIZE =
		sizeof(TileFlag_t) +
		sizeof(WallTypeMask_t) +
		(sizeof(FloorType_t) * 2);

	void Initialize();

	extern VesselModuleTemplate** vesselModuleTemplates;
	extern float* wallRotations;
	extern float* wallMagnitudes;
	extern glm::ivec2* wallOffsets;
	extern glm::vec2* wallVectorsNormalized;
}

void swap_16(void* val);
void swap_32(void* val);

namespace VMFlags
{
	enum VMFlags : VMType_t {
		None = 0,
		FlipVertical = 1,
		FlipHorizontal = 2
	};
}

namespace VMType
{
	enum VMType : VMType_t {
		SimpleFrigateCore = 0,
		SimpleFrigateBridge = 1
	};
}

namespace TileFlag
{
	enum TileFlag : TileFlag_t {
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

namespace ObjectType
{
	enum ObjectType : ObjectType_t {
		None = 0,
		Spawner = 1,
		Turret = 2
	};
}

namespace FloorType
{
	enum FloorType : FloorType_t {
		None = 0,
		Basic = 1,
		SmoothWhite = 2
	};
}

namespace ClientMessageType
{
	enum ClientMessageType : MessageType_t {
		RequestTiles = 0,
		Inputs = 1,
		FillAt = 2,
		PingMessageResponse = 3,
		RequestModule = 4,
		RequestPlayerInfo = 5
	};
}

namespace ServerMessageType
{
	enum ServerMessageType : MessageType_t {
		SetTiles = 0,
		SyncVessel = 1,
		UpdatePlayer = 2,
		RemovePlayer = 3,
		AddPlayer = 4,
		AddYourself = 5,
		MakeVesselActive = 6,
		PingMessage = 7,
		SetModule = 8,
		EndianessCheck = 9,
		SetPlayerInfo = 10
	};
}

namespace WallTypeMask
{
	enum WallTypeMask : WallTypeMask_t {
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
	enum WallType : WallType_t {
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

namespace VesselControlMode
{
	enum VesselControlMode : VesselControlMode_t {
		None = 0,
		Pilot = 1,
		Gunner = 2,
	};
}

template<typename value_type>
value_type abs(value_type val)
{
	if (val < 0)
	{
		return -val;
	}
	else
	{
		return val;
	}
}

float Determinant(glm::vec2 c0, glm::vec2 c1);
glm::vec2 WallCorner(glm::vec2 wall0, glm::vec2 wall1);
WallType_t CCWReverseWallSweepOpposite(int start, int stop, SW::TileSet* ts, glm::ivec2 location, WallType_t type);
WallType_t CCWReverseWallSweepLocal(int start, int stop, SW::TileSet* ts, glm::ivec2 location, WallType_t type);
WallType_t CCWWallSweepOpposite(int start, int stop, SW::Tile* orgtile, WallType_t type);
WallType_t CCWWallSweepLocal(int start, int stop, SW::Tile* orgtile, WallType_t type);
glm::vec2 Rotate90CCW(glm::vec2 v);
glm::vec2 Rotate90CW(glm::vec2 v);
bool WallVertexSweepCCW(SW::TileSet* ts, SW::Tile* orgtile, glm::ivec2 location, WallType_t type, bool end, glm::vec2 & v);
bool WallVertexSweepCW(SW::TileSet* ts, SW::Tile* orgtile, glm::ivec2 location, WallType_t type, bool end, glm::vec2 & v);