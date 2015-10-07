#include "Vessel.h"
#include "SW_Client.h"
#include <SW/CompartmentTile.h>
#include <SW/FixedCompartmentTileSet.h>
#include <sstream>
#include <SW/Tile.h>
#include <SW/SW.h>
#include <SW/AABBi.h>
#include "Player.h"
#include "BufferedMeshArray.h"


namespace SW_Client
{
	Vessel::Vessel(VesselIndex_t index, glm::vec3 vel, float m, glm::vec3 pos, glm::vec3 rot) : SW::Vessel(index, vel, m, pos, rot)
	{
	}
	Vessel::~Vessel()
	{
	}

	void Vessel::Step(NetworkWriter* nw)
	{
		StepXZ();
	}

	
	//point between vectors if considering CCW convention

	

}