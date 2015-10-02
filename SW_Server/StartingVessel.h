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

		virtual SW::VesselObject* PlaceObject(ObjectType::ObjectType type, glm::vec2 location) override;
		void AddPlayer(NetworkWriter* nw, Player* player);

		void Initialize();

	private:

		std::vector<SW::VesselObject*> spawnPoints;
	};

}
