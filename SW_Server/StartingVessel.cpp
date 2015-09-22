#include "stdafx.h"
#include "StartingVessel.h"
#include <SW/VesselObject.h>
#include "Compartment.h"


namespace SW_Server
{
	StartingVessel::StartingVessel(VesselIndex_t index, VesselVecType vel, VesselValueType m, VesselVecType pos, VesselValueType rot, Vessel* next) :
		Vessel(index, vel, m, pos, rot, next)
	{
		Initialize();
	}


	StartingVessel::~StartingVessel()
	{
	}

	void StartingVessel::Initialize()
	{
		tiles.BuildFoundation(glm::ivec2(-20, -20), glm::ivec2(40, 40));
		//tip
		//		BuildWall(glm::ivec2(4,13), 1, WallType::WallType::OneByZero);
		//		//outer left
		//		//
		//		BuildWall(glm::ivec2(1,1), 1, WallType::WallType::ZeroByOne);
		//		BuildWall(glm::ivec2(1,2), 1, WallType::WallType::OneByTwo);
		//		BuildWall(glm::ivec2(2,4), 5, WallType::WallType::ZeroByOne);
		//		BuildWall(glm::ivec2(2,9), 1, WallType::WallType::OneByTwo);
		//		BuildWall(glm::ivec2(3,11), 1, WallType::WallType::ZeroByOne);
		//		BuildWall(glm::ivec2(3,12), 1, WallType::WallType::OneByOne);
		//		//outer right
		//		//
		//		BuildWall(glm::ivec2(8,1), 1, WallType::WallType::ZeroByOne);
		//		BuildWall(glm::ivec2(8,2), 1, WallType::WallType::OneByTwoFlipped);
		//		BuildWall(glm::ivec2(7,4), 5, WallType::WallType::ZeroByOne);
		//		BuildWall(glm::ivec2(7,9), 1, WallType::WallType::OneByTwoFlipped);
		//		BuildWall(glm::ivec2(6,11), 1, WallType::WallType::ZeroByOne);
		//		BuildWall(glm::ivec2(6,12), 1, WallType::WallType::OneByOneFlipped);
		//
		//		BuildWall(glm::ivec2(1,1), 7, WallType::WallType::OneByZero);
		//
		//		//inner vertical walls
		//		BuildWall(glm::ivec2(4,4), 7, WallType::WallType::ZeroByOne);
		//		BuildWall(glm::ivec2(5,4), 7, WallType::WallType::ZeroByOne);
		//		//left horizontal walls
		//		BuildWall(glm::ivec2(3,11), 1, WallType::WallType::OneByZero);
		//		BuildWall(glm::ivec2(2,7), 2, WallType::WallType::OneByZero);
		//		BuildWall(glm::ivec2(2,4), 2, WallType::WallType::OneByZero);
		//		//right horizontal walls
		//		BuildWall(glm::ivec2(5,11), 1, WallType::WallType::OneByZero);
		//		BuildWall(glm::ivec2(5,7), 2, WallType::WallType::OneByZero);
		//		BuildWall(glm::ivec2(5,4), 2, WallType::WallType::OneByZero);

		glm::ivec2 loc0 = glm::ivec2(0, -1);
		tiles.BuildWall(&loc0, 1, WallType::WallType::OneByOneFlipped, false);
		tiles.BuildWall(&loc0, 1, WallType::WallType::ZeroByOne, false);
		tiles.BuildWall(&loc0, 1, WallType::WallType::OneByOne, false);
		loc0 += glm::ivec2(1, 0); //tiles.BuildWall(&loc0, 1, WallType::WallType::OneByZero, false);
		tiles.BuildWall(&loc0, 1, WallType::WallType::OneByOneFlipped, true);
		tiles.BuildWall(&loc0, 1, WallType::WallType::ZeroByOne, true);
		tiles.BuildWall(&loc0, 1, WallType::WallType::OneByOne, true);
		tiles.BuildWall(&loc0, 1, WallType::WallType::OneByZero, true);
		loc0 = glm::ivec2(0, 2);
		tiles.BuildWall(&loc0, 4, WallType::WallType::ZeroByOne, false);
		tiles.BuildWall(&loc0, 1, WallType::WallType::OneByOneFlipped, false);
		tiles.BuildWall(&loc0, 1, WallType::WallType::ZeroByOne, false);
		glm::ivec2 circleInner = loc0;
		loc0 += glm::ivec2(0, 3);// tiles.BuildWall(&loc0, 3, WallType::WallType::ZeroByOne, false);
		glm::ivec2 circleOuter = loc0;
		tiles.BuildWall(&loc0, 1, WallType::WallType::ZeroByOne, false);
		tiles.BuildWall(&loc0, 3, WallType::WallType::OneByZero, false);
		tiles.BuildWall(&loc0, 5, WallType::WallType::ZeroByOne, true);
		tiles.BuildWall(&loc0, 1, WallType::WallType::OneByOne, true);
		tiles.BuildWall(&loc0, 4, WallType::WallType::ZeroByOne, true);

		tiles.BuildWall(&circleInner, 4, WallType::WallType::OneByZero, true);
		tiles.BuildWall(&circleInner, 2, WallType::WallType::OneByOne, true);
		tiles.BuildWall(&circleInner, 11, WallType::WallType::ZeroByOne, true);
		tiles.BuildWall(&circleInner, 2, WallType::WallType::OneByOneFlipped, true);
		tiles.BuildWall(&circleInner, 11, WallType::WallType::OneByZero, false);
		tiles.BuildWall(&circleInner, 2, WallType::WallType::OneByOne, false);
		tiles.BuildWall(&circleInner, 11, WallType::WallType::ZeroByOne, false);
		tiles.BuildWall(&circleInner, 2, WallType::WallType::OneByOneFlipped, false);
		tiles.BuildWall(&circleInner, 4, WallType::WallType::OneByZero, true);

		tiles.BuildWall(&circleOuter, 5, WallType::WallType::OneByZero, true);
		tiles.BuildWall(&circleOuter, 4, WallType::WallType::OneByOne, true);
		tiles.BuildWall(&circleOuter, 13, WallType::WallType::ZeroByOne, true);
		tiles.BuildWall(&circleOuter, 4, WallType::WallType::OneByOneFlipped, true);
		tiles.BuildWall(&circleOuter, 13, WallType::WallType::OneByZero, false);
		tiles.BuildWall(&circleOuter, 4, WallType::WallType::OneByOne, false);
		tiles.BuildWall(&circleOuter, 13, WallType::WallType::ZeroByOne, false);
		tiles.BuildWall(&circleOuter, 4, WallType::WallType::OneByOneFlipped, false);
		tiles.BuildWall(&circleOuter, 5, WallType::WallType::OneByZero, true);

		//place spawner for noobs
		PlaceObject(ObjectType::ObjectType::Spawner, glm::vec2(0.0f, 0.0f));

		tiles.RebuildCompartments(true);

		tiles.SetCompartmentFloorAt(FloorType::SmoothWhite, glm::vec2(0.0f, 0.0f));
		tiles.SetCompartmentFloorAt(FloorType::Basic, glm::vec2(19.0f, 0.0f));
		tiles.SetCompartmentFloorAt(FloorType::Basic, glm::vec2(4.0f, 0.0f));
	}
	SW::VesselObject* StartingVessel::PlaceObject(ObjectType::ObjectType type, glm::vec2 location)
	{
		SW::VesselObject* obj = Vessel::PlaceObject(type, location);
		if (type == ObjectType::ObjectType::Spawner)
		{
			spawnPoints.push_back(obj);
		}
		return obj;
	}
	void StartingVessel::AddPlayer(NetworkWriter* nw, Player* player)
	{
		Vessel::AddPlayerVessel(player, nw, spawnPoints[0]->pos);
	}
}