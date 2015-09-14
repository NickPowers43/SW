#include "stdafx.h"
#include "Player.h"
#include <SW\NetworkReader.h>
#include <SW\NetworkWriter.h>

using namespace SW;

namespace SW_Server
{
	Player::Player(websocketpp::connection_hdl hdl, glm::vec2 vel, float m, glm::vec2 pos, float rot, glm::ivec2 chunkI, Vessel* currentVessel) :
		SW::Player(vel, m, pos, rot, chunkI), 
		hdl(hdl), currentVessel(currentVessel)
	{
	}


	Player::~Player()
	{
	}

	void Player::SendString(const std::string message)
	{
		try
		{
			myServer->send(hdl, message, websocketpp::frame::opcode::binary);
		}
		catch (const websocketpp::lib::error_code& e) {
			std::cout << "Failed to send string because: " << e << "(" << e.message() << ")" << std::endl;
			throw websocketpp::lib::error_code(e);
		}
	}
	void Player::JustSendBuffer(NetworkWriter* nw)
	{
		try
		{
			if (nw->Position() > 0)
			{
				myServer->send(hdl, nw->buffer, nw->Position(), websocketpp::frame::opcode::binary);
				//nw->Reset();
			}
		}
		catch (const websocketpp::lib::error_code& e) {
			std::cout << "Failed to flush buffer because: " << e << "(" << e.message() << ")" << std::endl;
			throw websocketpp::lib::error_code(e);
		}
	}
	void Player::FlushBuffer(NetworkWriter* nw)
	{
		try
		{
			if (nw->Position() > 0)
			{
				myServer->send(hdl, nw->buffer, nw->Position(), websocketpp::frame::opcode::binary);
				nw->Reset();
			}
		}
		catch (const websocketpp::lib::error_code& e) {
			std::cout << "Failed to flush buffer because: " << e << "(" << e.message() << ")" << std::endl;
			throw websocketpp::lib::error_code(e);
		}
	}
}
