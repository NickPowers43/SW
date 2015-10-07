#include "Player.h"
#include <SDL/SDL_keycode.h>
#include "NetworkWriter.h"
#include "NetworkReader.h"
#include <sstream>

namespace SW_Client
{
	Player::Player(PlayerID_t id, glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 rot) : SW::Player(id, vel, m, pos, rot)
	{
	}


	Player::~Player()
	{
	}

	void Player::ReadUpdateMessage(NetworkReader* nr)
	{
		pos.x = nr->ReadSingle();
		pos.z = nr->ReadSingle();

		/*std::stringstream print;
		print << "pos.x : " << pos.x << "\npos.y : " << pos.y;
		PrintMessage((int)print.str().c_str());*/

		vel = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	void Player::ProcessInputs(NetworkWriter* nw)
	{
		inputs = 0;

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

		nw->WriteMessageType(ClientMessageType::Inputs);
		nw->WriteUint16(inputs);
		nw->WriteSingle(rot.x);
		nw->WriteSingle(rot.y);
	}
}
