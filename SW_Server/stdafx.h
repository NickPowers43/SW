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

#include "NetworkWriter.h"
#include "Player.h"
#include "StartingVessel.h"

using namespace std;
using namespace SW_Server;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

typedef websocketpp::server<websocketpp::config::asio> server;
typedef server::message_ptr message_ptr;

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
		AddPlayer = 4
	};
}

namespace ClientMessageType
{
	enum ClientMessageType : uint8_t {
		RequestChunk = 0,
		Inputs = 1,
		FillAt = 2
	};
}

// TODO: reference additional headers your program requires here
