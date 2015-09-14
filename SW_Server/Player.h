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
		Player(websocketpp::connection_hdl hdl, glm::vec2 vel, float m, glm::vec2 pos, float rot, glm::ivec2 chunkI, Vessel* currentVessel);
		~Player();

		void SendString(const std::string message);
		void JustSendBuffer(NetworkWriter* nw);
		void FlushBuffer(NetworkWriter* nw);

		Vessel* currentVessel;
		websocketpp::connection_hdl hdl;
	};
}

