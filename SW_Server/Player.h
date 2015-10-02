#pragma once
#include "Vessel.h"
#include <SW\SW.h>
#include <SW\Player.h>

namespace SW_Server
{
	class Player :
		public SW::Player
	{
	public:
		Player(websocketpp::connection_hdl hdl, glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 rot, Vessel* currentVessel);
		~Player();

		void SendString(const std::string message);
		void JustSendBuffer(NetworkWriter* nw);
		void ProcessInputs(NetworkReader* nr);
		void FlushBuffer(NetworkWriter* nw);
		void Step();

		InputFlags_t inputs;
		Vessel* currentVessel;
		websocketpp::connection_hdl hdl;
	};
}

