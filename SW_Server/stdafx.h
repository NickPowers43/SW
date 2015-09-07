// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <iostream>
#include <map>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp\server.hpp>

#include <glm/geometric.hpp>
#include <glm\common.hpp>
#include <glm\vec2.hpp>

using namespace std;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

typedef websocketpp::server<websocketpp::config::asio> server;
typedef server::message_ptr message_ptr;

namespace SW_Server
{
	typedef double VesselValueType;
	typedef glm::tvec2<double> VesselVecType;

	class VesselTile;
	typedef uint8_t MessageType_t;
	typedef uint16_t TileFlag_t;
	typedef uint8_t FloorType_t;
	typedef uint8_t WallType_t;
	typedef uint8_t WallTypeMask_t;
	typedef uint32_t CompartmentIndex_t;
	typedef uint8_t VMFlags_t;
	typedef uint16_t VMType_t;

	static int MAX_VESSELS_PER_NODE = 4;
	static int QT_MAX_DEPTH = 14;
	static int QT_WORKING_DEPTH = 10;

	static int PLAYER_CHUNK_RANGE = 2;
	static int CHUNK_SIZE_POW = 3;
	static int CHUNK_SIZE = 1 << CHUNK_SIZE_POW;
	static int CHUNK_DATA_COUNT = CHUNK_SIZE * CHUNK_SIZE;
	static int CHUNK_OFFSET_MASK = (1 << (CHUNK_SIZE_POW + 1)) - 1;

	static VesselValueType WORLD_SIZE = 10000000000.0;

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

	static int MAX_MODULE_MESSAGE_SIZE = sizeof(VMType_t) + (2 * sizeof(uint32_t));

	class Vessel;
	class Player;
	class StartingVessel;
	class NetworkReader;
	class NetworkWriter;
	class AABBi;
	class VesselTile;
	class VesselChunk;
	class VesselObject;
	template<typename value_type>
	class RigidBody;
	class VesselModule;
	class AdjacentTiles;
	class Compartment;
	class VesselModule;
	class VesselModuleTemplate;
	class QTNode;
	class SpacialQTNode;
	class WorldQTNode;

	template<typename T>
	class LinkedListNode;
	template<typename T>
	class DynamicArray2D;

	void Initialize();
}

using namespace SW_Server;

#define MODULE_TYPE_COUNT 2

extern WorldQTNode* qt;
extern VesselModuleTemplate* moduleTemplates;
extern glm::ivec2* wallOffsets;
extern glm::vec2* wallVectorsNormalized;
extern server myServer;
extern std::map<void*, Player*> players;
extern std::vector<Vessel*> vessels;
extern std::vector<StartingVessel*> startingVessels;

namespace ServerMessageType
{
	enum ServerMessageType : uint8_t {
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

namespace VMFlags
{
	enum VMFlags : uint8_t {
		None = 0,
		FlipVertical = 1,
		FlipHorizontal = 2
	};
}

namespace VMType
{
	enum VMType : uint16_t {
		SimpleFrigateCore = 0,
		SimpleFrigateBridge = 1
	};
}

namespace ClientMessageType
{
	enum ClientMessageType : uint8_t {
		RequestChunk = 0,
		Inputs = 1,
		FillAt = 2,
		PingMessageResponse = 3,
		RequestModule = 4
	};
}

namespace TileFlag
{
	enum TileFlag : uint16_t {
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

namespace WallTypeMask
{
	enum WallTypeMask : uint8_t {
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
	enum WallType : uint8_t {
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

namespace ObjectType
{
	enum ObjectType : uint8_t {
		Spawner = 0,
		Turret = 1
	};
}

namespace FloorType
{
	enum FloorType : uint8_t {
		None = 0,
		Basic = 1,
		SmoothWhite = 2
	};
}


