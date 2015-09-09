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

#include <glm/geometric.hpp>
#include <glm\common.hpp>
#include <glm\vec2.hpp>

#include <SW/SW.h>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp\server.hpp>

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

	//typedef SW::Tile Tile;
	typedef SW::NetworkReader NetworkReader;
	typedef SW::NetworkWriter NetworkWriter;

	static int MAX_VESSELS_PER_NODE = 4;
	static int QT_MAX_DEPTH = 14;
	static int QT_WORKING_DEPTH = 10;


	static VesselValueType WORLD_SIZE = 10000000000.0;

	class StartingVessel;
	class WorldQTNode;
	class Player;
	class TileChunk;

	template<typename value_type>
	class SpacialQTNode;

	extern SW::NetworkWriter* nw_main;
	extern WorldQTNode* qt;
	extern server* myServer;

	void SendPingMessage(websocketpp::connection_hdl hdl, SW::NetworkWriter* nw, size_t size);
	void ReadPingMessageResponse(websocketpp::connection_hdl hdl, SW::NetworkWriter* nw, SW::NetworkReader* nr);
}





