#pragma once
#include "Vessel.h"


namespace SW_Server
{
	class StartingVessel :
		public Vessel
	{
	public:
		StartingVessel(SW::VesselIndex_t index, VesselVecType vel, VesselValueType m, VesselVecType pos, VesselValueType rot, Vessel* next);
		~StartingVessel();

		virtual SW::VesselObject* PlaceObject(ObjectType::ObjectType type, glm::vec2 location) override;
		void AddPlayer(NetworkWriter* nw, Player* player);

		void Initialize();

	private:

		std::vector<SW::VesselObject*> spawnPoints;
	};

}
