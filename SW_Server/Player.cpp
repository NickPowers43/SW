#include "stdafx.h"
#include "Player.h"
#include <SW\NetworkReader.h>
#include <SW\NetworkWriter.h>
#include <glm/glm.hpp>

using namespace SW;

namespace SW_Server
{
	Player::Player(websocketpp::connection_hdl hdl, glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 rot, Vessel* currentVessel) :
		SW::Player(vel, m, pos, rot), 
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
	void Player::ProcessInputs(NetworkReader* nr)
	{
		inputs = nr->ReadUint16();
		rot.x = nr->ReadSingle();
		rot.y = nr->ReadSingle();
	}
	void Player::WriteUpdateMessage(NetworkWriter* nw)
	{
		if (nw->Remaining() < ((sizeof(float) * 2) + sizeof(MessageType_t)))
		{
			FlushBuffer(nw);
		}

		nw->Write((MessageType_t)ServerMessageType::UpdatePlayer);
		nw->Write((float)pos.x);
		nw->Write((float)pos.z);
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
