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
	void Player::Step()
	{

		if (inputs & (INPUT_FLAG_W | INPUT_FLAG_A | INPUT_FLAG_S | INPUT_FLAG_D))
		{
			glm::vec2 f(0.0f, 0.0f);
			if (inputs & INPUT_FLAG_W)
			{
				f += glm::vec2(0.0f, 1.0f);
			}
			if (inputs & INPUT_FLAG_S)
			{
				f += glm::vec2(0.0f, -1.0f);
			}
			if (inputs & INPUT_FLAG_A)
			{
				f += glm::vec2(-1.0f, 0.0f);
			}
			if (inputs & INPUT_FLAG_D)
			{
				f += glm::vec2(1.0f, 0.0f);
			}

			f = glm::normalize(f) * deltaTime;
			float cos = glm::cos(rot.x);
			float sin = glm::sin(rot.x);
			f = glm::mat2(cos, -sin, sin, cos) * f;

			ApplyForceXZ(f);
			//pos += glm::vec3(translate.x, 0.0f, translate.y);
		}

		StepXZ();
	}
}
