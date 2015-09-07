#include "stdafx.h"
#include "Player.h"
#include "Vessel.h"
#include "NetworkWriter.h"


namespace SW_Server
{
	Player::Player(websocketpp::connection_hdl hdl) : hdl(hdl)
	{
		Player::currentVessel = NULL;
		Player::pos = glm::vec2(0.0f, 0.0f);
		Player::chunkI = glm::ivec2(0, 0);
		Player::rot = 0.0f;
	}


	Player::~Player()
	{
	}

	void Player::FlushBuffer(NetworkWriter* nw)
	{
		try
		{
			myServer.send(hdl, nw->StringCopy(), websocketpp::frame::opcode::binary);
		}
		catch (const websocketpp::lib::error_code& e) {
			std::cout << "Failed to flush buffer because: " << e << "(" << e.message() << ")" << std::endl;
			throw websocketpp::lib::error_code(e);
		}
	}
}
