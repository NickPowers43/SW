#include "Player.h"
#include <SDL/SDL_keycode.h>
#include "NetworkWriter.h"
#include "NetworkReader.h"

namespace SW_Client
{
	Player::Player(glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 rot) : SW::Player(vel, m, pos, rot)
	{
	}


	Player::~Player()
	{
	}

	void Player::ReadUpdateMessage(NetworkReader* nr)
	{
		pos.x = nr->ReadSingle();
		pos.z = nr->ReadSingle();
	}
	void Player::ProcessInputs(NetworkWriter* nw)
	{
		if (keyStates[SDLK_w] || keyStates[SDLK_a] || keyStates[SDLK_s] || keyStates[SDLK_d])
		{
			InputFlags_t inputs = 0;
			if (keyStates[SDLK_w])
			{
				inputs |= INPUT_FLAG_W;
			}
			if (keyStates[SDLK_s])
			{
				inputs |= INPUT_FLAG_S;
			}
			if (keyStates[SDLK_a])
			{
				inputs |= INPUT_FLAG_A;
			}
			if (keyStates[SDLK_d])
			{
				inputs |= INPUT_FLAG_D;
			}
			myPlayer->inputs = inputs;
		}

		nw->WriteMessageType(ClientMessageType::Inputs);
		nw->WriteUint16(inputs);
		nw->WriteSingle(myPlayer->rot.x);
		nw->WriteSingle(myPlayer->rot.y);
	}
}
