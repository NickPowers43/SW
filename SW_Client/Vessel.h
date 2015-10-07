#pragma once
#include "TileChunks.h"
#include "NetworkReader.h"
#include "NetworkWriter.h"
#include <SW/AABBi.h>
#include <SW/SW.h>
#include <SW/Vessel.h>
#include "Player.h"

namespace SW_Client
{
	class Vessel :
		public SW::Vessel
	{
	public:
		Vessel(VesselIndex_t index, glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 rot);
		~Vessel();

		TileChunks tiles;

		void Step(NetworkWriter* nw);
	};

}
