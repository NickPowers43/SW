#include "stdafx.h"
#include "StartingVessel.h"
#include "VesselObject.h"


namespace SW_Server
{
	StartingVessel::StartingVessel(VesselVecType vel, VesselValueType m, VesselVecType pos, VesselValueType rot, Vessel* next) :
		Vessel(vel, m, pos, rot, next)
	{
		Initialize();
	}


	StartingVessel::~StartingVessel()
	{
	}

	void StartingVessel::Initialize()
	{
		BuildFoundation(glm::ivec2(-50, -50), glm::ivec2(100, 100));
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
		BuildWall(&loc0, 1, WallType::WallType::OneByOneFlipped, false);
		BuildWall(&loc0, 1, WallType::WallType::ZeroByOne, false);
		BuildWall(&loc0, 1, WallType::WallType::OneByOne, false);
		loc0 += glm::ivec2(1, 0); //BuildWall(&loc0, 1, WallType::WallType::OneByZero, false);
		BuildWall(&loc0, 1, WallType::WallType::OneByOneFlipped, true);
		BuildWall(&loc0, 1, WallType::WallType::ZeroByOne, true);
		BuildWall(&loc0, 1, WallType::WallType::OneByOne, true);
		BuildWall(&loc0, 1, WallType::WallType::OneByZero, true);
		loc0 = glm::ivec2(0, 2);
		BuildWall(&loc0, 4, WallType::WallType::ZeroByOne, false);
		BuildWall(&loc0, 1, WallType::WallType::OneByOneFlipped, false);
		BuildWall(&loc0, 1, WallType::WallType::ZeroByOne, false);
		glm::ivec2 circleInner = loc0;
		BuildWall(&loc0, 3, WallType::WallType::ZeroByOne, false);
		glm::ivec2 circleOuter = loc0;
		BuildWall(&loc0, 1, WallType::WallType::ZeroByOne, false);
		BuildWall(&loc0, 3, WallType::WallType::OneByZero, false);
		BuildWall(&loc0, 5, WallType::WallType::ZeroByOne, true);
		BuildWall(&loc0, 1, WallType::WallType::OneByOne, true);
		BuildWall(&loc0, 4, WallType::WallType::ZeroByOne, true);

		BuildWall(&circleInner, 4, WallType::WallType::OneByZero, true);
		BuildWall(&circleInner, 2, WallType::WallType::OneByOne, true);
		BuildWall(&circleInner, 11, WallType::WallType::ZeroByOne, true);
		BuildWall(&circleInner, 2, WallType::WallType::OneByOneFlipped, true);
		BuildWall(&circleInner, 11, WallType::WallType::OneByZero, false);
		BuildWall(&circleInner, 2, WallType::WallType::OneByOne, false);
		BuildWall(&circleInner, 11, WallType::WallType::ZeroByOne, false);
		BuildWall(&circleInner, 2, WallType::WallType::OneByOneFlipped, false);
		BuildWall(&circleInner, 4, WallType::WallType::OneByZero, true);

		BuildWall(&circleOuter, 5, WallType::WallType::OneByZero, true);
		BuildWall(&circleOuter, 4, WallType::WallType::OneByOne, true);
		BuildWall(&circleOuter, 13, WallType::WallType::ZeroByOne, true);
		BuildWall(&circleOuter, 4, WallType::WallType::OneByOneFlipped, true);
		BuildWall(&circleOuter, 13, WallType::WallType::OneByZero, false);
		BuildWall(&circleOuter, 4, WallType::WallType::OneByOne, false);
		BuildWall(&circleOuter, 13, WallType::WallType::ZeroByOne, false);
		BuildWall(&circleOuter, 4, WallType::WallType::OneByOneFlipped, false);
		BuildWall(&circleOuter, 5, WallType::WallType::OneByZero, true);

		//place spawner for noobs
		PlaceObject(ObjectType::ObjectType::Spawner, glm::vec2(0.0f, 0.0f));

		RebuildCompartments();

		SetCompartmentFloor(FloorType::Basic, CompartmentAt(glm::vec2(0.0f, 0.0f)));
	}
	VesselObject* StartingVessel::PlaceObject(ObjectType::ObjectType type, glm::vec2 location)
	{
		VesselObject* obj = Vessel::PlaceObject(type, location);
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