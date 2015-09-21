


using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Networking.NetworkSystem;
using System.Collections;
using System.Collections.Generic;
using Utility;


public class StartingVessel : ServerVessel
{
	private Vec2i spawnerLocation;

	public StartingVessel () :
		base()
	{
		Initialize();

		Vessels.Add(this);
	}

	public override void Update(NetworkWriter nw)
	{
		base.Update(nw);
	}

	private void Initialize()
	{
		BuildFoundation(new Vec2i(-50,-50), new Vec2i(100, 100));
		//tip
//		BuildWall(new Vec2i(4,13), 1, WallType.OneByZero);
//		//outer left
//		//
//		BuildWall(new Vec2i(1,1), 1, WallType.ZeroByOne);
//		BuildWall(new Vec2i(1,2), 1, WallType.OneByTwo);
//		BuildWall(new Vec2i(2,4), 5, WallType.ZeroByOne);
//		BuildWall(new Vec2i(2,9), 1, WallType.OneByTwo);
//		BuildWall(new Vec2i(3,11), 1, WallType.ZeroByOne);
//		BuildWall(new Vec2i(3,12), 1, WallType.OneByOne);
//		//outer right
//		//
//		BuildWall(new Vec2i(8,1), 1, WallType.ZeroByOne);
//		BuildWall(new Vec2i(8,2), 1, WallType.OneByTwoFlipped);
//		BuildWall(new Vec2i(7,4), 5, WallType.ZeroByOne);
//		BuildWall(new Vec2i(7,9), 1, WallType.OneByTwoFlipped);
//		BuildWall(new Vec2i(6,11), 1, WallType.ZeroByOne);
//		BuildWall(new Vec2i(6,12), 1, WallType.OneByOneFlipped);
//
//		BuildWall(new Vec2i(1,1), 7, WallType.OneByZero);
//
//		//inner vertical walls
//		BuildWall(new Vec2i(4,4), 7, WallType.ZeroByOne);
//		BuildWall(new Vec2i(5,4), 7, WallType.ZeroByOne);
//		//left horizontal walls
//		BuildWall(new Vec2i(3,11), 1, WallType.OneByZero);
//		BuildWall(new Vec2i(2,7), 2, WallType.OneByZero);
//		BuildWall(new Vec2i(2,4), 2, WallType.OneByZero);
//		//right horizontal walls
//		BuildWall(new Vec2i(5,11), 1, WallType.OneByZero);
//		BuildWall(new Vec2i(5,7), 2, WallType.OneByZero);
//		BuildWall(new Vec2i(5,4), 2, WallType.OneByZero);

		Vec2i loc0 = new Vec2i(0,-1);
		BuildWall(ref loc0, 1, WallType.OneByOneFlipped, false);
		BuildWall(ref loc0, 1, WallType.ZeroByOne, false);
		BuildWall(ref loc0, 1, WallType.OneByOne, false);
		loc0 += new Vec2i(1, 0); //BuildWall(ref loc0, 1, WallType.OneByZero, false);
		BuildWall(ref loc0, 1, WallType.OneByOneFlipped, true);
		BuildWall(ref loc0, 1, WallType.ZeroByOne, true);
		BuildWall(ref loc0, 1, WallType.OneByOne, true);
		BuildWall(ref loc0, 1, WallType.OneByZero, true);
		loc0 = new Vec2i(0,2);
		BuildWall(ref loc0, 4, WallType.ZeroByOne, false);
		BuildWall(ref loc0, 1, WallType.OneByOneFlipped, false);
		BuildWall(ref loc0, 1, WallType.ZeroByOne, false);
		Vec2i circleInner = loc0;
		BuildWall(ref loc0, 3, WallType.ZeroByOne, false);
		Vec2i circleOuter = loc0;
		BuildWall(ref loc0, 1, WallType.ZeroByOne, false);
		BuildWall(ref loc0, 3, WallType.OneByZero, false);
		BuildWall(ref loc0, 5, WallType.ZeroByOne, true);
		BuildWall(ref loc0, 1, WallType.OneByOne, true);
		BuildWall(ref loc0, 4, WallType.ZeroByOne, true);

		BuildWall(ref circleInner, 4, WallType.OneByZero, true);
		BuildWall(ref circleInner, 2, WallType.OneByOne, true);
		BuildWall(ref circleInner, 11, WallType.ZeroByOne, true);
		BuildWall(ref circleInner, 2, WallType.OneByOneFlipped, true);
		BuildWall(ref circleInner, 11, WallType.OneByZero, false);
		BuildWall(ref circleInner, 2, WallType.OneByOne, false);
		BuildWall(ref circleInner, 11, WallType.ZeroByOne, false);
		BuildWall(ref circleInner, 2, WallType.OneByOneFlipped, false);
		BuildWall(ref circleInner, 4, WallType.OneByZero, true);
		
		BuildWall(ref circleOuter, 5, WallType.OneByZero, true);
		BuildWall(ref circleOuter, 4, WallType.OneByOne, true);
		BuildWall(ref circleOuter, 13, WallType.ZeroByOne, true);
		BuildWall(ref circleOuter, 4, WallType.OneByOneFlipped, true);
		BuildWall(ref circleOuter, 13, WallType.OneByZero, false);
		BuildWall(ref circleOuter, 4, WallType.OneByOne, false);
		BuildWall(ref circleOuter, 13, WallType.ZeroByOne, false);
		BuildWall(ref circleOuter, 4, WallType.OneByOneFlipped, false);
		BuildWall(ref circleOuter, 5, WallType.OneByZero, true);

		//place spawner for noobs
		PlaceBlock(BlockType.Spawner, new Vec2i(0, 0));

		RebuildCompartments();
	}

	public override bool PlaceBlock(BlockType type, Vec2i location)
	{
		if (type == BlockType.Spawner) {
			spawnerLocation = location;
		}

		return base.PlaceBlock(type, location);
	}

	public void AddPlayer(ServerPlayer player, NetworkWriter nw)
	{
		base.AddPlayer(player, TileCenterToLocal(spawnerLocation), nw);
	}

	public static StartingVessel GetStartingVessel()
	{
		StartingVessel output;
		
		//find start vessel
		if (Vessels.Count == 0) {
			//create one
			
			return new StartingVessel();
		} else {
			return Vessels[0];
		}
	}
	
	private static List<StartingVessel> Vessels = new List<StartingVessel>(64);
}

