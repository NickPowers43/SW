#pragma once
#include <SW/Player.h>
#include "SW_Client.h"

#define PLAYER_EYE_HEIGHT 

namespace SW_Client
{
	class Player :
		public SW::Player
	{
	public:
		Player(glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 rot);
		~Player();

		void ReadUpdateMessage(NetworkReader* nr);
		void ProcessInputs(NetworkWriter* nw);
	};
}

