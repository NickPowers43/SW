#pragma once
#include "Vessel.h"


namespace SW_Server
{
	class StartingVessel :
		public Vessel
	{
	public:
		StartingVessel(VesselIndex_t index, glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 rot, Vessel* next);
		~StartingVessel();

		virtual void PlaceObject(ObjectType_t type, glm::ivec2 location) override;
		void AddPlayer(NetworkWriter* nw, Player* player);

		void Initialize();

	private:

		std::vector<glm::ivec2> spawnPoints;
	};

}
