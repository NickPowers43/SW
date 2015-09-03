#pragma once
namespace SW_Server
{
	class Vessel;

	class Player
	{
	public:
		Player();
		~Player();

		Vessel* currentVessel;
		glm::vec2 pos;
		glm::ivec2 chunkI;
		float rot;
	};

}
