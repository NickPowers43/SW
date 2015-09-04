#pragma once

namespace SW_Server
{
	class Vessel;

	class Player
	{
	public:
		Player(websocketpp::connection_hdl hdl);
		~Player();

		websocketpp::connection_hdl hdl;
		Vessel* currentVessel;
		glm::vec2 pos;
		glm::ivec2 chunkI;
		float rot;
	};

}
